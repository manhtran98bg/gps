/*
 * delay.h
 *
 *  Created on: Apr 10, 2021
 *      Author: manht
 */

#ifndef SERVICE_DELAY_H_
#define SERVICE_DELAY_H_
#include "main.h"

extern volatile uint32_t uwTicks;
#define MAX_DELAY      0xFFFFFFFFU
typedef enum
{
  TICK_FREQ_10HZ         = 100U,
  TICK_FREQ_100HZ        = 10U,
  TICK_FREQ_1KHZ         = 1U,
  TICK_FREQ_DEFAULT      = TICK_FREQ_1KHZ
} TickFreqTypeDef;
void tim4_init();
void tim5_init();
void delay_ms(uint32_t ms);
void dUS_tim4(uint16_t uS);
uint32_t get_tick(void);
#endif /* SERVICE_DELAY_H_ */
