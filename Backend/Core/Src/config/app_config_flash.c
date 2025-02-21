/*
 * app_config_flash.c
 *
 *  Created on: Jul 18, 2024
 *      Author: catsa
 */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

#include "app_config_flash.h"
#include "m_flash.h"
#include "crc32_sw.h"
#include "macros_common.h"

/* Private define ------------------------------------------------------------*/
#define FLASH_CONFIG_VALID	0x18UL
#define DEF_CRC				0x04030201

/* Private typedef -----------------------------------------------------------*/
#pragma pack(push)
#pragma pack(1)
typedef struct{
	uint32_t		valid;
	uint32_t		crc;
	app_config_t	dev_cfg;
}app_cfg_flash_data_t;

typedef struct{
	app_cfg_flash_data_t	data;
}app_dev_db_t;

typedef union{
	app_dev_db_t			dev_db;
	uint32_t				padding[CEIL_DIV(sizeof(app_dev_db_t),4)];
}app_cfg_flash_config_t;
#pragma pack(pop)
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
CCMRAM static app_cfg_flash_config_t	m_config;
/* Private function prototypes -----------------------------------------------*/
int app_config_flash_config_store(app_config_t *p_config)
{
    int            err_code;

    NULL_PARAM_CHECK(p_config);

	if (p_config != &m_config.dev_db.data.dev_cfg)
		memcpy(&m_config.dev_db.data.dev_cfg, p_config, sizeof(app_config_t));

    m_config.dev_db.data.valid = FLASH_CONFIG_VALID;
	m_config.dev_db.data.crc   = crc32_ieee((uint8_t*)&m_config.dev_db.data.dev_cfg, sizeof(app_config_t));

	err_code = m_flash_config_erase();
	err_code |= m_flash_config_write(&m_config, sizeof(app_cfg_flash_config_t));
    RETURN_IF_ERROR(err_code);

    return 0;
}

int app_config_flash_config_load(app_config_t ** p_config)
{
	uint32_t crc;

	m_flash_config_read(&m_config, sizeof(app_cfg_flash_config_t));

	if(m_config.dev_db.data.valid != FLASH_CONFIG_VALID){
		LOG_DBG("Flash valid code error!!");
		return -1;
	}

	*p_config = &m_config.dev_db.data.dev_cfg;
	crc = crc32_ieee((uint8_t *)&m_config.dev_db.data.dev_cfg, sizeof(app_config_t));
	if(crc != m_config.dev_db.data.crc){
		LOG_ERR("Flash crc error!!");
		return -1;
	}

	return 0;
}

static void set_default_fgpa_value(void)
{
	int i;

	memset(m_config.dev_db.data.dev_cfg.cfg.delay_time, 0, sizeof(uint16_t) * MAX_CHANNEL);
	for(i = 0; i < MAX_CHANNEL;i++){
		m_config.dev_db.data.dev_cfg.cfg.on_time[i] = 1000;
	}
	m_config.dev_db.data.dev_cfg.cfg.out_mode = eMODE_OUN_ONE_N;
	m_config.dev_db.data.dev_cfg.cfg.out_cfg.one_n.input = 1;

	m_config.dev_db.data.valid = FLASH_CONFIG_VALID;
	m_config.dev_db.data.crc = crc32_ieee((uint8_t *)&m_config.dev_db.data.dev_cfg, sizeof(app_config_t));
}

int app_config_flash_renew(app_config_t* p_config)
{
	uint32_t err_code;

	LOG_INF("Write default config\n");

	if (p_config != &m_config.dev_db.data.dev_cfg)
		memcpy(&m_config.dev_db.data.dev_cfg, p_config, sizeof(app_config_t));

	set_default_fgpa_value();

	m_config.dev_db.data.valid = FLASH_CONFIG_VALID;
	m_config.dev_db.data.crc   = crc32_ieee((uint8_t*)&m_config.dev_db.data.dev_cfg, sizeof(app_config_t));

	err_code = m_flash_config_erase();
	err_code |= m_flash_config_write(&m_config, sizeof(app_cfg_flash_config_t));
    RETURN_IF_ERROR(err_code);

	return err_code;
}

int app_config_flash_init(const app_config_t * p_default_config, app_config_t ** p_config)
{
	uint32_t errcode;

	if(app_config_flash_config_load(p_config)){
		LOG_DBG("Configuration Load Error..Write default configuration");

		memcpy(&m_config.dev_db.data.dev_cfg, p_default_config, sizeof(app_config_t));

		set_default_fgpa_value();

		errcode = m_flash_config_erase();
		errcode |= m_flash_config_write(&m_config, sizeof(app_cfg_flash_config_t));
		RETURN_IF_ERROR(errcode);

		*p_config = &m_config.dev_db.data.dev_cfg;
	}

	return 0;
}
