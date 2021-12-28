/*
 * rtc.c
 *
 *  Created on: May 1, 2021
 *      Author: manht
 */

#include "rtc.h"
const uint8_t daysInMonth [] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
char time_str[10]={0};				//Chuoi luu thoi gian dinh dang hh:mm:ss;
int gps_speed_count = 0;
int gps_location_count = 0;

static void RTC_NVIC_Config();
static void RTC_ClockConfig();

static uint32_t time_to_counter(RTC_Time_t Date_Time);
static struct tm counter_to_time(uint32_t rtc_counter);
void RTC_Init()
{
	RTC_NVIC_Config();
	if(BKP_ReadBackupRegister(BKP_DR1)!=0xA5A8)
	{
		RTC_Time_t Defaut_DateTime = {21,11,27,9,35,20};
		/* Backup data register value is not correct or not yet programmed (when
		 * 		       the first time the program is executed) */
		Logi("LOG: RTC not yet configured...");
		/* RTC Configuration */
		RTC_ClockConfig();
		RTC_SetCounter(time_to_counter(Defaut_DateTime));
		logi("LOG: RTC configured...");
		BKP_WriteBackupRegister(BKP_DR1, 0xA5A8);
	}
	else
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
	    /* Check if the Power On Reset flag is set */
	    if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET) logi("LOG: Power On Reset occurred.");
	    /* Check if the Pin Reset flag is set */
	    if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET) logi("LOG: External Reset occurred.");
	    logi("LOG: No need to configure RTC.");
	    /* Wait for RTC registers synchronization */
	    RTC_WaitForSynchro();
	    /* Enable the RTC Second */
	    RTC_ITConfig(RTC_IT_SEC, ENABLE);
	    /* Wait until last write operation on RTC registers has finished */
	    RTC_WaitForLastTask();
	    RCC_ClearFlag();
	}
}
static void RTC_NVIC_Config()
{
	NVIC_InitTypeDef NVIC_InitStructure;
	/* Configure one bit for preemption priority */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	/* Enable the RTC Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = SysTick_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
static void RTC_ClockConfig()
{
	/* Enable PWR and BKP clocks */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR|RCC_APB1Periph_BKP, ENABLE);
	/* Allow access to BKP Domain */
	PWR_BackupAccessCmd(ENABLE);
	/* Reset Backup Domain */
	BKP_DeInit();
	/* Enable LSE */
	RCC_LSEConfig(RCC_LSE_ON);
	/* Wait till LSE is ready */
	while (RCC_GetFlagStatus(RCC_FLAG_LSERDY)==RESET);
	/* Select LSE as RTC Clock Source */
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
	/* Enable RTC Clock */
	RCC_RTCCLKCmd(ENABLE);
	/* Wait for RTC registers synchronization */
	RTC_WaitForSynchro();
	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();
	/* Enable the RTC Second */
	RTC_ITConfig(RTC_IT_SEC, ENABLE);
	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();
	/* Set RTC prescaler: set RTC period to 1sec */
	RTC_SetPrescaler(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */
	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();
}
void RTC_SetDateTime(RTC_Time_t Date_Time)
{
	uint32_t rtc_counter = time_to_counter(Date_Time);
	PWR->CR|=(uint16_t)PWR_CR_DBP;
	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();
	RTC_EnterConfigMode();
	/* Change the current time */
	RTC_SetCounter(rtc_counter);
	RTC_ExitConfigMode();
	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();
	PWR->CR&=~(uint16_t)PWR_CR_DBP;
}
void RTC_GetDateTime(RTC_Time_t *Date_Time)
{
	uint32_t rtc_counter = RTC_GetCounter();
	struct tm time_struct = counter_to_time(rtc_counter);
	Date_Time->second = time_struct.tm_sec;
	Date_Time->minute = time_struct.tm_min;
	Date_Time->hour = time_struct.tm_hour;
	Date_Time->year = time_struct.tm_year;
	Date_Time->mon = time_struct.tm_mon;
	Date_Time->day = time_struct.tm_mday;
}

//uint32_t time_to_counter(uint32_t year, uint32_t month, uint32_t day, uint32_t hour, uint32_t minute, uint32_t second)
static uint32_t time_to_counter(RTC_Time_t Date_Time)
{
    struct tm time_struct;
    time_t m_time;
    time_struct.tm_year = Date_Time.year + 100; //1900
    time_struct.tm_mon = Date_Time.mon;
    time_struct.tm_mday = Date_Time.day;
    time_struct.tm_hour = Date_Time.hour;
    time_struct.tm_min = Date_Time.minute;
    time_struct.tm_sec = Date_Time.second;
    m_time = mktime(&time_struct);// Convert to timestamp
    return m_time;
}
static struct tm counter_to_time(uint32_t rtc_counter)
{
    struct tm time_struct;
    uint32_t yOff, m, d, hh, mm, ss;
    rtc_counter -= 946684800;//SECONDS_FROM_1970_TO_2000;    // bring to 2000 timestamp from 1970
    ss = rtc_counter % 60;
    rtc_counter /= 60;
    mm = rtc_counter % 60;
    rtc_counter /= 60;
    hh = rtc_counter % 24;
    uint16_t days = rtc_counter / 24;
    uint8_t leap;
    for (yOff = 0; ; ++yOff)
    {
        leap = yOff % 4 == 0;
        if (days < 365 + leap)
            break;
        days -= 365 + leap;
    }
    for (m = 1; ; ++m)
    {
        uint8_t daysPerMonth = daysInMonth[m - 1];
        if (leap && m == 2)
            ++daysPerMonth;
        if (days < daysPerMonth)
            break;
        days -= daysPerMonth;
    }
    d = days + 1;
    time_struct.tm_year = (yOff + 100);
    time_struct.tm_mon = m - 1;
    time_struct.tm_mday = d;
    time_struct.tm_hour = hh;
    time_struct.tm_min = mm;
    time_struct.tm_sec = ss;
    return time_struct;
}
/* Tao chuoi Time co dinh dang hh:mm:ss */
void create_time_str(RTC_Time_t *time, char *time_str)
{
	uint8_t i = 0;
	if (time->hour<10) sprintf(&time_str[0],"0%d:",time->hour);
	else sprintf(&time_str[0],"%d:",time->hour);
	i = strlen(time_str);
	if (time->minute<10) sprintf(&time_str[i],"0%d:",time->minute);
	else sprintf(&time_str[i],"%d:",time->minute);
	i = strlen(time_str);
	if (time->second<10) sprintf(&time_str[i],"0%d",time->second);
	else sprintf(&time_str[i],"%d",time->second);
}
void create_date_str(RTC_Time_t *time, char *date_str){
	uint8_t i = 0;
	if (time->day<10) sprintf(&date_str[0],"0%d/",time->day);
	else sprintf(&date_str[0],"%d/",time->day);
	i = strlen(date_str);
	if (time->mon+1<10) sprintf(&date_str[i],"0%d/",time->mon+1);
	else sprintf(&date_str[i],"%d/",time->mon+1);
	i = strlen(date_str);
	sprintf(&date_str[i],"%d",time->year+1900);
}
