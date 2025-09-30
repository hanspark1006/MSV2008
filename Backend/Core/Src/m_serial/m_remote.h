/*
 * m_remote.h
 *
 *  Created on: Oct 15, 2024
 *      Author: catsa
 */

#ifndef SRC_M_SERIAL_M_REMOTE_H_
#define SRC_M_SERIAL_M_REMOTE_H_

#include "fpga_proc.h"

#define REMOTE_HEADER	0xEF
#define REMOTE_TAIL		0xFEFF
#define RESPONSE_HEADER	0xDF
#define FREQ_140KHz		140

typedef struct _remote_time{
	uint16_t	on_time[MAX_CHANNEL];
	uint16_t	delay_time[MAX_CHANNEL];
}Remote_Time_t;

enum{
	OUTMODE_1_1 = 0x00,
	OUTMODE_1_N,
	OUTMODE_1_N_SEQ,
	OUTMODE_GRP_OUT,
	OUTMODE_GRP_SEQ,
};
enum{
	eCOMMAND_CON_CMD = 0xA0,

	eCOMMAND_RDY_CMD = 0xB0,
	eCOMMAND_OUT_MODE,
	eCOMMAND_ON_TIME,
	eCOMMAND_DELAY_TIME,
	eCOMMAND_INPUT_CHANNEL,
	eCOMMAND_OUTPUT_CHANNEL,
	eCOMMAND_TRIGGER_ORDER,
	eCOMMAND_TRIGGER_ORDER_SET,
	eCOMMAND_INPUT_EDGE,
	eCOMMAND_FACTORY_RESET,
	eCOMMAND_REMOTE_IP_STATUS,
	eCOMMAND_CLOSE_REMOTE,
	eCOMMAND_SETOPMODE,
	eCOMMAND_SETDUTY,
	eCOMMAND_SETPERIOD,

	eCOMMAND_SET_GR_IN = 0xC0,
	eCOMMAND_SET_GR_OUT = 0xC1,

	eRESPONSE_OK = 0xD0,
	eRESPONSE_ERROR,
	eRESPONSE_CON_DONE,
	eRESPONSE_RDY_LOAD_DONE,
};

typedef enum {
	eREMOTE_NONE,
	eREMOTE_SERIAL,
	eREMOTE_ETHER,
	eREMOTE_FRONT,
	eREMOTE_CLOSE
}remote_mode_t;

void m_remote_set_prev_mode(remote_mode_t mode);
uint8_t m_remote_front_parser(uint8_t *recv_buf, uint16_t size);
uint8_t m_remote_eth_parser(uint8_t *recv_buf, uint16_t size);
void m_remote_response(uint8_t *data);
void remote_push_buf(remote_mode_t eSource, uint8_t ch);
uint8_t remote_proc_init(void);
#endif /* SRC_M_SERIAL_M_REMOTE_H_ */
