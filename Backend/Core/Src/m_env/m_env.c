/*
 * m_env.c
 *
 *  Created on: Feb 9, 2024
 *      Author: Robyn
 */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "m_normal.h"
#include "m_env.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define MAX_DIP_SW_CHANNEL	4

#define I2C_TIMEOUT			1000
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
int m_dipsw_read_Channel(void)
{
#if 0
	GPIO_TypeDef *dipsw_port[MAX_DIP_SW_CHANNEL] ={DIPSW1_GPIO_Port, DIPSW2_GPIO_Port, DIPSW3_GPIO_Port, DIPSW4_GPIO_Port};
	uint16_t	dipsw_pin[MAX_DIP_SW_CHANNEL] = {DIPSW1_Pin, DIPSW2_Pin, DIPSW3_Pin, DIPSW4_Pin};
	int devNo = 0;

	for(int i=0; i < MAX_DIP_SW_CHANNEL; i++){
		if(HAL_GPIO_ReadPin(dipsw_port[i], dipsw_pin[i]) == 0){
			devNo |= 1<<i;
		}
	}

	if(devNo == 0){
		devNo = 1;
	}

	//LOG_INF("device No : %d", devNo);
	return devNo;
#endif
	return 0;
}

void m_env_active_mode_led(uint8_t active)
{
#if 0
	uint16_t active_pin[]={LED_LOCAL_Pin, LED_RS232_Pin, LED_LAN_Pin};

	//LOG_DBG("Led Active id[%d]", active);
	for(int i = 0; i < 3; i++){
		HAL_GPIO_WritePin(GPIOB, active_pin[i], i==active?GPIO_PIN_RESET:GPIO_PIN_SET);
	}
#endif
}

int m_env_e2p_write(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size)
{
	HAL_StatusTypeDef ret;

	ret = HAL_I2C_Mem_Write(hi2c, DevAddress,  MemAddress,  MemAddSize, pData, Size, I2C_TIMEOUT);
	if(ret != HAL_OK)
	{
		LOG_ERR("[%02x]I2c_Write_E2p Error_no [%d]", DevAddress, ret);
		return -1;
	}
	return 0;
}

int m_env_e2p_read(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size)
{
	HAL_StatusTypeDef ret;

	ret = HAL_I2C_Mem_Read(hi2c, DevAddress, MemAddress, MemAddSize, pData, Size, I2C_TIMEOUT);
	if(ret != HAL_OK)
	{
		LOG_ERR("[%02x]I2c_Read_E2p Error_no [%d]", DevAddress, ret);
		return -1;
	}
	return 0;
}

