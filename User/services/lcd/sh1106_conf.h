/*
 * sh1106_conf.h
 *
 *  Created on: May 9, 2021
 *      Author: manht
 */

#ifndef _SH1106_CONF_H_
#define _SH1106_CONF_H_
#include "main.h"
#include "i2c/i2c.h"
// I2C Configuration
#define SH1106_I2C       I2C1
#define SH1106_I2C_ADDR       0x3C

#define SH1106_I2C_GPIO			GPIOB
#define SH1106_I2C_PIN_SDA		GPIO_Pin_7
#define SH1106_I2C_PIN_SCL		GPIO_Pin_6


// Mirror the screen if needed
// #define SH1106_MIRROR_VERT
// #define SH1106_MIRROR_HORIZ

// Set inverse color if needed
// # define SH1106_INVERSE_COLOR

// Include only needed fonts
#define SH1106_INCLUDE_FONT_6x8
#define SH1106_INCLUDE_FONT_7x10
#define SH1106_INCLUDE_FONT_11x18
#define SH1106_INCLUDE_FONT_16x26
#define SH1106_INCLUDE_BITMAP
// Some OLEDs don't display anything in first two columns.
// In this case change the following macro to 130.
// The default value is 128.
#define SH1106_WIDTH           130

// The height can be changed as well if necessary.
// It can be 32, 64 or 128. The default value is 64.
#define SH1106_HEIGHT          64

#define SH1106_BUFFER_SIZE   SH1106_WIDTH * SH1106_HEIGHT / 8
//#define SH1106_MIRROR_VERT
//#define SH1106_MIRROR_HORIZ
//#define SH1106_INVERSE_COLOR
#endif /* LCD_SH1106_CONF_H_ */
