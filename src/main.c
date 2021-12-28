/*
 * This file is part of the µOS++ distribution.
 *   (https://github.com/micro-os-plus)
 * Copyright (c) 2014 Liviu Ionescu.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

// ----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "clock/clock.h"
#include "usart/usart.h"
#include "delay/delay.h"
#include "sensor/dht11.h"
#include "sensor/ds18b20.h"
#include "gps/gps.h"
#include "lcd/sh1106.h"
#include "sdcard/sdmm.h"
#include "rtc/rtc.h"
#include "event_groups.h"
#include "rfid/mfrc552.h"
#include "board/board.h"

typedef struct {
	RTC_Time_t 	Start_time;
	RTC_Time_t	Stop_time;
	uint32_t work_time;
	uint8_t	id[5];
	bool log_in;
	char *name;
}driver_info;
driver_info driver[3];
UART_Init_Struct	log_uart;
gps_t gps_l70;
#ifdef LOG_EN

int __io_putchar(int ch)
{
    usart_send_char(&log_uart, ch);
	return ch;
}
#endif
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

TaskHandle_t sensor_task_handle;
TaskHandle_t gps_task_handle;
TaskHandle_t one_sec_task_handle;
TaskHandle_t display_task_handle;
TaskHandle_t rtc_sync_task_handle;
TaskHandle_t rfid_read_task_handle;
EventGroupHandle_t rtc_config_event_gr;

dht11_data	dht11;
TM_OneWire_t OneWire1;
ds18b20_t ds18b20;
oled_t oled;
sdCard_t sdcard;
RTC_Time_t Time;
uint8_t screen = 0;	//Screen = 0 Display GPS info
					//Screen = 1 Display SystemCheck
					//Screen = 2 Display Driver info (RFID)
static void sensor_task(void *param);
static void gps_task(void *param);
static void display_task(void *param);
static void rtc_sync_task(void *param);
static void sdcard_check();
static void print_start_result_screen();
static void print_gps_info_screen();
static void print_driver_info_screen();
static void driver_info_init(driver_info *driver);
static int driver_check_id(driver_info *driver, uint8_t *id);

static void gps_rx_callback(void);

static void sensor_task(void *param){
	TickType_t xLastWakeTime;
	const TickType_t xFrequency = 1000;
	xLastWakeTime = xTaskGetTickCount ();
	while(1){
		vTaskSuspendAll();
		if (ds18b20_read_temp(&OneWire1, &ds18b20)){
			logi("Temp:%d",(int)ds18b20.temp);
		}
		else logi("Read DS18B20 Failed");
		xTaskResumeAll();
		vTaskDelayUntil( &xLastWakeTime, xFrequency);
	}
}
static void gps_task(void *param){
	while(1){
		if (gps_process_data(&gps_l70))
			if (gps_l70.rmc.data_valid[0] == 'A') xEventGroupSetBits(rtc_config_event_gr, GPS_FIX_BIT);
		vTaskDelay(10);
	}
}
static void rtc_sync_task(void *param){
	EventBits_t uxBit;
	while(1){
		if (rtc_config_event_gr !=NULL){
			uxBit = xEventGroupWaitBits(rtc_config_event_gr, GPS_FIX_BIT, pdTRUE, pdFALSE, portMAX_DELAY);
			if (uxBit & GPS_FIX_BIT){
				logi("Sync RTC Task begin");
				Time.second = gps_l70.rmc.time.tm_hour;
				Time.minute = gps_l70.rmc.time.tm_min;
				Time.hour = gps_l70.rmc.time.tm_hour;
				Time.year = gps_l70.rmc.time.tm_year;
				Time.mon = gps_l70.rmc.time.tm_mon-1;
				Time.day = gps_l70.rmc.time.tm_mday;
				RTC_SetDateTime(Time);
				vTaskDelete(NULL);
			}
		}
	}
}
static void rfid_read_task(void *param){
	char str_id[32]={0};
	uint8_t str[16]; // Max_LEN = 16
	uint8_t serNum[5];
	uint8_t status;
	int i = 0;
	for(;;){
		status = MFRC522_Request(PICC_REQIDL, (uchar*)str);
		if (status == MI_OK) {
			status = MFRC522_Anticoll(str);
			memcpy(serNum, str, 5);
			sprintf(str_id,"0x%.2x 0x%.2x 0x%.2x 0x%.2x 0x%.2x",serNum[0], serNum[1], serNum[2], serNum[3],serNum[4]);
			logi("LOG: Find out a card, ID = %s",str_id);
			buzzer_beep();
			i = driver_check_id(driver, serNum);
			if (i != -1){
				logi("LOG: %d",i);
				driver[i].log_in = true;
				driver[i].Start_time.hour = Time.hour;
				driver[i].Start_time.minute = Time.minute;
				driver[i].Start_time.second = Time.second;
				driver[i].work_time = 0;
			}

			vTaskDelay(3000);
		}
		vTaskDelay(10);
	}
}
static void display_task(void *param){
	TickType_t xLastWakeTime;
	const TickType_t xFrequency = 1000;
	xLastWakeTime = xTaskGetTickCount ();
	while(1){
		switch (screen){
		case 0:
			sh1106_Clear(&oled, Black);
			while(screen == 0) {
				print_gps_info_screen();
				vTaskDelayUntil( &xLastWakeTime, xFrequency);
			}
			break;
		case 1:
			sh1106_Clear(&oled, Black);
			while(screen == 1) {
				print_start_result_screen();
				vTaskDelayUntil( &xLastWakeTime, xFrequency);
			}
			break;
		case 2:
			break;
		}
		vTaskDelayUntil( &xLastWakeTime, xFrequency);
	}
}
static void gps_rx_callback(void){
	gps_l70.data_in_tmp = USART_ReceiveData(UART4);
	CBUFFER_Putc(&gps_l70.cbuffer, gps_l70.data_in_tmp);
}
int main(int argc, char* argv[])
{
	clock_init();
	tim5_init();
	tim4_init();
	initIO();
	driver_info_init(driver);
	usart_init(&log_uart, UART5, GPIOC, GPIO_Pin_12, GPIOD, GPIO_Pin_2, 0, 115200);
	RTC_Init();
	board_init();
	sh1106_Init(&oled);
	sh1106_DrawBitmap(&oled, 32, 16, Gps_logo);
	logi(INFO_FM, FIRMWARE_VERSION, HARDWARE_VERSION, __DATE__, __TIME__);
	gps_register_rxCallback(gps_rx_callback);
	gps_init(&gps_l70);
	ds18b20_init(&OneWire1, &ds18b20);
	MFRC522_Init();
	sdcard_check();
	sh1106_Clear(&oled, Black);
	print_start_result_screen();
	delay_ms(2000);
	rtc_config_event_gr = xEventGroupCreate();
	xTaskCreate(sensor_task, "SENSOR_TASK", 400, NULL, 3, &sensor_task_handle);
	xTaskCreate(gps_task, "GPS_TASK", 400, NULL, 2, &gps_task_handle);
	xTaskCreate(display_task, "DISPLAY_TASK", 800, NULL, 2, &display_task_handle);
	xTaskCreate(rtc_sync_task, "RTC_SYNC_TASK", 200, NULL, 3, &rtc_sync_task_handle);
	xTaskCreate(rfid_read_task, "RFID_READ_TASK", 200, NULL, 3, &rfid_read_task_handle);
	gps_l70.rmc.data_valid[0] = 'V';
	vTaskStartScheduler();
	while(1){

	}
}
static void sdcard_check()
{
	TCHAR label[10];
	DWORD	vsn;
	FATFS *fs;
	DWORD fre_clust, fre_sect, tot_sect;
	FRESULT	fr;
	double percent;
	sdcard.mount =false;
	fr = f_mount(&sdcard.Fatfs, "", 1);
	if(fr == FR_OK){
		f_getlabel("", label, &vsn);
		f_getfree("", &fre_clust, &fs);
	    tot_sect = (fs->n_fatent - 2) * fs->csize;
	    fre_sect = fre_clust * fs->csize;
	    percent = (double)(fre_sect*100/tot_sect);
	    sdcard.free_size = (int)percent;
		sdcard.mount = true;
		sdcard.serNum = vsn;
		sdcard.size = tot_sect / (2.0*1024.0*1024.0);
		sdcard.size_int = (int)sdcard.size;
		sdcard.size_dec = (sdcard.size - sdcard.size_int)*100;
		strcpy(sdcard.label,label);
		logi("LOG: Mount SDCard OK. Label [%s]. Serial [%lu]",label,vsn);
		logi("LOG: Total drive space: %d.%d GB",sdcard.size_int,sdcard.size_dec);
		logi("LOG: SDcard free %d %%",(int)percent);
	}
	else loge("LOG: Mount SDCard Fail.\r\n");
}
static void print_start_result_screen(){
	char *buf = malloc(20);
	sh1106_WriteString(&oled, 0, 0, "CHECKING SYSTEM", Font_6x8, White, ALIGN_CENTER);
	sprintf(buf,"SENSOR: %d",OneWire1.Devices);
	sh1106_WriteString(&oled, 0, 15, buf, Font_6x8, White, ALIGN_LEFT);
	if (!sdcard.mount)
		sh1106_WriteString(&oled, 0, 30, "SDCard: Failed", Font_6x8, White, ALIGN_LEFT);
	else {
		memset(buf,0,20);
		sprintf(buf,"SD:%d.%d GB Free:%d%%",sdcard.size_int, sdcard.size_dec, sdcard.free_size);
		sh1106_WriteString(&oled, 0, 30, buf, Font_6x8, White, ALIGN_LEFT);
	}
	if (gps_l70.inited) sh1106_WriteString(&oled, 0, 45, "GPS: NO ERROR", Font_6x8, White, ALIGN_LEFT);
	else sh1106_WriteString(&oled, 0, 45, "GPS: FAILED", Font_6x8, White, ALIGN_LEFT);
	sh1106_UpdateScreen(&oled);
}
static void print_gps_info_screen(){
	char buffer[32]={0};
	memset(buffer,0,32);
	RTC_GetDateTime(&Time);
	create_time_str(&Time, buffer);
	sh1106_WriteString(&oled, 2, 0, buffer, Font_6x8, White, NO_ALIGN);
	memset(buffer,0,32);
	create_date_str(&Time, buffer);
	sh1106_WriteString(&oled, 70, 0, buffer, Font_6x8, White, NO_ALIGN);
	sh1106_DrawLine(&oled, 2, 10, 129, 10, White);
	sh1106_DrawLine(&oled, 2, 10, 2, 63, White);
	sh1106_DrawLine(&oled, 2, 63, 129, 63, White);
	sh1106_DrawLine(&oled, 129, 63, 129, 10, White);
	sh1106_DrawLine(&oled, 85, 10, 85, 63, White);

	if(gps_l70.rmc.data_valid[0] == 'V') {
		sh1106_WriteString(&oled, 4, 12, "GPS: No Fix", Font_6x8, White, NO_ALIGN);
	}
	if(gps_l70.rmc.data_valid[0] == 'A') {
		sh1106_WriteString(&oled, 4, 12, "GPS: Fix   ", Font_6x8, White, NO_ALIGN);
	}
	memset(buffer,0,32);
	sprintf(buffer,"LAT:%s",gps_l70.rmc.lat.dec_degree);
	sh1106_WriteString(&oled, 4, 22, buffer, Font_6x8, White, NO_ALIGN);
	memset(buffer,0,32);
	sprintf(buffer,"LON:%s",gps_l70.rmc.lon.dec_degree);
	sh1106_WriteString(&oled, 4, 32, buffer, Font_6x8, White, NO_ALIGN);
	memset(buffer,0,32);
	sprintf(buffer,"SPEED:%d Km/h",(int)gps_l70.rmc.speed);
	sh1106_WriteString(&oled, 4, 42, buffer, Font_6x8, White, NO_ALIGN);
	memset(buffer,0,32);
	sprintf(buffer," T:%d",(int)ds18b20.temp);
	sh1106_WriteString(&oled, 87, 32, buffer, Font_6x8, White, NO_ALIGN);
	memset(buffer,0,32);
	sprintf(buffer," H:69%%");
	sh1106_WriteString(&oled, 87, 42, buffer, Font_6x8, White, NO_ALIGN);
	sh1106_WriteString(&oled, 87, 52, " V:12.1", Font_6x8, White, NO_ALIGN);
	if (sdcard.mount) {
		memset(buffer,0,32);
		sprintf(buffer,"%d.%d",sdcard.size_int,sdcard.size_dec);
		sh1106_WriteString(&oled, 103, 13,buffer, Font_6x8, White, NO_ALIGN);
		memset(buffer,0,32);
		sprintf(buffer,"%d%%",sdcard.free_size);
		sh1106_WriteString(&oled, 103, 22,buffer, Font_6x8, White, NO_ALIGN);
		sh1106_DrawBitmap(&oled, 87, 14, sdcard_status[0]);
	}
	else sh1106_DrawBitmap(&oled, 87, 14, sdcard_status[1]);
	sh1106_UpdateScreen(&oled);
}
static void print_driver_info_screen(){

}
static void driver_info_init(driver_info *driver){
	static const uint8_t id_0[5]={0xc9,0xac,0x5a,0xb2,0x8d};
	static const uint8_t id_1[5]={0x65,0x27,0x0e,0xad,0xe1};
	static const uint8_t id_2[5]={0x5a,0x8c,0xe1,0x0b,0x3c};
	memcpy(driver[0].id,id_0,5);
	memcpy(driver[1].id,id_1,5);
	memcpy(driver[2].id,id_2,5);
	driver[0].log_in = 0;
	driver[1].log_in = 0;
	driver[2].log_in = 0;
	driver[0].name = "Hung Manh";
	driver[1].name = "Quoc Hung";
	driver[2].name = "Ba Dat";
}
static int driver_check_id(driver_info *driver_info, uint8_t *id){
	for (int i = 0; i < 3; i++){
		uint8_t ret = 0;
		for (int j = 0; j < 5; j++)
			if (id[j] == driver_info[i].id[j]) ret++;
		if (ret == 5) return i;
	}
	return -1;
}
#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
