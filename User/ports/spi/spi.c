/*
 * spi.c
 *
 *  Created on: May 17, 2021
 *      Author: manht
 */

#include "spi.h"

void spi_init(SPI_Init_Struct *hspi, SPI_TypeDef *SPIx,
			GPIO_TypeDef *SPI_GPIOx, uint32_t MOSI_Pin,
			uint32_t MISO_Pin, uint32_t SCK_Pin,
			uint32_t CS_Pin)
{
	if (SPIx == SPI2) RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	if (SPIx == SPI3) RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);
	if (SPIx == SPI1) RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

	hspi->SPIx = SPIx;
	hspi->SPI_GPIOx = SPI_GPIOx;
	hspi->SPI_SCK.GPIO_Pin = SCK_Pin;
	hspi->SPI_SCK.GPIO_Mode = GPIO_Mode_AF_PP;
	hspi->SPI_SCK.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(hspi->SPI_GPIOx, &hspi->SPI_SCK);

	hspi->SPI_MOSI.GPIO_Pin = MOSI_Pin;
	hspi->SPI_MOSI.GPIO_Mode = GPIO_Mode_AF_PP;
	hspi->SPI_MOSI.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(hspi->SPI_GPIOx, &hspi->SPI_MOSI);

	hspi->SPI_MISO.GPIO_Pin = MISO_Pin;
	hspi->SPI_MISO.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	hspi->SPI_MISO.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(hspi->SPI_GPIOx, &hspi->SPI_MISO);

	hspi->SPI_CS.GPIO_Pin = CS_Pin;
	hspi->SPI_CS.GPIO_Mode = GPIO_Mode_Out_PP;
	hspi->SPI_CS.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(hspi->SPI_GPIOx, &hspi->SPI_CS);


	hspi->Init.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
	hspi->Init.SPI_Direction= SPI_Direction_2Lines_FullDuplex;
	hspi->Init.SPI_Mode = SPI_Mode_Master;
	hspi->Init.SPI_DataSize = SPI_DataSize_8b;
	hspi->Init.SPI_CPOL = SPI_CPOL_Low;
	hspi->Init.SPI_CPHA = SPI_CPHA_1Edge;
	hspi->Init.SPI_NSS = SPI_NSS_Soft;
	hspi->Init.SPI_FirstBit = SPI_FirstBit_MSB;
	hspi->Init.SPI_CRCPolynomial = 7;
	SPI_Init(hspi->SPIx, &hspi->Init);
	SPI_Cmd(hspi->SPIx, ENABLE);
}
uint8_t spi_send_byte(SPI_Init_Struct *hspi, uint8_t data)
{
	while(SPI_I2S_GetFlagStatus(hspi->SPIx, SPI_I2S_FLAG_TXE)==RESET);
	SPI_I2S_SendData(hspi->SPIx,data);
	while(SPI_I2S_GetFlagStatus(hspi->SPIx, SPI_I2S_FLAG_RXNE)==RESET);
	return SPI_I2S_ReceiveData(hspi->SPIx);
}

