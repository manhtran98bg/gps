/*
 * bitmap.c
 *
 *  Created on: May 10, 2021
 *      Author: manht
 */
#include "bitmap.h"

#ifdef SH1106_INCLUDE_BITMAP
static const uint8_t screen [] = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xc0, 0x00, 0x07, 0xff, 0xf8, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xe0, 0x00, 0x00, 0x00, 0x3f, 0xfe, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xc0, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xe0, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xf0, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xf0, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xf8, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x01, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xf8, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x07, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xfc, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfc, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfc, 0x00,
		0x00, 0x00, 0x00, 0x01, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0x00,
		0x00, 0x00, 0x00, 0x07, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0x00,
		0x00, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0x00,
		0x00, 0x00, 0x3c, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0x00,
		0x00, 0x01, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x00,
		0x00, 0x07, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x00,
		0x00, 0x0f, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x00,
		0x00, 0x0f, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x00,
		0x00, 0x0f, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x00,
		0x00, 0x0f, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe7, 0xf0,
		0x00, 0x0f, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xe7, 0xf0,
		0x00, 0x07, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xc7, 0xf0,
		0x00, 0x07, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x87, 0xf0,
		0x00, 0x0e, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x07, 0xf0,
		0x00, 0x1c, 0x00, 0x00, 0x00, 0x3f, 0x00, 0x0f, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x07, 0x07, 0xf0,
		0x00, 0x38, 0xfe, 0x03, 0xf9, 0xff, 0xe0, 0x7f, 0xfc, 0x00, 0x00, 0x7f, 0xfc, 0x0e, 0x7f, 0xff,
		0x00, 0x78, 0xfe, 0x03, 0xff, 0xff, 0xf9, 0xff, 0xfe, 0x00, 0x01, 0xff, 0xff, 0x9c, 0x7f, 0xff,
		0x00, 0xf0, 0xfe, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x07, 0xff, 0xff, 0xf8, 0x7f, 0xff,
		0x01, 0xe0, 0xfe, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x1f, 0xff, 0xff, 0xf0, 0x7f, 0xff,
		0x03, 0xc0, 0xfe, 0x03, 0xff, 0x83, 0xff, 0xf0, 0x7f, 0xc0, 0x3f, 0xf0, 0x1f, 0xf8, 0x07, 0xf0,
		0x07, 0xc0, 0xfe, 0x03, 0xfe, 0x00, 0xff, 0xc0, 0x3f, 0xc0, 0x7f, 0xc0, 0x03, 0xfc, 0x07, 0xf0,
		0x0f, 0x80, 0xfe, 0x03, 0xfc, 0x00, 0x7f, 0x80, 0x1f, 0xc0, 0x7f, 0x00, 0x01, 0xfe, 0x07, 0xf0,
		0x0f, 0x00, 0xfe, 0x03, 0xfc, 0x00, 0x7f, 0x00, 0x0f, 0xe0, 0xfe, 0x00, 0x00, 0xfe, 0x07, 0xf0,
		0x1f, 0x00, 0xfe, 0x03, 0xfc, 0x00, 0x7f, 0x00, 0x0f, 0xe0, 0xff, 0xff, 0xff, 0xfe, 0x07, 0xf0,
		0x3f, 0x00, 0xfe, 0x03, 0xf8, 0x00, 0x7f, 0x00, 0x0f, 0xe0, 0xff, 0xff, 0xff, 0xff, 0x07, 0xf0,
		0x3e, 0x00, 0xfe, 0x03, 0xf8, 0x00, 0x7f, 0x00, 0x0f, 0xe0, 0xff, 0xff, 0xff, 0xff, 0x07, 0xf0,
		0x7e, 0x00, 0xfe, 0x03, 0xf8, 0x00, 0x7f, 0x00, 0x0f, 0xe0, 0xff, 0xff, 0xff, 0xfe, 0x07, 0xf0,
		0x7e, 0x00, 0xfe, 0x03, 0xf8, 0x00, 0x7f, 0x00, 0x0f, 0xe0, 0xfe, 0x00, 0x00, 0x00, 0x07, 0xf0,
		0x7c, 0x00, 0xfe, 0x03, 0xf8, 0x00, 0x7f, 0x00, 0x0f, 0xe0, 0x7f, 0x00, 0x00, 0x00, 0x07, 0xf0,
		0xfc, 0x00, 0xfe, 0x03, 0xf8, 0x00, 0x7f, 0x00, 0x0f, 0xe0, 0x7f, 0xc0, 0x00, 0x00, 0x07, 0xf0,
		0xfc, 0x00, 0xfe, 0x03, 0xf8, 0x00, 0x7f, 0x00, 0x0f, 0xe0, 0x3f, 0xf0, 0x1f, 0xf8, 0x07, 0xf0,
		0xfc, 0x00, 0xfe, 0x03, 0xf8, 0x00, 0x7f, 0x00, 0x0f, 0xe0, 0x1f, 0xff, 0xff, 0xf0, 0x07, 0xf0,
		0xfe, 0x00, 0xfe, 0x03, 0xf8, 0x00, 0x7f, 0x00, 0x0f, 0xe0, 0x07, 0xff, 0xff, 0xc0, 0x07, 0xf0,
		0xfe, 0x00, 0xfe, 0x03, 0xf8, 0x00, 0x7f, 0x00, 0x0f, 0xe0, 0x1d, 0xff, 0xff, 0x00, 0x07, 0xf0,
		0xfe, 0x00, 0xfe, 0x03, 0xf8, 0x00, 0x7f, 0x00, 0x0f, 0xe0, 0x78, 0x3f, 0xf8, 0x00, 0x07, 0xf0,
		0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00,
		0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x7f, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x7f, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x3f, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x3f, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x1f, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x0f, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x03, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x01, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x01, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x3f, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x7f, 0xff, 0xc0, 0x00, 0x1f, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x03, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x1f, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
static const uint8_t gsm_signal_excellent [] = {
		0x00, 0x00, 0x00, 0x7f, 0x00, 0x30, 0x22, 0x00, 0x30, 0x14, 0x01, 0xb0,	0x08, 0x01, 0xb0,
		0x08, 0x0d, 0xb0, 0x08, 0x0d, 0xb0,	0x08, 0x6d, 0xb0, 0x08, 0x6d, 0xb0,	0x0b, 0x6d, 0xb0,
};
static const uint8_t gsm_signal_good [] = {
		0x00, 0x00, 0x00, 0x7f, 0x00, 0x00, 0x22, 0x00, 0x00, 0x14, 0x01, 0x80, 0x08, 0x01, 0x80,
		0x08, 0x0d, 0x80, 0x08, 0x0d, 0x80, 0x08, 0x6d, 0x80, 0x08, 0x6d, 0x80, 0x0b, 0x6d, 0x80,
};
static const uint8_t gsm_signal_ok [] = {
		0x00, 0x00, 0x00, 0x7f, 0x00, 0x00, 0x22, 0x00, 0x00, 0x14, 0x00, 0x00, 0x08, 0x00, 0x00,
		0x08, 0x0c, 0x00, 0x08, 0x0c, 0x00, 0x08, 0x6c, 0x00, 0x08, 0x6c, 0x00, 0x0b, 0x6c, 0x00,
};
static const uint8_t gsm_signal_marginal [] = {
		0x00, 0x00, 0x00, 0x7f, 0x00, 0x00, 0x22, 0x00, 0x00, 0x14, 0x00, 0x00, 0x08, 0x00, 0x00,
		0x08, 0x00, 0x00, 0x08, 0x00, 0x00, 0x08, 0x00, 0x00, 0x08, 0x00, 0x00, 0x0b, 0x00, 0x00,
};
static const uint8_t gsm_no_signal [] = {
		0x00, 0x00, 0x00, 0x7f, 0x00, 0x00, 0x22, 0x00, 0x00, 0x14, 0x00, 0x00, 0x09, 0x10, 0x00,
		0x08, 0xa0, 0x00, 0x08, 0x40, 0x00, 0x08, 0xa0, 0x00, 0x09, 0x10, 0x00, 0x08, 0x00, 0x00,
};
static const uint8_t gsm_signal_gprs [] = {
		0x78, 0x78, 0x84, 0x84, 0x05, 0x04, 0x05, 0x00, 0x09, 0x00,
		0x71, 0x1c, 0x81, 0x04, 0x81, 0x04, 0x80, 0x84, 0xfc, 0x78,
};
static const uint8_t gsm_no_signal_gprs [] = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
static const uint8_t gps_signal [] = {
		0x02, 0x00, 0x07, 0x00, 0x08, 0x80, 0x10, 0x40,	0x32, 0x60,
		0x10, 0x40, 0x08, 0x80, 0x07, 0x00, 0x02, 0x00,0x00, 0x00,
};
static const uint8_t gps_no_signal [] = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	0x00, 0x00,
};
static const uint8_t gps_logo [] = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x80, 0x00,
		0x00, 0x00, 0x06, 0x00, 0x00, 0x7f, 0xe0, 0x00, 0x00, 0x00, 0x1f, 0x00, 0x01, 0xff, 0xf8, 0x00,
		0x00, 0x00, 0x7f, 0x00, 0x07, 0xff, 0xfe, 0x00, 0x00, 0x01, 0xff, 0x00, 0x1f, 0xff, 0xfe, 0x00,
		0x00, 0x07, 0xff, 0xf8, 0x1f, 0xff, 0xf8, 0x00, 0x00, 0x1f, 0xff, 0xfe, 0x07, 0xff, 0xe0, 0x00,
		0x00, 0x7f, 0xff, 0xff, 0x8f, 0xff, 0x80, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0x7e, 0x00, 0x00,
		0x00, 0x00, 0xff, 0xff, 0xfc, 0x18, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x7f, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xfc, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x07, 0xff, 0xf8, 0x7f, 0xf8, 0x00, 0x00, 0x01, 0x83, 0xff, 0xe3, 0xff, 0xe0, 0x00,
		0x00, 0x07, 0xef, 0xff, 0x8f, 0xff, 0x80, 0x00, 0x00, 0x1f, 0xff, 0x0c, 0x3f, 0xfe, 0x04, 0xc0,
		0x00, 0x7f, 0xfe, 0x00, 0x7f, 0xf8, 0x4e, 0x60, 0x01, 0xff, 0xff, 0x80, 0xff, 0xe0, 0x66, 0x60,
		0x07, 0xff, 0xff, 0x81, 0xff, 0x80, 0x66, 0x60, 0x07, 0xff, 0xfe, 0x01, 0xfe, 0x00, 0xce, 0x60,
		0x01, 0xff, 0xf8, 0x01, 0xf8, 0x7f, 0x9c, 0xc0, 0x00, 0x7f, 0xe0, 0x01, 0xe0, 0x00, 0x31, 0x80,
		0x00, 0x1f, 0x80, 0x00, 0x81, 0xff, 0xc7, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
static const uint8_t server_connect [] = {
		0x10, 0x80, 0x38, 0x80, 0x54, 0x80, 0x10, 0x80, 0x10, 0x80,
		0x10, 0x80, 0x10, 0x80, 0x12, 0xa0, 0x11, 0xc0, 0x10, 0x80,
};
static const uint8_t server_no_connect [] = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	0x00, 0x00,
};
static const uint8_t sdcard_ok [] = {
		0x1f, 0xf8, 0x20, 0x08, 0x4a, 0xa8, 0x4a, 0xa8, 0x40, 0x08, 0x40, 0x08, 0x4f, 0xe8, 0x50, 0x28, 0x50, 0x28, 0x50, 0x28, 0x50, 0x28, 0x5f, 0xe8, 0x40, 0x08, 0x7f, 0xf8, 0x00, 0x00, 0x00, 0x00
};
static const uint8_t sdcard_fail [] ={
		0x1f, 0xf8, 0x20, 0x08, 0x4a, 0xa8, 0x4a, 0xa8, 0x40, 0x08, 0x40, 0x08, 0x58, 0xc8, 0x4d, 0x88, 0x47, 0x08, 0x47, 0x08, 0x4d, 0x88, 0x58, 0xc8, 0x40, 0x08, 0x7f, 0xf8, 0x00, 0x00, 0x00, 0x00
};
#endif
#ifdef SH1106_INCLUDE_BITMAP
BitmapDef Screen = {128,64,screen};
BitmapDef Gsm_signal[7] = {
		{24,10,gsm_signal_excellent},
		{24,10,gsm_signal_good},
		{24,10,gsm_signal_ok},
		{24,10,gsm_signal_marginal},
		{24,10,gsm_no_signal},
		{16,10,gsm_signal_gprs},
		{16,10,gsm_no_signal_gprs},
};
BitmapDef Gps_signal[2] = {
		{16,10,gps_signal},
		{16,10,gps_no_signal},
};
BitmapDef Server_connect[2] = {
		{16,10,server_connect},
		{16,10,server_no_connect},
};
BitmapDef Gps_logo = {64,32,gps_logo};
BitmapDef sdcard_status[2] = {
		{16,16,sdcard_ok},
		{16,16,sdcard_fail},
};
#endif