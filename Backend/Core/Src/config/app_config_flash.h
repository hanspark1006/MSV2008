/*
 * app_config_flash.h
 *
 *  Created on: Jul 18, 2024
 *      Author: catsa
 */

#ifndef SRC_CONFIG_APP_CONFIG_FLASH_H_
#define SRC_CONFIG_APP_CONFIG_FLASH_H_

/* Define to prevent recursive inclusion -------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/

#include "fpga_proc.h"

/* Private defines -----------------------------------------------------------*/
#define MAX_SAVE_FILE_NUM	8
#define MAX_SAVE_NAME_LEN	20
#define MAX_PWD_LEN			5

/* Exported types ------------------------------------------------------------*/
#pragma pack(push)
#pragma pack(1)
typedef struct _modle_t
{
	uint8_t SelectChannel;
	uint8_t	MCU_FW_Ver[10];
	uint8_t FPGA_FW_Ver[10];
}sys_info_t;

typedef struct _model_ip_
{
	uint8_t ipaddr[4];
	uint8_t submask[4];
	uint16_t port;
	uint8_t dev_name[16];
}model_ip_net_t;

typedef struct{
	model_ip_net_t	IpAddress;
	sys_info_t	System;
}app_config_model_t;

typedef struct{
	app_config_model_t	model;
	fpga_config_t		cfg;
}app_config_t;
#pragma pack(pop)

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
enum{
	INPUT_EDGE_FALLING,
	INPUT_EDGE_RISING
};

enum{
	eMODE_OUT_ONE_ONE,
	eMODE_OUN_ONE_N
};

enum{
	eChannel_1,
	eChannel_2,
	eChannel_4,
	eChannel_6,
	eChannel_8
};
/* Exported functions prototypes ---------------------------------------------*/
int app_config_flash_load_file_list(uint8_t file_list[][MAX_SAVE_NAME_LEN]);
int app_config_flash_read_config(app_config_t *p_config, uint8_t file_idx);
int app_config_flash_save_config(app_config_t *p_config, char *file_name, uint8_t file_idx);
int app_config_flash_remove_config(uint8_t file_idx);
int app_config_flash_config_store(app_config_t * p_config);
int app_config_flash_config_load(app_config_t ** p_config);
int app_config_flash_renew(app_config_t* p_config);
int app_config_flash_init(const app_config_t * p_default_config,
                          app_config_t      ** p_config);

#ifdef __cplusplus
}
#endif

#endif /* SRC_CONFIG_APP_CONFIG_FLASH_H_ */
