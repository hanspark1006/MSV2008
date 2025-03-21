/*
 * fpga_proc.c
 *
 *  Created on: May 7, 2024
 *      Author: catsa
 */

/* Includes ------------------------------------------------------------------*/
#include "cmsis_os.h"
#include "main.h"

#include "m_serial.h"
#include "m_remote.h"
#include "app_config.h"
#include "fpga_proc.h"
#include "m_front.h"

/* Private define ------------------------------------------------------------*/
#define MAX_PACKET_SIZE		5
#define MAX_FPGA_BUF		2048
/* Private typedef -----------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
CCMRAM static uint8_t recv_buf[MAX_FPGA_BUF];
static int rear = 0, front = 0;

static osThreadId fpgaprocTaskHandle;
#ifdef USE_TRIGGER_TIMER
static osTimerId triggerTimerHandle;
#endif
static struct{
	uint8_t run_trigger;
	uint8_t run_mode;
	uint8_t trigger_buffer[MAX_PACKET_SIZE];
	uint32_t last_out_channel;
}m_cfg={
	.run_trigger=0,
	.run_mode = eREMOTE_NONE,
	.trigger_buffer = {0,},
	.last_out_channel = 0
};

event_queue_observer_t fpga_event;
/* Private function prototypes -----------------------------------------------*/
void fpga_push_buf(uint8_t ch)
{
	int index = (front + 1) % MAX_FPGA_BUF;

	if(m_cfg.run_trigger == 0) return ;

	if(index == rear) return ;

	front = index;
	recv_buf[front] = ch;
}

uint8_t pop_fpga_buf(uint8_t *ch)
{
	if(front == rear) return 0;
	rear = (rear + 1) % MAX_FPGA_BUF;
	*ch =  recv_buf[rear];

	return 1;
}

uint8_t fpga_read_register(uint8_t reg, uint16_t *pValue)
{
	fpga_data_t read_data;
	uint8_t data[10], idx = 0;

	read_data.header = READ_HEADER;
	read_data.addr = reg;

	m_serial_send(eFPGA_UART, (uint8_t*)&read_data, 2);

	osDelay(10);
	while(pop_fpga_buf(&data[idx])){
		idx++;
		if(idx > 10){
			LOG_ERR("FPGA Read overflow!!");
			break;
		}
	}
	idx = 0;
	while(1){
		if(data[idx] == 0xAA) break;
		idx++;
	}
	if(idx > 6){
		LOG_ERR("FPGA read error!!")
		return 1;
	}
	memcpy(pValue, &data[idx+2], idx);

	return 0;
}

void fpga_set_last_output_channel(void)
{
	fpga_set_output_channel(0, m_cfg.last_out_channel);
}

int fpga_set_output_channel(uint8_t outmode, uint32_t channel)
{
	fpga_data_t send_data;
	uChBit Channel;
	int error;

	Channel.channel_bit = channel;
	send_data.header = WRITE_HEADER;
	send_data.addr = OUTPUT_CH_L_REG;
	for(int i = 0; i<2; i++){
		send_data.addr-=i;
		send_data.data2 = Channel.channel_data[i*2];
		send_data.data1 = Channel.channel_data[i*2+1];
		LOG_HEX_DUMP(&send_data, 4, "set output");
		error = m_serial_send(eFPGA_UART, (uint8_t*)&send_data, 4);
		RETURN_IF_ERROR(error);
	}
	if(channel != 0){
		m_cfg.last_out_channel = channel;
	}

	return 0;
}

int fpga_set_input_channel(uint8_t outmode, uint32_t channel)
{
	fpga_data_t send_data;
	uChBit Channel;
	int error;

	Channel.channel_bit = channel;
	send_data.header = WRITE_HEADER;
	send_data.addr = INPUT_CH_L_REG;
	for(int i = 0; i<2; i++){
		send_data.addr-=i;
		send_data.data2 = Channel.channel_data[i*2];
		send_data.data1 = Channel.channel_data[i*2+1];
		LOG_HEX_DUMP(&send_data, 4, "set input");
		error = m_serial_send(eFPGA_UART, (uint8_t*)&send_data, 4);
		RETURN_IF_ERROR(error);
	}

	return 0;
}

uint8_t fpga_set_mode(uint8_t mode)
{
	fpga_data_t send_data;

	send_data.header = WRITE_HEADER;
	send_data.addr = OPERATION_MODE_REG;
	send_data.data1 = mode;
	send_data.data2 = 0;

	LOG_DBG("Set Mode : [%d]", mode);
	if(m_serial_send(eFPGA_UART, (uint8_t*)&send_data, 4) != HAL_OK){
		return 1;
	}

	return 0;
}

uint8_t fpga_select_output(uint8_t sel_output, uint8_t ex_type)
{
	fpga_data_t send_data;

	send_data.header = WRITE_HEADER;
	send_data.addr = INOUT_SELECT_REG;
	send_data.data1 = sel_output;
	send_data.data2 = 0;
	if(sel_output == eONE_N){
		send_data.data2 = ex_type;
	}

	LOG_HEX_DUMP(&send_data, 4, "set out mode");
	if(m_serial_send(eFPGA_UART, (uint8_t*)&send_data, 4) != HAL_OK){
		return 1;
	}

	return 0;
}

uint8_t fpga_select_edge(uint8_t sel_edge)
{
	fpga_data_t send_data;

	send_data.header = WRITE_HEADER;
	send_data.addr = TIGGER_INPUT_REG;
	send_data.data1 = sel_edge;
	send_data.data2 = 0;

	if(m_serial_send(eFPGA_UART, (uint8_t*)&send_data, 4) != HAL_OK){
		return 1;
	}

	return 0;
}

uint8_t fpga_pwm_reset(void)
{
	fpga_data_t send_data;
	int err;

	send_data.header = WRITE_HEADER;
	send_data.addr = DIMMIMG_RESET_REG;
	send_data.data1 = 0;
	send_data.data2 = 0;

	err = m_serial_send(eFPGA_UART, (uint8_t*)&send_data, 4);

	return err;
}

int fpga_set_delay_time(uint8_t ch, uint16_t time)
{
	fpga_data_t send_data;
	uChtime set_time;
	int err_code;

	//LOG_DBG("CH[%d] Delay time[%d]", ch, time);
	set_time.u16_time = time;

	send_data.header = WRITE_HEADER;
	send_data.addr = CH1_DELAY_TIME_REG + ch;
	send_data.data1 = set_time.array_time[1];
	send_data.data2 = set_time.array_time[0];
	err_code = m_serial_send(eFPGA_UART, (uint8_t*)&send_data, 4);
	if(err_code != HAL_OK){
		RETURN_IF_ERROR(err_code);
	}

	return 0;
}

int fpga_set_on_time(uint8_t ch, uint16_t time)
{
	fpga_data_t send_data;
	uChtime set_time;

	//LOG_DBG("CH[%d] On time[%d]", ch, time);
	set_time.u16_time = time;

	send_data.header = WRITE_HEADER;
	send_data.addr = CH1_ON_TIME_REG + ch;
	send_data.data1 = set_time.array_time[1];
	send_data.data2 = set_time.array_time[0];
	if(m_serial_send(eFPGA_UART, (uint8_t*)&send_data, 4) != HAL_OK){
		return -1;
	}

	return 0;
}

int fpga_set_duty_time(uint8_t ch, uint16_t time)
{
	fpga_data_t send_data;
	uChtime set_time;

	set_time.u16_time = time;

	send_data.header = WRITE_HEADER;
	send_data.addr = CH1_DUTY_TIME_REG + ch;
	send_data.data1 = set_time.array_time[1];
	send_data.data2 = set_time.array_time[0];
	if(m_serial_send(eFPGA_UART, (uint8_t*)&send_data, 4) != HAL_OK){
		return -1;
	}

	return 0;
}

int fpga_set_period_time(uint8_t ch, uint16_t time)
{
	fpga_data_t send_data;
	uChtime set_time;

	set_time.u16_time = time;

	send_data.header = WRITE_HEADER;
	send_data.addr = CH1_PERIOD_TIME_REG + ch;
	send_data.data1 = set_time.array_time[1];
	send_data.data2 = set_time.array_time[0];

	if(m_serial_send(eFPGA_UART, (uint8_t*)&send_data, 4) != HAL_OK){
		return -1;
	}

	return 0;
}

int fpga_set_trigger_order(uint8_t *ch_odder)
{
	fpga_data_t send_data ={0,};

	send_data.header = WRITE_HEADER;
	for(int i = 0; i < MAX_CHANNEL; i+=2){
		LOG_DBG("Ch %d Odder[%d] Ch %d Odder[%d]", i+1, ch_odder[i], i+2, ch_odder[i+1]);
		if(ch_odder[i] > 0 || ch_odder[i+1] > 0){
			send_data.data1 = ch_odder[i];
			send_data.data2 = ch_odder[i+1];
			send_data.addr = CH_1_2_TRIGGER_ORDER_REG + (i/2);
			LOG_HEX_DUMP(&send_data, sizeof(fpga_data_t), "Write Oder");
			if(m_serial_send(eFPGA_UART, (uint8_t*)&send_data, 4) != HAL_OK){
				return -1;
			}
		}
	}

	return 0;
}

static void onTriggerCtrl(uint8_t ctrl)
{
	//LOG_DBG("Trigger %s", ctrl?"Start":"Stop");
	m_cfg.run_trigger = ctrl;
}

static uint8_t parse_packet(uint8_t *pBuf)
{
	if(pBuf[0] == 0x33){
		m_front_update_trigger_status(pBuf[1]);
		return 1;
	}else if(pBuf[0] == 0xAA){
		// response data
	}

	return 0;
}

void fpga_set_factory_reset(void)
{
	int i;
	uint8_t order[MAX_CHANNEL] = {0,};

	fpga_select_output(eONE_ONE, 0);
	fpga_select_edge(eTRIGGER_RISING);
	for(i = 0; i < MAX_CHANNEL; i++){
		fpga_set_on_time(i, 1000);
		fpga_set_delay_time(i, 0);
	}

	for(i = 0; i < 3; i++){

		order[(i*3)] = i+1;
		order[(i*3)+1] = i+1;
		order[(i*3)+2] = i+1;
	}
	fpga_set_trigger_order(order);
	fpga_set_input_channel(0, 0x00000001);
	fpga_set_output_channel(0, 0x00000000);
}

void fpga_get_trigger_status(uint8_t *pBuf)
{
	memcpy(pBuf, m_cfg.trigger_buffer, MAX_PACKET_SIZE);
}

static void evt_handler(event_t const* evt, void* p_context)
{
	switch (evt->event)
	{
		case EVT_start_trigger:
			onTriggerCtrl(1);
			break;
		case EVT_stop_trigger:
			onTriggerCtrl(0);
			break;
		case EVT_remote_mode:
			m_cfg.run_mode = evt->param1;
			break;
		default:
			break;
	}
}

static void FpgaProcTask(void const * argument)
{
	uint8_t ch;
	uint8_t cmd_buf[MAX_PACKET_SIZE]={0,}, cmd_pos = 0;
	uint16_t status_timer_tick = 0;

	while(1){
		if(m_cfg.run_trigger == 0){
			osDelay(100);
			continue;
		}

		if(pop_fpga_buf(&ch)){
			if((cmd_pos == 0) && ((ch != 0x33) && (ch != 0xAA))){
				osDelay(10);
				continue;
			}

			if(cmd_pos<MAX_PACKET_SIZE){
				cmd_buf[cmd_pos++] = ch;
			}
			if(cmd_pos == MAX_PACKET_SIZE){
//				if(cmd_buf[1] != 0){
//					LOG_HEX_DUMP(cmd_buf, 5, "RAW Trigger Data");
//				}
#if 0
				if(m_cfg.run_mode == eREMOTE_NONE){
					parse_packet(cmd_buf);
				}else{
					m_remote_response(cmd_buf);
				}
#else
				if(parse_packet(cmd_buf)){
					if((m_cfg.run_mode == eREMOTE_SERIAL) || (m_cfg.run_mode == eREMOTE_ETHER)){
						status_timer_tick = 1;
						if(m_cfg.run_mode == eREMOTE_SERIAL){
							m_remote_response(cmd_buf);
						}else{
							memcpy(m_cfg.trigger_buffer, cmd_buf, MAX_PACKET_SIZE);
						}
					}
				}
#endif
				cmd_pos = 0;
				memset(cmd_buf, 0, MAX_PACKET_SIZE);
			}
			osDelay(5);
		}else{
			if((m_cfg.run_mode == eREMOTE_SERIAL) || (m_cfg.run_mode == eREMOTE_ETHER)){
				if(status_timer_tick && (status_timer_tick++ > TRIGGER_TIME_OUT)){
					LOG_DBG("Clear Trigger Status");
					status_timer_tick = 0;
					memset(cmd_buf, 0, MAX_PACKET_SIZE);
					cmd_buf[0] =0x33;
					if(m_cfg.run_mode == eREMOTE_SERIAL){
						m_remote_response(cmd_buf);
					}else{
						memcpy(m_cfg.trigger_buffer, cmd_buf, MAX_PACKET_SIZE);
					}
				}
			}
			osDelay(100);
		}
	}
}

uint32_t fpga_proc_init(void)
{
	osThreadDef(fpgaProc, FpgaProcTask, osPriorityNormal, 0, 1024);
	fpgaprocTaskHandle = osThreadCreate(osThread(fpgaProc), NULL);

	fpga_event.handler = evt_handler;
	fpga_event.p_context = NULL;
	if(m_event_register(&fpga_event)){
		return 1;
	}

	return 0;
}

