/*
 * log.h
 *
 *  Created on: Dec 15, 2021
 *      Author: manht
 */

#ifndef INC_LOG_H_
#define INC_LOG_H_



#include <stdio.h>

/* Console color */
#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"
#define LOG_EN

#define Logi(fmt, ...) do {\
    printf("\x1B[32m[I][%ld] " fmt "\r\n\x1B[0m", get_tick(), ##__VA_ARGS__);\
} while (0)

#define Logw(fmt, ...) do {\
    printf("\x1B[33m[W][%ld][%s:%d] " fmt "\r\n\x1B[0m", get_tick(), __FILE__, __LINE__, ##__VA_ARGS__);\
} while (0)

#define Loge(fmt, ...) do {\
    printf("\x1B[31m[E][%ld][%s:%d] " fmt "\r\n\x1B[0m", get_tick(), __FILE__, __LINE__, ##__VA_ARGS__);\
} while (0)
#ifdef LOG_EN
#define logi Logi
#define loge Loge
#define logw Logw
#else
#define logi
#define loge
#define logw
#endif
#endif /* INC_LOG_H_ */
