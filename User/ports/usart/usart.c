/*
 * usart.c
 *
 *  Created on: Apr 10, 2021
 *      Author: manht
 */
#include "usart.h"

void usart_init(UART_Init_Struct *huart, USART_TypeDef *UARTx,
				GPIO_TypeDef *TX_GPIOx, uint32_t UART_TX_Pin,
				GPIO_TypeDef *RX_GPIOx, uint32_t UART_RX_Pin,
				uint8_t USART_Priority,	uint32_t Baudrate)
{
	if (UARTx == USART1 ) RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	if (UARTx == USART2 ) RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	if (UARTx == USART3 ) RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
	if (UARTx == UART4 ) RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);
	if (UARTx == UART5 ) RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5,ENABLE);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);

	huart->UART_Tx.GPIO_Pin = UART_TX_Pin;
	huart->UART_Tx.GPIO_Mode = GPIO_Mode_AF_PP;
	huart->UART_Tx.GPIO_Speed = GPIO_Speed_50MHz;
	huart->TX_GPIOx = TX_GPIOx;
	GPIO_Init(TX_GPIOx, &huart->UART_Tx);

	huart->UART_Rx.GPIO_Pin = UART_RX_Pin;
	huart->UART_Rx.GPIO_Mode = GPIO_Mode_IPU;
	huart->UART_Rx.GPIO_Speed = GPIO_Speed_50MHz;
	huart->RX_GPIOx = RX_GPIOx;
	GPIO_Init(RX_GPIOx, &huart->UART_Rx);

	huart->Init.USART_BaudRate = Baudrate;
	huart->Init.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;
	huart->Init.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	huart->Init.USART_Parity = USART_Parity_No;
	huart->Init.USART_StopBits = USART_StopBits_1;
	huart->Init.USART_WordLength = USART_WordLength_8b;
	huart->UARTx = UARTx;
	USART_Init(UARTx, &huart->Init);
	USART_Cmd(UARTx, ENABLE);
	USART_ITConfig(UARTx, USART_IT_RXNE, ENABLE);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	if (UARTx == USART1 ) huart->NVIC_Init.NVIC_IRQChannel = USART1_IRQn;
	if (UARTx == USART2 ) huart->NVIC_Init.NVIC_IRQChannel = USART2_IRQn;
	if (UARTx == USART3 ) huart->NVIC_Init.NVIC_IRQChannel = USART3_IRQn;
	if (UARTx == UART4 ) huart->NVIC_Init.NVIC_IRQChannel = UART4_IRQn;
	if (UARTx == UART5 ) huart->NVIC_Init.NVIC_IRQChannel = UART5_IRQn;
	huart->NVIC_Init.NVIC_IRQChannelCmd = ENABLE;
	huart->NVIC_Init.NVIC_IRQChannelPreemptionPriority = USART_Priority;
	NVIC_Init(&huart->NVIC_Init);
}
void usart_send_char(UART_Init_Struct *huart, char chr)
{
	while(USART_GetFlagStatus(huart->UARTx, USART_FLAG_TXE) == RESET);
	USART_SendData(huart->UARTx, chr);
}
void usart_send_str(UART_Init_Struct *huart, char *str)
{
	while(*str)
	{
		while(USART_GetFlagStatus(huart->UARTx,USART_FLAG_TXE) == RESET);
		USART_SendData(huart->UARTx,*str);
		str++;
	}
}
void usart_send_buffer(UART_Init_Struct *huart, uint8_t *buffer, uint16_t data_len)
{
	for(int i=0;i<data_len;i++)
	{
		while(USART_GetFlagStatus(huart->UARTx,USART_FLAG_TXE) == RESET);
		USART_SendData(huart->UARTx,*(buffer+i));
	}
}

