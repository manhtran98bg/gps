/*
 * i2c.h
 *
 *  Created on: May 9, 2021
 *      Author: manht
 */

#ifndef I2C_I2C_H_
#define I2C_I2C_H_
#include "main.h"
#define I2C1_RCC		RCC_APB1Periph_I2C1
#define I2C_GPIO_RCC	RCC_APB2Periph_GPIOB
#define I2C_GPIO		GPIOB
#define I2C_PIN_SDA		GPIO_Pin_7
#define I2C_PIN_SCL		GPIO_Pin_6

typedef struct {
	I2C_InitTypeDef	Init;
	I2C_TypeDef	*I2Cx;
	GPIO_InitTypeDef	I2C_SCL;
	GPIO_TypeDef	*SCL_GPIOx;
	GPIO_InitTypeDef	I2C_SDA;
	GPIO_TypeDef	*SDA_GPIOx;
	NVIC_InitTypeDef	NVIC_Init;
}I2C_Init_Struct;

void i2c_init(I2C_Init_Struct *hi2c, I2C_TypeDef *I2Cx,
			GPIO_TypeDef *SCL_GPIOx, uint32_t I2C_SCL_Pin,
			GPIO_TypeDef *SDA_GPIOx, uint32_t I2C_SDA_Pin,
			uint8_t I2C_Priority, uint32_t Baudrate);
void i2c_start(I2C_Init_Struct *hi2c);
void i2c_stop(I2C_Init_Struct *hi2c);
void i2c_address_direction(I2C_Init_Struct *hi2c, uint8_t address, uint8_t direction);
void i2c_transmit(I2C_Init_Struct *hi2c, uint8_t byte);
uint8_t i2c_receive_ack(I2C_Init_Struct *hi2c);
uint8_t i2c_receive_nack(I2C_Init_Struct *hi2c);
void i2c_write(I2C_Init_Struct *hi2c, uint8_t address, uint8_t data);
void i2c_send_array(I2C_Init_Struct *hi2c, uint8_t address, uint8_t *buffer, size_t buffer_size);
void i2c_read(I2C_Init_Struct *hi2c, uint8_t address, uint8_t* data);
#endif /* I2C_I2C_H_ */
