/*
 * m_serial.c
 *
 *  Created on: Feb 8, 2024
 *      Author: catsa
 */


/*
 * uart_proc.c
 *
 *  Created on: Jan 5, 2024
 *      Author: catsa
 */
/* Includes ------------------------------------------------------------------*/
#include <m_serial.h>
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <stdarg.h>
#include "main.h"
#include "cmsis_os.h"

#include "m_normal.h"
#include "fpga_proc.h"
#include "m_remote.h"
#include "m_ether.h"
#include "m_front.h"
#include "m_serial.h"
/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define EXT_UART			(&huart1)
#define FRONT_UART			(&huart2)
#define FPGA_UART			(&huart6)

#define USE_SEND_IT			1
#define UART_TIME_OUT		1000
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static uint8_t Fpga_rx_buf[1];
static uint8_t Front_rx_buf[1];
static uint8_t Ext_rx_buf[1];
#if USE_SEND_IT
static uint8_t g_tx_complete;
#endif

/* Private function prototypes -----------------------------------------------*/

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	/* Console */
	if ( huart->Instance == USART6 ){
		fpga_push_buf(Fpga_rx_buf[0]);
		HAL_UART_Receive_IT(FPGA_UART , &Fpga_rx_buf[0], 1 );
	}else if(huart->Instance == USART1){
		remote_push_buf(eREMOTE_SERIAL, Ext_rx_buf[0]);
		HAL_UART_Receive_IT(EXT_UART , &Ext_rx_buf[0], 1 );
	}else if(huart->Instance == USART2){
		front_push_buf(Front_rx_buf[0]);
		HAL_UART_Receive_IT(FRONT_UART , &Front_rx_buf[0], 1 );
	}
}

#if USE_SEND_IT
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *UartHandle)
{
	g_tx_complete = 1;
}
#endif

int m_serial_send(eUartDev dest, uint8_t *send_data, uint32_t len)
{
	UART_HandleTypeDef *huart = FPGA_UART;
	int error, timeout = 0;
//	char str_buf[30];

	if(dest == eFRONT_UART)
		huart = FRONT_UART;
	else if(dest == eEXT_UART)
		huart = EXT_UART;
#if USE_SEND_IT
//	if(dest == eFPGA_UART){
//		LOG_HEX_DUMP(send_data, len, "FPGA Send Data");
//	}
//	if(dest ==eEXT_UART){
//		LOG_HEX_DUMP(send_data, len, "Send PC Data");
//	}
	while(huart->gState != HAL_UART_STATE_READY){
		osDelay(10);
		if(timeout++ > 10){
			LOG_DBG("Uart busy....");
			timeout = 0;
		}
	}
	timeout = 0;
	for(int i = 0; i < 3; i++){
		error = HAL_UART_Transmit_IT (huart, send_data, len);
		if (error == HAL_OK){
			break;
		}
		osDelay(10);
	}
	if (error != HAL_OK){
		RETURN_IF_ERROR(error);
	}

	while(HAL_UART_GetState(huart) == HAL_UART_STATE_BUSY_TX || HAL_UART_GetState(huart) == HAL_UART_STATE_BUSY_TX_RX){
		if(timeout++ > UART_TIME_OUT){
			RETURN_IF_ERROR(error);
		}
		osDelay(10);
	}

	timeout = 0;
	while(g_tx_complete == 0){
		if(timeout++ > UART_TIME_OUT){
			g_tx_complete = 0;
			RETURN_IF_ERROR(error);
		}
		osDelay(10);
	}
#else
/*
	sprintf(str_buf, "%s Data Send", dest == eLCD_UART?"LCD":"FPGA");
	LOG_HEX_DUMP(send_data, len, str_buf);
*/
	while(HAL_UART_GetState(huart) == HAL_UART_STATE_BUSY_TX || HAL_UART_GetState(huart) == HAL_UART_STATE_BUSY_TX_RX){
		if(timeout++ > UART_TIME_OUT){
			RETURN_IF_ERROR(err_code);
		}
		osDelay(10);
	}

	error = HAL_UART_Transmit(huart, send_data, len, UART_TIME_OUT);
	RETURN_IF_ERROR(error);
#endif

	return error;
}

void m_serial_SendPC(uint8_t *data)
{
	//LOG_HEX_DUMP(data, MAX_RESPONSE_LEN, "Remote Send Data");
	m_serial_send(eEXT_UART, data, MAX_RESPONSE_LEN);
}

int m_serial_init(void)
{
	int err_code;

	LOG_INF("Serial Init");
	//osDelay(300);

	err_code = fpga_proc_init();
	RETURN_IF_ERROR(err_code);

	err_code = remote_proc_init();
	RETURN_IF_ERROR(err_code);

	HAL_UART_Receive_IT(FPGA_UART, &Fpga_rx_buf[0], 1);
	HAL_UART_Receive_IT(EXT_UART, &Ext_rx_buf[0], 1);
	HAL_UART_Receive_IT(FRONT_UART, &Front_rx_buf[0], 1);

	return 0;
}
