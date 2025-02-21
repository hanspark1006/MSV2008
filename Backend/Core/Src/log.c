/*
 * log.c
 *
 *  Created on: Feb 6, 2024
 *      Author: catsa
 */
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <stdarg.h>

#include "cmsis_os.h"
#include "main.h"
#include "log.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define DBG_UART	(&huart3)
#define MAX_PRINT_BUF	0xFF

#define HEXDUMP_BYTES_IN_LINE 8U
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
CCMRAM static  char debug_buf[MAX_PRINT_BUF];
static osMutexId hPrintMutexHandle;
uint8_t log_level = LOG_NON_LEVEL;
/* Private function prototypes -----------------------------------------------*/

void Printf(const char *fmt, ...)
{
	va_list args = {0};

	osMutexWait(hPrintMutexHandle, osWaitForever);
	va_start(args, fmt);
	vsnprintf(debug_buf, MAX_PRINT_BUF, fmt, args);
	va_end(args);

	HAL_UART_Transmit(DBG_UART, (uint8_t *)debug_buf, strlen(debug_buf), 0xFFFFFF);
	osMutexRelease(hPrintMutexHandle);

}

static void hex_dump_line_print(const char *data, int length)
{
	for (int i = 0U; i < HEXDUMP_BYTES_IN_LINE; i++) {
		if (i < length) {
			Printf("%02x ", (unsigned char)data[i] & 0xFFu);
		} else {
			Printf("   ");
		}
	}
	Printf("\r\n");
}

void dump_printf(const void *data, int size, const char* str)
{
	const char *data_buf = (const char *)data;

	Printf(str);
	Printf("\r\n");

	while(size > 0){
		hex_dump_line_print(data_buf, size);
		if(size < HEXDUMP_BYTES_IN_LINE){
			break;
		}
		size -= HEXDUMP_BYTES_IN_LINE;
		data_buf += HEXDUMP_BYTES_IN_LINE;
	}
}

void Debug_Init(void)
{
	log_level = LOG_INF_LEVEL;

	osMutexDef(hPrintMutex);
	hPrintMutexHandle = osMutexCreate(osMutex(hPrintMutex));
}
