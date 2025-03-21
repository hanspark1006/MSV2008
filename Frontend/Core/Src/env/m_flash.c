/*
 * m_flash.c
 *
 *  Created on: Jul 15, 2024
 *      Author: catsa
 */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "m_flash.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
int m_flash_config_erase(void)
{
	int err_code;
	uint32_t PageError;
	FLASH_EraseInitTypeDef EraseInitStruct;

	HAL_FLASH_Unlock();

	EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
	EraseInitStruct.PageAddress = FRONT_CONFIG_ADDRESS;
	EraseInitStruct.NbPages     = 1;

	err_code = HAL_FLASHEx_Erase(&EraseInitStruct, &PageError);
	if (err_code != HAL_OK)
	{
		LOG_ERR("Page Error[%x] Flash Error[%x]", PageError, HAL_FLASH_GetError());
		HAL_FLASH_Lock();
		RETURN_IF_ERROR(err_code);
	}

	HAL_FLASH_Lock();
	return 0;
}

void m_flash_config_read(void *pBuffer, int size)
{
	uint32_t address;

	address = FRONT_CONFIG_ADDRESS;

	memcpy(pBuffer, (uint32_t *)address, size);
	//LOG_HEX_DUMP(pBuffer, size, "Dump DB");
}

int m_flash_config_write(void *Data_p, int size)
{
	unsigned int write_size =0;
	uint32_t Address, write_data;
	uint16_t *pData;

	Address = FRONT_CONFIG_ADDRESS;

	pData = (uint16_t *)Data_p;

	if(m_flash_config_erase()){
		return -1;
	}

	HAL_FLASH_Unlock();

	while(write_size < size)
	{
		write_data = (uint16_t)*pData;
		if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, Address, write_data) != HAL_OK)
		{

			LOG_ERR("Flash Write Error [%lx] TotSize[%d] WriteSize[%d]", HAL_FLASH_GetError(), size, write_size);

			HAL_FLASH_Lock();
			return -1;
		}
		pData++;
		write_size+=2;
		Address +=2;
	}

	HAL_FLASH_Lock();
	return 0;
}

