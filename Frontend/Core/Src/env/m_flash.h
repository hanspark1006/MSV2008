/*
 * m_flash.h
 *
 *  Created on: Jul 15, 2024
 *      Author: catsa
 */

#ifndef SRC_M_ENV_M_FLASH_H_
#define SRC_M_ENV_M_FLASH_H_
/* Define to prevent recursive inclusion -------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/
int m_flash_config_erase(void);
void m_flash_config_read(void *pBuffer, int size);
int m_flash_config_write(void *Data_p, int size);
/* Private defines -----------------------------------------------------------*/

#define ADDR_FLASH_PAGE_62    ((uint32_t)0x0800F800) /* Base @ of Page 62, 1 Kbytes */

#define FRONT_CONFIG_ADDRESS	ADDR_FLASH_PAGE_62

#ifdef __cplusplus
}
#endif
#endif /* SRC_M_ENV_M_FLASH_H_ */
