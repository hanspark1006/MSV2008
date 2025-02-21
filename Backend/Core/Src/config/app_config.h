/*
 * app_config.h
 *
 *  Created on: Jul 18, 2024
 *      Author: catsa
 */

#ifndef SRC_CONFIG_APP_CONFIG_H_
#define SRC_CONFIG_APP_CONFIG_H_
/* Define to prevent recursive inclusion -------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
#include "app_config_flash.h"
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
extern app_config_t *m_app_cfg;
/* Exported macro ------------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/
int app_config_init(void);
int app_config_reset(void);
int app_config_save(void);
int app_config_save_file(int file_num, char *file_name);
int app_config_load_file(int file_num);
int app_config_read_file(uint8_t file_list[][MAX_SAVE_NAME_LEN]);
int app_config_remove_file(int file_num);

/* Private defines -----------------------------------------------------------*/

#ifdef __cplusplus
}
#endif
#endif /* SRC_CONFIG_APP_CONFIG_H_ */
