/*
 * m_env.h
 *
 *  Created on: Feb 9, 2024
 *      Author: Robyn
 */

#ifndef SRC_M_ENV_M_ENV_H_
#define SRC_M_ENV_M_ENV_H_
/* Define to prevent recursive inclusion -------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/
int m_dipsw_read_Channel(void);
void m_env_active_mode_led(uint8_t active);

int m_env_e2p_write(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size);
int m_env_e2p_read(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size);
/* Private defines -----------------------------------------------------------*/
enum{
	eACTIVE_LOCAL_LED,
	eACTIVE_RS232_LED,
	eACTIVE_ETHER_LED
};

#ifdef __cplusplus
}
#endif

#endif /* SRC_M_ENV_M_ENV_H_ */
