/*
 * gps.c
 *
 *  Created on: Apr 12, 2021
 *      Author: manht
 */

#include "gps.h"


static gpio_t *gps_pwr_pin;
rx_callbackFunction ptrFuntion = NULL;

UART_Init_Struct gps_uart;
static void gps_uart_init(UART_Init_Struct *huart, USART_TypeDef *UARTx, uint32_t baudrate);
static void gps_pwr_pin_init();
static uint8_t gps_set_baudrate(gps_t *gps, uint32_t baudrate);
static uint8_t gps_check_current_baudrate(gps_t *gps, uint32_t baudrate);
static void gps_send_cmd(gps_t *gps, char *cmd, uint16_t size);
static uint8_t gps_nmea_parser(gps_t *gps);

static void gps_uart_init(UART_Init_Struct *huart, USART_TypeDef *UARTx, uint32_t baudrate){
	usart_init(huart, UARTx, GPS_GPIO, GPS_TX, GPS_GPIO, GPS_RX, 0, baudrate);
}
static void gps_pwr_pin_init(){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	GPIO_InitTypeDef GPIO_initstruct;
	GPIO_initstruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_initstruct.GPIO_Pin = GPS_PWR_PIN;
	GPIO_initstruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_initstruct);
	GPIO_SetBits(GPIOA, GPS_PWR_PIN);

}
static void gps_send_cmd(gps_t *gps, char *cmd, uint16_t size){
	usart_send_buffer(gps->huart, (uint8_t*)cmd, size);
}

static uint8_t gps_set_baudrate(gps_t *gps, uint32_t baudrate){
	char cmdBuf[25]={0};
	logi("Change baudrate %ld -> %ld",gps->baudrate, baudrate);
	sprintf(cmdBuf,"$PMTK251,%ld*1F\r\n",baudrate);
	gps_send_cmd(gps, cmdBuf, sizeof(cmdBuf));
	delay_ms(200);
	return gps_check_current_baudrate(gps, baudrate);
}

static uint8_t gps_check_current_baudrate(gps_t *gps, uint32_t baudrate){
	logi("Try baudrate %ld.",baudrate);
	CBUFFER_Reset(&gps->cbuffer);
	gps->response = 0;
	gps_uart_init(gps->huart, (USART_TypeDef*)gps->UARTx,baudrate);
	gps->gps_wait_time = get_tick() + 3000;
	while (!gps->response){
		gps_process_data(gps);
		delay_ms(100);
		if (gps->gps_wait_time < get_tick()) break;
	}
	if (gps->response){
		logi("Module response at baudrate %ld.",baudrate);
		gps->baudrate = baudrate;
		return 1;
	}
	return 0;
}

void gps_pwr_on(gps_t *gps){
	logi("Power on GPS Module.");
	if (gps->pwr_state == OFF){
		GPIO_WriteBit(GPS_PWR_PORT, GPS_PWR_PIN, Bit_RESET);	//pull pin to GND to turn on GPS Module
		gps->pwr_state = ON;
	}
}

void gps_pwr_off(gps_t *gps){
	logi("Power off GPS Module.");
	if (gps->pwr_state == OFF){
		GPIO_WriteBit(GPS_PWR_PORT, GPS_PWR_PIN, Bit_SET);		//pull pin to GND to turn on GPS Module
		gps->pwr_state = ON;
	}
	gpio_set_state(gps_pwr_pin, 0);		//pull pin to VCC to turn off GPS Module
}
uint8_t gps_auto_baudrate(gps_t *gps){
	uint32_t baudrate[2]={115200,9600};
	int i;
	for (i=0; i<2; i++){
		if (gps_check_current_baudrate(gps, baudrate[i])) return 1;
	}
	return 0;
}
void gps_init(gps_t *gps){
	gps_pwr_pin_init();
	gps_pwr_on(gps);
	delay_ms(500);
	gps->UARTx = GPS_UART;
	gps->huart = &gps_uart;
	gps->baudrate = 9600;
	gps->pwr_state = ON;
	gps->data_in[0] = 0;
	gps->data_in_index = 0;
	gps->nmea_buffer[0] = 0;
	gps->nmea_index = 0;
	gps->response = 0;
	gps->inited = 0;
	CBUFFER_Init(&gps->cbuffer, gps->data_in, GPS_BUFFER_MAX_SIZE);
	uint8_t ret = gps_auto_baudrate(gps);
	if (!ret){
		loge("Module no response.");
	}
	if (gps->baudrate == 9600){
		gps_set_baudrate(gps, 115200);
	}
	logi("Config GPS module");
	char cmd[7][64] = {	"$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29\r\n", //Set GPRMC Sentence Output
						"$PMTK886,0*28\r\n", //Enter Vehicle Mode
						"$PMTK386,0.9*34\r\n", //Setting speed threshold
						"$PMTK869,1,1*35\r\n", //Enable EASY
						"$PMTK353,1,1,1,0,0*2A\r\n", //Search GPS,GLONASS,GALILEO satellites
						"$PMTK352,0*2A\r\n", //Enable QZSS Function
						"$PMTK286,1*23\r\n", //Enable active interference cancellation function
	};
	for (int i=0; i<7; i++){
		gps_send_cmd(gps, cmd[i], sizeof(cmd[i]));
		delay_ms(GPS_DELAY);
	}
	gps->inited = 1;
}
uint8_t gps_process_data(gps_t *gps){
	uint8_t tmp_data;
	uint32_t wait_time = get_tick() + 200;
	if (CBUFFER_Available(&gps->cbuffer)){
		while(tmp_data != '$'){
			CBUFFER_Getc(&gps->cbuffer, &tmp_data);
			if (wait_time < get_tick()) break;
		}
		if (tmp_data == '$'){
			gps->nmea_index = 0;
			gps->nmea_buffer[gps->nmea_index++] = '$';
			do {
				if (CBUFFER_Getc(&gps->cbuffer, &tmp_data)){
					gps->nmea_buffer[gps->nmea_index++] = tmp_data;
					gps->nmea_buffer[gps->nmea_index] = 0;
				}
			} while (tmp_data != '\n');
			gps->nmea_buffer[gps->nmea_index-1] = 0;
			logi("%s",gps->nmea_buffer);
			gps->nmea_size = gps->nmea_index +1;
			gps->response = 1;
			gps->nmea_index = 0;
			return gps_nmea_parser(gps);
		}
	}
	return 0;
}
static uint8_t gps_nmea_get_time(gps_t *gps, char *time_str){
	int i=0;
	char hour[3]={0};
	char minute[3]={0};
	char second[3]={0};
	if (strlen(time_str) == 0) return 0;
	for (i=0;i<2;i++) hour[i]=time_str[i];
	gps->rmc.time.tm_hour = atoi(hour);
	for (i=0;i<2;i++) minute[i]=time_str[i+2];
	gps->rmc.time.tm_min = atoi(minute);
	for (i=0;i<2;i++) second[i]=time_str[i+4];
	gps->rmc.time.tm_sec = atoi(second);
	return 1;
}
static uint8_t gps_nmea_get_date(gps_t *gps, char *date_str){
	int i=0;
	char dd[3]={0};
	char mm[3]={0};
	char yy[3]={0};
	if (strlen(date_str) == 0) return 0;
	for (i=0;i<2;i++) dd[i]=date_str[i];
	gps->rmc.time.tm_mday = atoi(dd);
	for (i=0;i<2;i++) mm[i]=date_str[i+2];
	gps->rmc.time.tm_mon = atoi(mm);
	for (i=0;i<2;i++) yy[i]=date_str[i+4];
	gps->rmc.time.tm_year = atoi(yy);
	return 1;
}
static uint8_t gps_nmea_get_latitude(gps_t *gps, char *lat_str){
	char lat_dd[3]={0};
	char lat_mm[3]={0};
	char lat_mmmm[5]={0};
	double M_m=0,D_d=0,Decimal_Degree=0;
	int i=0,j=0;
	if (strlen(lat_str)==0)
	{
		gps->rmc.lat.ddd = 0;
		gps->rmc.lat.mm = 0;
		gps->rmc.lat.mmmm = 0;
		memset(gps->rmc.lat.dec_degree,0,16);
		return 0;
	}
	else
	{
		for (i=0;i<2;i++) lat_dd[j++]=lat_str[i];
		j=0;
		for (i=0;i<2;i++) lat_mm[j++]=lat_str[i+2];
		j=0;
		for (i=0;i<4;i++) lat_mmmm[j++]=lat_str[i+5];
		gps->rmc.lat.ddd = atoi(lat_dd);
		gps->rmc.lat.mm = atoi(lat_mm);
		gps->rmc.lat.mmmm = atoi(lat_mmmm);
		M_m = gps->rmc.lat.mm + (double)gps->rmc.lat.mmmm/10000.0;
		D_d = (double)M_m/60.0;
		Decimal_Degree = (double)(gps->rmc.lat.ddd+D_d);
		ftoa(Decimal_Degree, gps->rmc.lat.dec_degree, 5);
	}
	return 1;
}
static uint8_t gps_nmea_get_longtitude(gps_t *gps, char *lon_str){
	char lon_ddd[4]={0};
	char lon_mm[3]={0};
	char lon_mmmm[5]={0};
	double M_m=0,D_d=0,Decimal_Degree=0;
	int i=0,j=0;
	if (strlen(lon_str)==0)
	{
		gps->rmc.lon.ddd = 0;
		gps->rmc.lon.mm = 0;
		gps->rmc.lon.mmmm = 0;
		memset(gps->rmc.lon.dec_degree,0,16);
		return 0;
	}
	else
	{
		for (i=0;i<3;i++) lon_ddd[j++]=lon_str[i];
		j=0;
		for (i=0;i<2;i++) lon_mm[j++]=lon_str[i+3];
		j=0;
		for (i=0;i<4;i++) lon_mmmm[j++]=lon_str[i+6];
		gps->rmc.lon.ddd = atoi(lon_ddd);
		gps->rmc.lon.mm = atoi(lon_mm);
		gps->rmc.lon.mmmm = atoi(lon_mmmm);
		M_m = gps->rmc.lon.mm + (double)gps->rmc.lon.mmmm/10000.0;
		D_d = (double)M_m/60.0;
		Decimal_Degree = (double) (gps->rmc.lon.ddd + D_d);
		ftoa(Decimal_Degree, gps->rmc.lon.dec_degree, 8);
	}
	return 1;
}
static double str2float(char *str){
	int int_part = 0,dec_part = 0;
	unsigned int i=0,j=0,lt=1;
	char int_chr[10]={0};
	char dec_chr[10]={0};
	double knot=0;
	while (str[i]!='.') {
	    int_chr[i]=str[i];
	    i++;
	}
	i++;
	while (i<strlen(str)){
	    dec_chr[j++]=str[i];
	    i++;
	}
	int_part = atoi(int_chr);
    dec_part = atoi(dec_chr);
    for (i=0;i<strlen(dec_chr);i++) lt=lt*10;
    knot = (double)(int_part+(double)dec_part/lt);
    return (double)knot*1.852;
}
static void gps_fix_time(gps_t *gps){
	int day_of_mon[12]={31,28,31,30,31,30,31,31,30,31,30,31};
	if (gps->rmc.time.tm_hour+7>23) {
		gps->rmc.time.tm_hour = gps->rmc.time.tm_hour+7-24;
		gps->rmc.time.tm_mday = gps->rmc.time.tm_mday+1;
	}
	else gps->rmc.time.tm_hour = gps->rmc.time.tm_hour+7;
	if (gps->rmc.time.tm_mday > day_of_mon[gps->rmc.time.tm_mon-1]){
		gps->rmc.time.tm_mday = 1;
		gps->rmc.time.tm_mon = gps->rmc.time.tm_mon+1;
	}
}
static uint8_t gps_nmea_parser(gps_t *gps){
	int i = 0, index = 0, k = 0;
	char tmp[15]={0};
	if (gps->nmea_buffer[0] != '$') return 0;
	while (i<gps->nmea_size){
		for (index = 0; index < 15; index++) tmp[index] = 0;
		index = 0;
		while (i < gps->nmea_size && gps->nmea_buffer[i] != ','){
			tmp[index++] = gps->nmea_buffer[i];
			i++;
		}
		i++;
		if ( k == 0 ) strcpy(gps->rmc.id, tmp);
		if (strcmp(gps->rmc.id, "$GPRMC") != 0 ) return 0;
		if (k == 1) gps_nmea_get_time(gps, tmp);
		if (k == 2) strcpy(gps->rmc.data_valid,tmp);
		if (k == 3) gps_nmea_get_latitude(gps, tmp);
		if (k == 4) strcpy(gps->rmc.lat_dir,tmp);
		if (k == 5) gps_nmea_get_longtitude(gps, tmp);
		if (k == 6) strcpy(gps->rmc.lon_dir,tmp);
        if (k == 7 && strlen(tmp) > 0) gps->rmc.speed = str2float(tmp);
        if (k == 9) gps_nmea_get_date(gps, tmp);
        k++;
	}
	gps_fix_time(gps);
	return k;
}
void gps_register_rxCallback(rx_callbackFunction callback){
	ptrFuntion = callback;
}
void UART4_IRQHandler(void)
{
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)
	{
		ptrFuntion();
	}
}
