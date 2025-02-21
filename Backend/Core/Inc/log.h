/*
 * log.h
 *
 *  Created on: Feb 6, 2024
 *      Author: catsa
 */

#ifndef INC_LOG_H_
#define INC_LOG_H_
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <stdarg.h>

#define LOG_NON_LEVEL	0
#define LOG_ERR_LEVEL	1
#define LOG_WRN_LEVEL	2
#define LOG_DBG_LEVEL	3
#define LOG_INF_LEVEL	4

void Printf(const char *fmt, ...);
void dump_printf(const void *data, int size, const char* str);
void Print_init(void);
extern uint8_t log_level;

#if defined(DEBUG)

#define LOG_ERR(fmt, ...) if(log_level >= LOG_ERR_LEVEL) Printf("ERR:[%s] " fmt "\r\n", __func__,##__VA_ARGS__);
#define LOG_WRN(fmt, ...) if(log_level >= LOG_WRN_LEVEL) Printf("WRN:[%s] " fmt "\r\n", __func__,##__VA_ARGS__);
#define LOG_DBG(fmt, ...) if(log_level >= LOG_DBG_LEVEL) Printf("DBG:[%s] " fmt "\r\n", __func__,##__VA_ARGS__);
#define LOG_INF(fmt, ...) if(log_level >= LOG_INF_LEVEL) Printf("INF:[%s] " fmt "\r\n", __func__, ##__VA_ARGS__);

#define LOG_HEX_DUMP(_data, _length, _str) dump_printf(_data, _length, _str)
#else
#define LOG_ERR(...) (void) 0
#define LOG_WRN(...) (void) 0
#define LOG_DBG(...) (void) 0
#define LOG_INF(...) (void) 0
#define LOG_HEX_DUMP(...) (void)0
#endif

#endif /* INC_LOG_H_ */
