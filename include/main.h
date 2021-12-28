/*
 * main.h
 *
 *  Created on: Apr 10, 2021
 *      Author: manht
 */

#ifndef MAIN_H_
#define MAIN_H_
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "diag/Trace.h"
#include "float.h"
#include "time.h"
#include "math.h"
#include "stm32f10x.h"
//#include "port_pin.h"

#include "stm32f10x_conf.h"
#include "FreeRTOS.h"
#include "task.h"
#include "log.h"

#define INFO_FM "Fw: %d Hw: %d  Author: ManhTH  Build: %s, %s\r\n"\
                    "===============================================================\r\n\x1B"
#define FIRMWARE_VERSION        100
#define HARDWARE_VERSION        104
#define GPS_FIX_BIT	(1<<0)
#endif /* MAIN_H_ */
