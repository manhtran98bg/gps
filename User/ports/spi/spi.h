/*
 * spi.h
 *
 *  Created on: May 17, 2021
 *      Author: manht
 */

#ifndef SDCARD_SPI_H_
#define SDCARD_SPI_H_

#include "main.h"
#include "delay/delay.h"
typedef struct {
	SPI_InitTypeDef	Init;
	SPI_TypeDef	*SPIx;
	GPIO_TypeDef	*SPI_GPIOx;
	GPIO_InitTypeDef	SPI_MOSI;
	GPIO_InitTypeDef	SPI_MISO;
	GPIO_InitTypeDef	SPI_SCK;
	GPIO_InitTypeDef	SPI_CS;
	NVIC_InitTypeDef	NVIC_Init;
}SPI_Init_Struct;

void spi_init(SPI_Init_Struct *hspi, SPI_TypeDef *SPIx,
			GPIO_TypeDef *SPI_GPIOx, uint32_t MOSI_Pin,
			uint32_t MISO_Pin, uint32_t SCK_Pin,
			uint32_t CS_Pin);
uint8_t spi_send_byte(SPI_Init_Struct *hspi, uint8_t data);
#endif /* SDCARD_SPI_H_ */
