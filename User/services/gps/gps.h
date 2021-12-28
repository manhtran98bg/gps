/*
 * gps.h
 *
 *  Created on: Apr 12, 2021
 *      Author: manht
 */

#ifndef GPS_GPS_H_
#define GPS_GPS_H_

#include "main.h"
#include "usart/usart.h"
#include "func.h"
#include "io/io.h"
#include "fifo.h"
#include "delay/delay.h"
#define GPS_BUFFER_MAX_SIZE 1024
#define GPS_DELAY	200

/*----------------------------------------- Define UART4 ----------------------------------*/
#define GPS_RST_PORT	GPIOC
#define GPS_RST_PIN		GPIO_Pin_5

#define GPS_PWR_PORT	GPIOA
#define GPS_PWR_PIN		GPIO_Pin_15


/* UART4 For GPS L70R*/
#define GPS_UART		UART4
#define GPS_GPIO		GPIOC
#define GPS_TX			GPIO_Pin_10
#define GPS_RX			GPIO_Pin_11

typedef enum{
	OFF,
	ON
}pwr_state;
typedef enum {
	FUNC_INIT,
}gps_func_t;
typedef struct {
	int ddd;
	uint8_t mm;
	int mmmm;
	char dec_degree[16];
}location_t;
typedef struct {
	char id[7];
	struct tm time;
	char time_chr[11];
	char data_valid[2];
	location_t lat;
	char lat_dir[2];
	char lat_chr[10];
	location_t lon;
	char lon_dir[2];
	char lon_chr[11];
	double speed;
	char speed_chr[6];
}rmc_t;
typedef struct{
	pwr_state pwr_state ;
	uint8_t inited : 1;
	uint8_t gps_time_sync : 1;
	int8_t retry;
	uint8_t data_in_tmp;
	uint8_t nmea_start : 1;
	uint8_t response : 1;
	uint8_t nmea_received : 1;
	uint16_t data_in_index ;
	uint32_t data_in_last_time;
	uint32_t gps_wait_time;
	UART_Init_Struct *huart;
	void * UARTx;
	uint32_t baudrate;
	rmc_t	rmc;
	uint8_t data_in[GPS_BUFFER_MAX_SIZE];
	fifo_t	cbuffer;
	uint8_t nmea_buffer[256];
	uint16_t nmea_index;
	uint16_t nmea_size;
	gps_func_t func;
}gps_t;

typedef void (*rx_callbackFunction)(void);

void gps_init(gps_t *gps);
void gps_receive_data_it(gps_t *gps);
uint8_t gps_process_data(gps_t *gps);
void gps_register_rxCallback(rx_callbackFunction callback);
#endif /* GPS_GPS_H_ */
