
#include "stm32f10x_it.h"
#include "delay/delay.h"
#include "gps/gps.h"
extern uint8_t screen;
void NMI_Handler(void)
{

}
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

void TIM4_IRQHandler(void)
{

}

void TIM5_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM5, TIM_IT_Update)!=RESET)
	{
		uwTicks++;
		TIM_ClearFlag(TIM5, TIM_FLAG_Update);
	}
}

void USART1_IRQHandler(void)
{
}
void USART2_IRQHandler(void)
{
}
void RTC_IRQHandler(void)
{
	if (RTC_GetITStatus(RTC_IT_SEC) != RESET){
		RTC_ClearITPendingBit(RTC_IT_SEC);
	    /* Wait until last write operation on RTC registers has finished */
		RTC_WaitForLastTask();
	}
}
void EXTI9_5_IRQHandler()
{
	if(EXTI_GetITStatus(EXTI_Line5)!=RESET)
	{
		screen = 0;
		EXTI_ClearITPendingBit(EXTI_Line5);
	}
}
void EXTI4_IRQHandler()
{
	if(EXTI_GetITStatus(EXTI_Line4)!=RESET)
	{
		screen = 1;
		 EXTI_ClearITPendingBit(EXTI_Line4);
	}
}

void EXTI0_IRQHandler()
{
	if(EXTI_GetITStatus(EXTI_Line0)!=RESET)
	{
		screen = 2;
		EXTI_ClearITPendingBit(EXTI_Line0);
	}
}

