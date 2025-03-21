/*
 * m_front.c
 *
 *  Created on: Feb 12, 2025
 *      Author: catsa
 */
/* Includes ------------------------------------------------------------------*/
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <stdarg.h>
#include "main.h"
#include "cmsis_os.h"

#include "m_normal.h"
#include "m_remote.h"
#include "fpga_proc.h"
#include "m_front.h"
#include "m_serial.h"

/* Private define ------------------------------------------------------------*/
#define MAX_QUEUE_SIZE	128
#define MAX_CMD_SIZE	8

#define CMD_HEAD		0xEF
#define CMD_TAIL		0xFEFF

#define RES_CMD_HEAD	0xDF

#define CMD_FR_SET_MODE		0xC0
#define CMD_FR_SELF_TEST	0xC1
#define CMD_FR_SET_DEVID	0xC2

/* Private typedef -----------------------------------------------------------*/
typedef struct _QUEUE{
	uint16_t _head;
	uint16_t _tail;
	uint8_t _data[MAX_QUEUE_SIZE];
}QUEUE;

static struct {
	uint16_t cmd_pos;
	uint16_t cmd_state;
	uint16_t cmd_size;
	uint8_t cmd_buffer[MAX_CMD_SIZE];
}m_cfg={
	.cmd_pos =0,
	.cmd_state = 0,
};

QUEUE que;
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static osThreadId FrontTaskHandle;
/* Private function prototypes -----------------------------------------------*/
static void reset_queue(void)
{
	que._head = que._tail = 0;
}

void front_push_buf(uint8_t ch)
{
	int pos = (que._head + 1) % MAX_QUEUE_SIZE;

	if(pos == que._tail) return ;

	que._data[que._head] = ch;
	que._head = pos;
}

static uint8_t pop_buf(uint8_t *ch)
{
	if(que._head == que._tail) return 0;

	*ch =  que._data[que._tail];
	que._tail = (que._tail + 1) % MAX_QUEUE_SIZE;

	return 1;
}

static uint16_t find_cmd(void)
{
	uint8_t data = 0, len = 0;

	while(pop_buf(&data)){
		if((m_cfg.cmd_pos == 0)&&(data != CMD_HEAD)){
			continue;
		}
		if(m_cfg.cmd_pos < MAX_CMD_SIZE){
			m_cfg.cmd_buffer[m_cfg.cmd_pos++] = data;
		}
		m_cfg.cmd_state = ((m_cfg.cmd_state<<8)|data);
		if(m_cfg.cmd_state == CMD_TAIL){
			len = m_cfg.cmd_pos;
			m_cfg.cmd_state = 0;
			m_cfg.cmd_pos = 0;
		}
	}
	return len;
}

static void parse_front_cmd(void)
{
	uint8_t res[1];

	res[0] = eRESPONSE_OK;
	switch(m_cfg.cmd_buffer[2]){
		case CMD_FR_SET_MODE:
			push_event1(EVT_remote_mode, (m_cfg.cmd_buffer[4]));
			break;
		case CMD_FR_SELF_TEST:
			push_event0(EVT_self_test);
			break;
		case CMD_FR_SET_DEVID:
			push_event1(EVT_Set_DevID, m_cfg.cmd_buffer[4]);
			break;
	}

	m_front_response(res);
}

void frontTask(void const * argument)
{
	uint8_t size, ret;

	while(1){
		size = find_cmd();
		if(size){
			if(m_cfg.cmd_buffer[2]>=CMD_FR_SET_MODE){
				parse_front_cmd();
			}else{
				ret = m_remote_front_parser(&m_cfg.cmd_buffer[2], size);
			}
		}
	}

	return ret;
}

void m_front_update_trigger_status(uint8_t trigger)
{
	uint8_t send_buf[]={RES_CMD_HEAD, 0x33, 0x00, 0x00, 0x00, 0x00, 0xFE, 0xFF};

	send_buf[2] = trigger;
	m_serial_send(eFRONT_UART, send_buf, MAX_CMD_SIZE);
}

void m_front_response(uint8_t *data)
{
	uint8_t send_buf[]={RES_CMD_HEAD, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0xFF};

	send_buf[1] = *data;
	m_serial_send(eFRONT_UART, send_buf, MAX_CMD_SIZE);
}

uint8_t m_front_init(void)
{
	reset_queue();

	osThreadDef(FrontTask, frontTask, osPriorityNormal, 0, 1024);
	FrontTaskHandle = osThreadCreate(osThread(FrontTask), NULL);
	return 0;
}
