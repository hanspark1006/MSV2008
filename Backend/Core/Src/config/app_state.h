/*
 * app_state.h
 *
 *  Created on: Jul 19, 2024
 *      Author: catsa
 */

#ifndef SRC_CONFIG_APP_STATE_H_
#define SRC_CONFIG_APP_STATE_H_
/* Define to prevent recursive inclusion -------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
#include "app_config.h"

/* Private defines -----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
typedef struct{
	app_config_model_t	model;
	fpga_config_t 		config;
}app_state_t;
/* Exported constants --------------------------------------------------------*/
extern app_state_t m_app_state;
/* Exported macro ------------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/
void app_state_init(void);

#ifdef __cplusplus
}
#endif
#endif /* SRC_CONFIG_APP_STATE_H_ */
