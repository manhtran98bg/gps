/*
 * i2c.c
 *
 *  Created on: May 9, 2021
 *      Author: manht
 */

#ifndef I2C_I2C_C_
#define I2C_I2C_C_

#include "i2c.h"
void i2c_init(I2C_Init_Struct *hi2c, I2C_TypeDef *I2Cx,
			GPIO_TypeDef *SCL_GPIOx, uint32_t I2C_SCL_Pin,
			GPIO_TypeDef *SDA_GPIOx, uint32_t I2C_SDA_Pin,
			uint8_t I2C_Priority, uint32_t Baudrate)
{
	// Initialize I2C
	if (I2Cx == I2C1) RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	if (I2Cx == I2C2) RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
	hi2c->I2Cx = I2Cx;
	hi2c->I2C_SCL.GPIO_Pin = I2C_PIN_SCL;
	hi2c->I2C_SCL.GPIO_Mode = GPIO_Mode_AF_OD;
	hi2c->I2C_SCL.GPIO_Speed = GPIO_Speed_50MHz;
	hi2c->SCL_GPIOx = SCL_GPIOx;
	GPIO_Init(hi2c->SCL_GPIOx, &hi2c->I2C_SCL);

	hi2c->I2C_SDA.GPIO_Pin = I2C_PIN_SDA;
	hi2c->I2C_SDA.GPIO_Mode = GPIO_Mode_AF_OD;
	hi2c->I2C_SDA.GPIO_Speed = GPIO_Speed_50MHz;
	hi2c->SDA_GPIOx = SDA_GPIOx;
	GPIO_Init(hi2c->SDA_GPIOx, &hi2c->I2C_SDA);

	hi2c->Init.I2C_Mode = I2C_Mode_I2C;
	hi2c->Init.I2C_ClockSpeed = Baudrate;
	hi2c->Init.I2C_DutyCycle = I2C_DutyCycle_2;
	hi2c->Init.I2C_Ack = I2C_Ack_Disable;
	hi2c->Init.I2C_OwnAddress1 = 0x00;
	hi2c->Init.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_Init(I2Cx, &hi2c->Init);
	I2C_Cmd(I2Cx, ENABLE);

}
void i2c_start(I2C_Init_Struct *hi2c)
{
	// Wait until I2Cx is not busy anymore
	while(I2C_GetFlagStatus(hi2c->I2Cx, I2C_FLAG_BUSY));
    // Generate start condition
    I2C_GenerateSTART(hi2c->I2Cx, ENABLE);
    // Wait for I2C EV5.
    // It means that the start condition has been correctly released
    // on the I2C bus (the bus is free, no other devices is communicating))
    while (!I2C_CheckEvent(hi2c->I2Cx,I2C_EVENT_MASTER_MODE_SELECT));
}
void i2c_stop(I2C_Init_Struct *hi2c)
{
    // Generate I2C stop condition
    I2C_GenerateSTOP(hi2c->I2Cx, ENABLE);
    // Wait until I2C stop condition is finished
    while (I2C_GetFlagStatus(hi2c->I2Cx, I2C_FLAG_STOPF));
}
void i2c_address_direction(I2C_Init_Struct *hi2c, uint8_t address, uint8_t direction)
{
    // Send slave address
    I2C_Send7bitAddress(hi2c->I2Cx, address, direction);

    // Wait for I2C EV6
    // It means that a slave acknowledges his address
    if (direction == I2C_Direction_Transmitter)
    {
        while (!I2C_CheckEvent(hi2c->I2Cx,
        		I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
    }
    else if (direction == I2C_Direction_Receiver)
    {
        while (!I2C_CheckEvent(hi2c->I2Cx,
        		I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
    }
}
void i2c_transmit(I2C_Init_Struct *hi2c, uint8_t byte)
{
    // Send data byte
    I2C_SendData(hi2c->I2Cx, byte);
    // Wait for I2C EV8_2.
    // It means that the data has been physically shifted out and
    // output on the bus)
    while (!I2C_CheckEvent(hi2c->I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
}
uint8_t i2c_receive_ack(I2C_Init_Struct *hi2c)
{
    // Enable ACK of received data
    I2C_AcknowledgeConfig(hi2c->I2Cx, ENABLE);
    // Wait for I2C EV7
    // It means that the data has been received in I2C data register
    while (!I2C_CheckEvent(hi2c->I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED));

    // Read and return data byte from I2C data register
    return I2C_ReceiveData(hi2c->I2Cx);
}
uint8_t i2c_receive_nack(I2C_Init_Struct *hi2c)
{
    // Disable ACK of received data
    I2C_AcknowledgeConfig(hi2c->I2Cx, DISABLE);
    // Wait for I2C EV7
    // It means that the data has been received in I2C data register
    while (!I2C_CheckEvent(hi2c->I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED));

    // Read and return data byte from I2C data register
    return I2C_ReceiveData(I2C1);
}
void i2c_write(I2C_Init_Struct *hi2c, uint8_t address, uint8_t data)
{
    i2c_start(hi2c);
    i2c_address_direction(hi2c, address << 1, I2C_Direction_Transmitter);
    i2c_transmit(hi2c, data);
    i2c_stop(hi2c);
}
void i2c_send_array(I2C_Init_Struct *hi2c, uint8_t address, uint8_t *buffer, size_t buffer_size)
{
	size_t i=0;
	i2c_start(hi2c);
	i2c_address_direction(hi2c, address<<1, I2C_Direction_Transmitter);
	while(i<buffer_size)
	{
		i2c_transmit(hi2c, *(buffer+i));
		i++;
	}
	i2c_stop(hi2c);
}
void i2c_read(I2C_Init_Struct *hi2c, uint8_t address, uint8_t* data)
{
    i2c_start(hi2c);
    i2c_address_direction(hi2c, address << 1, I2C_Direction_Receiver);
    *data = i2c_receive_nack(hi2c);
    i2c_stop(hi2c);
}
#endif /* I2C_I2C_C_ */
