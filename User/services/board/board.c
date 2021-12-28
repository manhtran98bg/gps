/*
 * board.c
 *
 *  Created on: 27 thg 12, 2021
 *      Author: manht
 */
#include "board.h"

static void user_led_init(){
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = LED_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(LED_PORT, &GPIO_InitStruct);
}
static void buzzer_init(){
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = BUZZER_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(BUZZER_PORT, &GPIO_InitStruct);
}
void buzzer_beep(){
	GPIOA->ODR |=BUZZER_PIN;
	vTaskDelay(50);
	GPIOA->ODR &=~BUZZER_PIN;
}
static void button_init(){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_InitTypeDef gpio_init_struct;
	EXTI_InitTypeDef exti_init_struct;
	NVIC_InitTypeDef nvic_init_struct;
	gpio_init_struct.GPIO_Pin = B3_PIN|B2_PIN|B1_PIN;
	gpio_init_struct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	gpio_init_struct.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOB, &gpio_init_struct);
	/* Connect EXTI0 Line to PB0 pin */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource0);
	/* Connect EXTI4 Line to PC4 pin */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource4);
	/* Connect EXTI4 Line to PC4 pin */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource5);
	/* Configure EXTI line */
	exti_init_struct.EXTI_Line = EXTI_Line0|EXTI_Line4|EXTI_Line5;
	exti_init_struct.EXTI_Mode = EXTI_Mode_Interrupt;
	exti_init_struct.EXTI_Trigger = EXTI_Trigger_Falling;
	exti_init_struct.EXTI_LineCmd = ENABLE;
	EXTI_Init(&exti_init_struct);
	nvic_init_struct.NVIC_IRQChannel = EXTI0_IRQn;
	nvic_init_struct.NVIC_IRQChannelPreemptionPriority = 1;
	nvic_init_struct.NVIC_IRQChannelSubPriority = 0;
	nvic_init_struct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic_init_struct);
	nvic_init_struct.NVIC_IRQChannel = EXTI4_IRQn;
	nvic_init_struct.NVIC_IRQChannelPreemptionPriority = 1;
	nvic_init_struct.NVIC_IRQChannelSubPriority = 0;
	nvic_init_struct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic_init_struct);
	nvic_init_struct.NVIC_IRQChannel = EXTI9_5_IRQn;
	nvic_init_struct.NVIC_IRQChannelPreemptionPriority = 1;
	nvic_init_struct.NVIC_IRQChannelSubPriority = 0;
	nvic_init_struct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic_init_struct);
}
void board_init(){
	user_led_init();
	buzzer_init();
	button_init();
}
