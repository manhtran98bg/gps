/*
 * usart.h
 *
 *  Created on: Apr 10, 2021
 *      Author: manht
 */

#ifndef USART_USART_H_
#define USART_USART_H_
#include "main.h"

#define DEBUG_BUFFER_SIZE	128

typedef struct {
	USART_InitTypeDef	Init;
	USART_TypeDef	*UARTx;
	GPIO_InitTypeDef	UART_Tx;
	GPIO_TypeDef	*TX_GPIOx;
	GPIO_InitTypeDef	UART_Rx;
	GPIO_TypeDef	*RX_GPIOx;
	NVIC_InitTypeDef	NVIC_Init;
}UART_Init_Struct;
/*----------------------------------------- Define UART DEBUG ----------------------------------*/
void usart_init(UART_Init_Struct *huart, USART_TypeDef *UARTx,
				GPIO_TypeDef *TX_GPIOx, uint32_t UART_TX_Pin,
				GPIO_TypeDef *RX_GPIOx, uint32_t UART_RX_Pin,
				uint8_t USART_Priority,	uint32_t Baudrate);
void usart_send_char(UART_Init_Struct *huart, char chr);
void usart_send_str(UART_Init_Struct *huart, char *str);
void usart_send_buffer(UART_Init_Struct *huart, uint8_t *buffer, uint16_t data_len);
#endif /* USART_USART_H_ */
