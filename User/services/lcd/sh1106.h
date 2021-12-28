/*
 * sh1106.h
 *
 *  Created on: May 9, 2021
 *      Author: manht
 */
#ifndef _SH1106_H_
#define _SH1106_H_

#include "sh1106_conf.h"
#include "sh1106_font.h"
#include "bitmap.h"

typedef enum {
    Black = 0x00, // Black color, no pixel
    White = 0x01  // Pixel is set. Color depends on OLED
} SH1106_COLOR;

typedef enum {
	SH1106_OK = 0x00,
	SH1106_ERR = 0x01  // Generic error.
} SH1106_Error_t;
typedef enum {
	NO_ALIGN = 0,
	ALIGN_LEFT,
	ALIGN_RIGHT,
	ALIGN_CENTER
}Align_t;
// Struct to store transformations
typedef struct {
    uint8_t CurrentX;
    uint8_t CurrentY;
    uint8_t Inverted;
    uint8_t Initialized;
    uint8_t DisplayOn;
} SH1106_t;

typedef struct {
    uint8_t x;
    uint8_t y;
} SH1106_VERTEX;
typedef struct{
	uint8_t pwr_state ;
	uint8_t inited;
	int8_t retry;
	I2C_Init_Struct *hi2c;
	void * I2Cx;
	uint32_t baudrate;
	uint8_t address;
    uint8_t CurrentX;
    uint8_t CurrentY;
    uint8_t Inverted;
    uint8_t Initialized;
    uint8_t DisplayOn;
    uint8_t SH1106_Buffer[SH1106_BUFFER_SIZE];
}oled_t;
// Procedure definitions
void sh1106_Init(oled_t *oled);
void sh1106_Fill(oled_t *oled, SH1106_COLOR color);
void sh1106_UpdateScreen(oled_t *oled);
void sh1106_DrawPixel(oled_t *oled, uint8_t x, uint8_t y, SH1106_COLOR color);
char sh1106_WriteChar(oled_t *oled, uint8_t x, uint8_t y, char ch, FontDef Font, SH1106_COLOR color);
char sh1106_WriteString(oled_t *oled, uint8_t x, uint8_t y, char* str, FontDef Font, SH1106_COLOR color, Align_t Align);
void sh1106_SetCursor(oled_t *oled, uint8_t x, uint8_t y);
void sh1106_DrawLine(oled_t *oled, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, SH1106_COLOR color);
void sh1106_DrawArc(uint8_t x, uint8_t y, uint8_t radius, uint16_t start_angle, uint16_t sweep, SH1106_COLOR color);
void sh1106_DrawCircle(uint8_t par_x, uint8_t par_y, uint8_t par_r, SH1106_COLOR color);
void sh1106_Polyline(const SH1106_VERTEX *par_vertex, uint16_t par_size, SH1106_COLOR color);
void sh1106_DrawRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, SH1106_COLOR color);
void sh1106_DrawBitmap(oled_t *oled, uint8_t x, uint8_t y, BitmapDef bitmap);
void sh1106_Clear(oled_t *oled, SH1106_COLOR color);
/**
 * @brief Sets the contrast of the display.
 * @param[in] value contrast to set.
 * @note Contrast increases as the value increases.
 * @note RESET = 7Fh.
 */
void sh1106_SetContrast(oled_t *oled, const uint8_t value);
/**
 * @brief Set Display ON/OFF.
 * @param[in] on 0 for OFF, any for ON.
 */
void sh1106_SetDisplayOn(oled_t *oled, const uint8_t on);
/**
 * @brief Reads DisplayOn state.
 * @return  0: OFF.
 *          1: ON.
 */
uint8_t sh1106_GetDisplayOn();

// Low-level procedures
void sh1106_Reset(void);
void sh1106_WriteCommand(oled_t *oled, uint8_t byte);
void sh1106_WriteData(oled_t *oled, uint8_t* buffer, size_t buff_size);
SH1106_Error_t sh1106_FillBuffer(oled_t *oled, uint8_t* buf, uint32_t len);

#endif // __SSD1306_H__
