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

#include "main.h"
#include "apps.h"
#include "screen.h"
#include "log.h"
#include "macros_common.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define DBG_UART	(&huart2)
#define MAX_PRINT_BUF	0xFF

#define HEXDUMP_BYTES_IN_LINE 8U
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
char debug_buf[MAX_PRINT_BUF];
/* Private function prototypes -----------------------------------------------*/

void Printf(const char *fmt, ...)
{
	va_list args = {0};

	va_start(args, fmt);
	vsnprintf(debug_buf, MAX_PRINT_BUF, fmt, args);
	va_end(args);

	HAL_UART_Transmit(DBG_UART, (uint8_t *)debug_buf, strlen(debug_buf), 0xFFFFFF);
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

const char *screen_id_2_str(int screen_id)
{
	switch(screen_id)
	{
		case_str(eMAIN_SC)
		case_str(eCHANNEL_SC)
		case_str(eSET_ON_TIME_SC)
		case_str(eSET_DELAY_TIME_SC)
		case_str(eSET_TRIGGER_SC)
		case_str(eOPMODE_SC)
		case_str(eUARTMODE_SC)
		case_str(eETH_IP_SC)
		case_str(eETH_PORT_SC)
		case_str(eTESTMODE_SC)
		case_str(eUSERMODE_SC)
		case_str(eFACTORY_SC)
		case_str(eCHSELECT_SC)
		case_str(eVERSION_SC)
		default:
			break;
	}
	return "Unknown Screen";
}

const char *key_id_2_str(int key_id)
{
	switch(key_id)
	{
		case_str(eKey_Idle)
		case_str(eKey_Enter)
		case_str(eKey_Up)
		case_str(eKey_User)
		case_str(eKey_Down)
		case_str(eKey_Version)
		case_str(eKey_ChSel)
		case_str(eKey_Mode)
		case_str(eKey_Factory)
		case_str(eKey_CursorUp)
		case_str(eKey_CursorDown)
		case_str(eKey_SetMode)
		case_str(eKey_ExitRemote)
		default:
			break;
	}
	return "Unknown key";
}
