/*
 * rtc.h
 *
 *  Created on: May 1, 2021
 *      Author: manht
 */

#ifndef RTC_RTC_H_
#define RTC_RTC_H_
#include "main.h"
typedef struct{
	uint8_t year;
	uint8_t mon;
	uint8_t day;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
}RTC_Time_t;
void RTC_Init();
void RTC_SetDateTime(RTC_Time_t Date_Time);
void RTC_GetDateTime(RTC_Time_t *Date_Time);
void create_time_str(RTC_Time_t *time, char *time_str);
void create_date_str(RTC_Time_t *time, char *date_str);
#endif /* RTC_RTC_H_ */
