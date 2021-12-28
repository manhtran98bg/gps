/*
 * board.h
 *
 *  Created on: 27 thg 12, 2021
 *      Author: manht
 */

#ifndef SERVICES_BOARD_BOARD_H_
#define SERVICES_BOARD_BOARD_H_

#include "main.h"
#include "delay/delay.h"
#define BUZZER_PORT	GPIOA
#define BUZZER_PIN	GPIO_Pin_1
#define LED_PORT	GPIOB
#define LED_PIN		GPIO_Pin_9
#define B3_PORT		GPIOB
#define B3_PIN		GPIO_Pin_0
#define B2_PORT		GPIOC
#define B2_PIN		GPIO_Pin_4
#define B1_PORT		GPIOC
#define B1_PIN		GPIO_Pin_5
void board_init();
void buzzer_beep();
#endif /* SERVICES_BOARD_BOARD_H_ */
