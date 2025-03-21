/*
 * m_remote.c
 *
 *  Created on: Oct 15, 2024
 *      Author: catsa
 */
#include <m_serial.h>
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <stdarg.h>
#include "main.h"
#include "cmsis_os.h"

#include "m_serial.h"
#include "app_config.h"
#include "m_remote.h"
#include "m_ether.h"

#define MAX_PACKET_SIZE	10
static struct{
	uint8_t			startRemoInit;
	remote_mode_t	run_mode;
	uint16_t 		cmd_tail;
	uint8_t 		tri_order[MAX_CHANNEL];
}m_cfg={
	.startRemoInit = 0,
	.run_mode=/*eREMOTE_NONE*/eREMOTE_ETHER,
	.cmd_tail = 0,
	.tri_order={0,}
};

Remote_Time_t r_time;
event_queue_observer_t remote_event;
osThreadId remoteTaskHandle;
CCMRAM static uint8_t recv_buf[MAX_RECV_BUF];
static int	front = 0, rear = 0;
static remote_mode_t old_mode;
void remote_push_buf(remote_mode_t eSource, uint8_t ch)
{
	int index = (front +1) % MAX_RECV_BUF;

	if(index == rear)
		return ;
	if(eSource != m_cfg.run_mode){
		if(old_mode != eSource){
			old_mode = eSource;
			LOG_DBG("Skip source[%d] run_mode[%d]", eSource, m_cfg.run_mode);
		}
		return ;
	}

	front = index;
	recv_buf[front] = ch;
}

uint8_t remote_pop_buf(uint8_t *ch)
{
	if(front == rear) return 0;
	rear = (rear + 1) % MAX_RECV_BUF;
	*ch =  recv_buf[rear];

	return 1;
}

static void On_set_mode(remote_mode_t mode)
{
	if(mode != m_cfg.run_mode){
		if(m_cfg.run_mode != eREMOTE_NONE){
			fpga_set_factory_reset();
		}
		m_cfg.run_mode = mode;
	}
	push_event1(EVT_fpga_load_done, mode);
}

void m_remote_response(uint8_t *data)
{
	uint8_t send_data[MAX_RESPONSE_LEN]={0,};

	send_data[0] = RESPONSE_HEADER;
	if(data[0] == 0x33){
		memcpy(&send_data[1], data, 5);
	}else{
		send_data[1] = data[0];
	}
	send_data[6] = 0xFE;
	send_data[7] = 0xFF;

	m_serial_SendPC(send_data);
}

const char* Remote_Command_2_str(uint8_t r_cmd)
{
    switch (r_cmd)
    {
    	case_str(eCOMMAND_CON_CMD)
		case_str(eCOMMAND_RDY_CMD)
		case_str(eCOMMAND_OUT_MODE)
		case_str(eCOMMAND_ON_TIME)
		case_str(eCOMMAND_DELAY_TIME)
		case_str(eCOMMAND_INPUT_CHANNEL)
		case_str(eCOMMAND_OUTPUT_CHANNEL)
		case_str(eCOMMAND_TRIGGER_ORDER)
		case_str(eCOMMAND_TRIGGER_ORDER_SET)
		case_str(eCOMMAND_INPUT_EDGE)
		case_str(eCOMMAND_FACTORY_RESET)
		case_str(eCOMMAND_REMOTE_IP_STATUS)
		case_str(eCOMMAND_CLOSE_REMOTE)
    	default :
			break;
    }

    return "Unknown Command";
}

static uint8_t parse_command(remote_mode_t source, uint8_t *cmd_buf)
{
	uint16_t time;
	uint32_t channel;
	uint8_t res[1] = {eRESPONSE_OK};

//	LOG_DBG("Recv CMD[%s]", Remote_Command_2_str(cmd_buf[0]));
//	LOG_HEX_DUMP(cmd_buf, 8,"Remote Data");
	switch(cmd_buf[0])
	{
		case eCOMMAND_OUT_MODE :
			fpga_select_output(cmd_buf[2], cmd_buf[3]); // In one_one mode, ex_type should be zero..
			break;
		case eCOMMAND_ON_TIME:
			time = cmd_buf[2] << 8 | cmd_buf[3];
			fpga_set_on_time(cmd_buf[1], time);
			r_time.on_time[cmd_buf[1]] = time;
			break;
		case eCOMMAND_DELAY_TIME:
			time = cmd_buf[2] << 8 | cmd_buf[3];
			fpga_set_delay_time(cmd_buf[1], time);
			r_time.delay_time[cmd_buf[1]] = time;
			break;
		case eCOMMAND_INPUT_CHANNEL:
			channel = cmd_buf[2];
			fpga_set_input_channel(0, channel);
			if(m_cfg.startRemoInit){
				res[0] = eRESPONSE_RDY_LOAD_DONE;
				m_cfg.startRemoInit = 0;
			}
			break;
		case eCOMMAND_OUTPUT_CHANNEL:
			channel = cmd_buf[2];
			fpga_set_output_channel(0, channel);
			if(channel != 0){
				//m_tftlcd_update_remote_time(&r_time);
				push_event0(EVT_set_remotetime);
				push_event0(EVT_start_trigger);
			}else{
				push_event0(EVT_stop_trigger);
			}
			break;
		case eCOMMAND_INPUT_EDGE:
			fpga_select_edge(cmd_buf[2]);
			break;
		case eCOMMAND_FACTORY_RESET:
			fpga_set_factory_reset();
			break;
		case eCOMMAND_CLOSE_REMOTE:
			push_event1(EVT_remote_mode, eREMOTE_CLOSE);
			break;
		case eCOMMAND_REMOTE_IP_STATUS:

			break;
		case eCOMMAND_CON_CMD:
			res[0] = eRESPONSE_CON_DONE;
			push_event0(EVT_stop_trigger);
			break;
		case eCOMMAND_RDY_CMD:
			res[0] = eRESPONSE_OK;
			m_cfg.startRemoInit = 1;
			break;
		default:
			break;
	}

	if(source == eREMOTE_ETHER){
		return res[0];
	}else if(source == eREMOTE_SERIAL){
		m_remote_response(res);
		return 0;
	}else if(source == eREMOTE_FRONT){
		m_front_response(res);
	}

	return 0;
}

uint8_t m_remote_front_parser(uint8_t *recv_buf, uint16_t size)
{
	uint8_t ret;

	ret = parse_command(eREMOTE_FRONT, recv_buf);

	return ret;
}

uint8_t m_remote_eth_parser(uint8_t *recv_buf, uint16_t size)
{
	uint8_t *pBuf = recv_buf;
	uint8_t cmd_buf[MAX_PACKET_SIZE]={0,}, cmd_pos = 0, ch, ret;
	uint16_t idx = 0, cmd_tail = 0;
	uint8_t response[8]={RESPONSE_HEADER,eRESPONSE_OK,0,0x00,0x00,0x00,0xFE,0xFF};

	//LOG_DBG("Receive Eth data size [%d]", size);
	//LOG_HEX_DUMP(pBuf, size, "Remote received Data");
	while(size > idx){
		ch = pBuf[idx++];
		if((cmd_pos == 0) && (ch != REMOTE_HEADER)){
			continue;
		}

		if(cmd_pos<MAX_PACKET_SIZE){
			cmd_buf[cmd_pos++] = ch;
		}

		cmd_tail = ((cmd_tail<<8)| ch);
		if(cmd_tail == REMOTE_TAIL){
			if(cmd_buf[1]){
				if(cmd_buf[2] == eCOMMAND_REMOTE_IP_STATUS){
					fpga_get_trigger_status(&response[1]);
				}else{
					ret = parse_command(eREMOTE_ETHER, &cmd_buf[2]);
					if((ret == eRESPONSE_CON_DONE) || (ret == eRESPONSE_RDY_LOAD_DONE)){
						response[1] = ret;
					}
				}
			}
			cmd_pos = 0;
			cmd_tail = 0;
			memset(cmd_buf, 0, MAX_PACKET_SIZE);
		}
	}
	if(idx == size){
		//LOG_HEX_DUMP(response, MAX_RESPONSE_LEN, "Eth Response Data");
		memcpy(recv_buf, response, MAX_RESPONSE_LEN);
		return MAX_RESPONSE_LEN;
	}
	return 0;
}

static void RemoteProcTask(void const * argument)
{
	uint8_t ch;
	uint8_t cmd_buf[MAX_PACKET_SIZE]={0,}, cmd_pos = 0;

	LOG_DBG("Remote proc Task Start!!");
	while(1){
		if(m_cfg.run_mode != eREMOTE_SERIAL){
			osDelay(100);
			continue;
		}

		if(remote_pop_buf(&ch)){
			if((cmd_pos == 0) && (ch != REMOTE_HEADER) ){
				osDelay(10);
				continue;
			}

			if(cmd_pos<MAX_PACKET_SIZE){
				cmd_buf[cmd_pos++] = ch;
			}

			m_cfg.cmd_tail = ((m_cfg.cmd_tail<<8)| ch);
			if(m_cfg.cmd_tail == REMOTE_TAIL){
				if(cmd_buf[1]){ // Write
					//LOG_HEX_DUMP(cmd_buf, 10, "Remote received Data");
					parse_command(eREMOTE_SERIAL, &cmd_buf[2]);
					cmd_pos = 0;
					m_cfg.cmd_tail = 0;
					memset(cmd_buf, 0, MAX_PACKET_SIZE);
					osDelay(1);
				}
			}
		}else{
			osDelay(10);
		}
	}
}

static void evt_handler(event_t const* evt, void* p_context)
{
	switch(evt->event){
		case EVT_remote_mode:
			On_set_mode(evt->param1);
			break;
		default:
			break;
	}
}

uint8_t remote_proc_init(void)
{
	osThreadDef(remoteProc, RemoteProcTask, osPriorityNormal, 0, 512);
	remoteTaskHandle = osThreadCreate(osThread(remoteProc), NULL);
	if(remoteTaskHandle == NULL){
		return 1;
	}

	remote_event.handler = evt_handler;
	remote_event.p_context = NULL;
	if(m_event_register(&remote_event)){
		LOG_DBG("Register Remote Processor event Error");
		return 1;
	}

	return 0;
}
