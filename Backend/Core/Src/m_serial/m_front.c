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
#include "app_config.h"
/* Private define ------------------------------------------------------------*/
#define MAX_QUEUE_SIZE	512

#define MAX_CMD_SIZE	8
#define RECV_MAX_SIZE	10

#define CMD_HEAD		0xEF
#define CMD_TAIL		0xFEFF

#define RES_CMD_HEAD	0xDF
#define RES_CMD_VER		0x11

#define CMD_FR_READ_VER		0x10
enum{
	eCMD_FR_SET_MODE = 0xE0,
	eCMD_FR_SELF_TEST,
	eCMD_FR_SET_DEVID,
	eCMD_FR_SET_IP,
	eCMD_FR_SET_PORT,
};
/* Private typedef -----------------------------------------------------------*/
CCMRAM static uint8_t _data[MAX_QUEUE_SIZE];
typedef struct _QUEUE{
	uint16_t _head;
	uint16_t _tail;
}QUEUE;

static struct {
	uint16_t cmd_pos;
	uint16_t cmd_state;
	uint16_t cmd_size;
	uint8_t cmd_buffer[RECV_MAX_SIZE];
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

	_data[que._head] = ch;
	que._head = pos;
}

static uint8_t pop_buf(uint8_t *ch)
{
	if(que._head == que._tail) return 0;

	*ch =  _data[que._tail];
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
		if(m_cfg.cmd_pos < RECV_MAX_SIZE){
			m_cfg.cmd_buffer[m_cfg.cmd_pos++] = data;
		}
		m_cfg.cmd_state = ((m_cfg.cmd_state<<8)|data);
		if(m_cfg.cmd_state == CMD_TAIL){
			len = m_cfg.cmd_pos;
			m_cfg.cmd_state = 0;
			m_cfg.cmd_pos = 0;

			return len;
		}
	}
	return 0;
}

static void parse_front_cmd(void)
{
	uint8_t res[1];
	model_ip_net_t ipAddress;

	res[0] = eRESPONSE_OK;
	switch(m_cfg.cmd_buffer[2]){
		case eCMD_FR_SET_MODE:
			LOG_DBG("Front Set Remote mode!![%d]", m_cfg.cmd_buffer[4]);
			push_event1(EVT_remote_mode, (m_cfg.cmd_buffer[4]));
			break;
		case eCMD_FR_SELF_TEST:
			push_event0(EVT_self_test);
			break;
		case eCMD_FR_SET_DEVID:
			push_event1(EVT_Set_DevID, m_cfg.cmd_buffer[4]);
			break;
		case eCMD_FR_SET_IP:
			ipAddress.port = m_app_cfg->model.IpAddress.port;
			memcpy(ipAddress.ipaddr, &m_cfg.cmd_buffer[4], 4);
			for(int i = 0; i < 4; i++){
				if(ipAddress.ipaddr[i] != m_app_cfg->model.IpAddress.ipaddr[i]){
					push_event0_param(EVT_Set_ip, &ipAddress, sizeof(model_ip_net_t));
					break;
				}
			}
			break;
		case eCMD_FR_SET_PORT:
			memcpy(ipAddress.ipaddr, &m_app_cfg->model.IpAddress.ipaddr, 4);
			ipAddress.port = m_cfg.cmd_buffer[5] << 8 | m_cfg.cmd_buffer[4];
			if(ipAddress.port!= m_app_cfg->model.IpAddress.port){
				push_event0_param(EVT_Set_ip, &ipAddress, sizeof(model_ip_net_t));
			}
			break;

	}

	m_front_response(res);
}

static void send_version(void)
{
	uint8_t send_buf[]={RES_CMD_HEAD, RES_CMD_VER, 0x00, 0x00, 0x00, 0x00, 0xFE, 0xFF};

	send_buf[2] = MCU_VERSION_NUM;
	send_buf[3] = FPGA_VERSION_NUM;
	m_serial_send(eFRONT_UART, send_buf, MAX_CMD_SIZE);
	LOG_DBG("Send Version MCU[%d] FPGA[%d]", send_buf[2], send_buf[3]);
}

void frontTask(void const * argument)
{
	uint8_t size;

	LOG_DBG("Start Front Task!!");
	for(int i = 0; i < 10; i++){
		osDelay(100);
	}

	while(1){
		size = find_cmd();
		if(size){
			LOG_HEX_DUMP(m_cfg.cmd_buffer,RECV_MAX_SIZE,"Receive Front command");
			LOG_DBG("Receive Cmd[%x]", m_cfg.cmd_buffer[2]);
			if(m_cfg.cmd_buffer[2]>=eCMD_FR_SET_MODE){
				parse_front_cmd();
			}else if(m_cfg.cmd_buffer[2]== CMD_FR_READ_VER){
				send_version();
			}else{
				m_remote_front_parser(&m_cfg.cmd_buffer[2], size);
			}
		}
		osDelay(50);
	}
}

void m_front_update_trigger_status(uint8_t trigger)
{
	uint8_t send_buf[]={RES_CMD_HEAD, 0x33, 0x00, 0x00, 0x00, 0x00, 0xFE, 0xFF};

	send_buf[2] = trigger;
	//LOG_DBG("Send Trigger Status[%d]", trigger);
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
