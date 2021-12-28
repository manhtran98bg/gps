/*
 * delay.c
 *
 *  Created on: Apr 10, 2021
 *      Author: manht
 */
#include "delay.h"
volatile uint32_t uwTicks;
void tim5_init(){
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_ClockDivision = 0;
	TIM_TimeBaseInitStruct.TIM_Prescaler = 71;
	TIM_TimeBaseInitStruct.TIM_Period = 999;
	TIM_TimeBaseInit(TIM5,&TIM_TimeBaseInitStruct);
	TIM_ClearFlag(TIM5, TIM_FLAG_Update);
	TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);
	NVIC_EnableIRQ(TIM5_IRQn);
	TIM_Cmd(TIM5, ENABLE);
}
void tim4_init()
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_ClockDivision = 0;
	TIM_TimeBaseInitStruct.TIM_Prescaler = 71;
	TIM_TimeBaseInitStruct.TIM_Period = 0xffff-1;
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStruct);
	TIM_ClearFlag(TIM4, TIM_FLAG_Update);
	TIM_Cmd(TIM4, ENABLE);
}

void delay_ms(uint32_t ms)
{
	 uint32_t tickstart = get_tick();
	 uint32_t wait = ms;
	 while ((get_tick() - tickstart) < wait){
	 }
}
void dUS_tim4(uint16_t uS)
{
	TIM_SetCounter(TIM4, 0);
	TIM4->CR1 |=TIM_CR1_CEN;	//Start TIM4
	while (TIM_GetCounter(TIM4)<uS);
	TIM4->CR1 &= (uint16_t)(~((uint16_t)TIM_CR1_CEN));	//Stop TIM4
}
uint32_t get_tick(void){
	return uwTicks;
}
