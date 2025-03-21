/*
 * m_normal.h
 *
 *  Created on: Feb 9, 2024
 *      Author: Robyn
 */

#ifndef SRC_M_MENU_M_NORMAL_H_
#define SRC_M_MENU_M_NORMAL_H_

/* Define to prevent recursive inclusion -------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/

/* Private defines -----------------------------------------------------------*/
enum{
	ePWM_CH_1,
	ePWM_CH_2,
	ePWM_CH_3,
	ePWM_CH_4,
	ePWM_CH_5,
	ePWM_CH_6,
	ePWM_CH_7,
	ePWM_CH_8,
	ePWM_CH_9,
	ePWM_CH_10,
	ePWM_CH_11,
	ePWM_CH_12,
	ePWM_CH_13,
	ePWM_CH_14,
	ePWM_CH_15,
	ePWM_CH_16,
	ePWM_CH_17,
	ePWM_CH_18,
	ePWM_CH_19,
	ePWM_CH_20,
	ePWM_CH_21,
	ePWM_CH_22,
	ePWM_CH_23,
	ePWM_CH_24,
	ePWM_CH_25,
	ePWM_CH_26,
	ePWM_CH_27,
	ePWM_CH_28,
	ePWM_CH_29,
	ePWM_CH_30,
	ePWM_CH_31,
	ePWM_CH_32,
	MAX_PWM_CH
};

#define STROBE_DELAY					0xC1
#define STROBE_PULSE					0xC2
#define STROBE_RISEFALL					0xC3
#define CV_PWM							0xC4
#define DEVICE_STATUS					0xD1
#define DEVICE_ECHO						0xE0
#define SENSING_LED						0xE1
#define DEVICE_UPDATE					0xF0
#define DEVICE_FIRMWARE_VERSION			114

#define LCD_STATUS_INITIALIZING			30
#define LCD_STATUS_MODE_LOCAL			31
#define LCD_STATUS_MODE_REMOTE			32
#define LCD_STATUS_MODE_EXT				33

#define LCD_STATUS_MODE_STROBE_NORMAL	41
#define LCD_STATUS_MODE_STROBE_REMOTE	42
#define LCD_STATUS_MODE_STROBE_TEST		43
#define LCD_STATUS_SET_TIME_DELAY		44
#define LCD_STATUS_SET_TIME_PULSE		45
#define LCD_STATUS_SET_INPUT			46
#define LCD_STATUS_FIRMWARE_UPDATE		47

enum{
	eStrobe_Init,
	eStrobe_Set,
	eStrobe_run
};
/* Exported types ------------------------------------------------------------*/
typedef struct
{
	int nDeviceNo;
	int nDeviceCurrentStatus;
	int nDeviceCurrentStatus_old;
	int nPWM[MAX_PWM_CH];
	int nDELAY[MAX_PWM_CH];
	int nPULSE[MAX_PWM_CH];
	int nEDGE[MAX_PWM_CH];
	int nSerialPWM[MAX_PWM_CH];
}DEVICE;

/* Exported constants --------------------------------------------------------*/
extern DEVICE Device;
/* Exported macro ------------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/
int m_menu_normal_init(void);
void m_normal_DataSend2(uint8_t channel, int data);
void m_normal_parse_received_data(uint8_t *rcv_data, uint32_t len);
#ifdef __cplusplus
}
#endif

#endif /* SRC_M_MENU_M_NORMAL_H_ */
