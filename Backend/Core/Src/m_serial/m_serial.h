/*
 * m_serial.h
 *
 *  Created on: Feb 8, 2024
 *      Author: catsa
 */

#ifndef SRC_M_SERIAL_M_SERIAL_H_
#define SRC_M_SERIAL_M_SERIAL_H_
/* Define to prevent recursive inclusion -------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
#include "main.h"
/* Exported types ------------------------------------------------------------*/
typedef enum{
	eEXT_UART,
	eFPGA_UART,
	eFRONT_UART
}eUartDev;

enum{
	DIGIT_1,
	DIGIT_3,
	RET_OK,
	RET_IP,
};
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/

int m_serial_init(void);
int m_serial_send(eUartDev dest, uint8_t *send_data, uint32_t len);
void m_serial_SendPC(uint8_t *data);
void ext_push_buf(uint8_t ch);
/* Private defines -----------------------------------------------------------*/
#define MAX_RECV_BUF		1024
#define MAX_RESPONSE_LEN	8

#ifdef __cplusplus
}
#endif

#endif /* SRC_M_SERIAL_M_SERIAL_H_ */
