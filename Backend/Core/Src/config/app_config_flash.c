/*
 * app_config_flash.c
 *
 *  Created on: Jul 18, 2024
 *      Author: catsa
 */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"

#include "app_config_flash.h"
#if (USE_CFG_ETH_E2P == 0)
#include "m_flash.h"
#endif
#include "m_env.h"
#include "crc32_sw.h"
#include "macros_common.h"

/* Private define ------------------------------------------------------------*/
#define FLASH_CONFIG_VALID	0x23UL
#define DEF_CRC				0x04030201

#if (USE_CFG_ETH_E2P == 1)
#define AT24EEP_ADDR		0xA0
#define E2P_I2C_HANDLE		hi2c1
#define E2P_MAX_SIZE		256
#define E2P_WRITE_SIZE		16
#endif
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
    int			err_code;
    uint8_t		e2p_data[E2P_MAX_SIZE]={0,};
#if (USE_CFG_ETH_E2P == 1)
	uint8_t size;

	size = sizeof(app_cfg_flash_config_t);
	if(size > E2P_MAX_SIZE){
		LOG_ERR("Check E2P size!! CFG size[%d]", size);
		return 1;
	}
#endif
    NULL_PARAM_CHECK(p_config);

	if (p_config != &m_config.dev_db.data.dev_cfg){
		LOG_DBG("Update device configuration Data");
		memcpy(&m_config.dev_db.data.dev_cfg, p_config, sizeof(app_config_t));
	}

    m_config.dev_db.data.valid = FLASH_CONFIG_VALID;
	m_config.dev_db.data.crc   = crc32_ieee((uint8_t*)&m_config.dev_db.data.dev_cfg, sizeof(app_config_t));
#if (USE_CFG_ETH_E2P == 0)
	err_code = m_flash_config_erase();
	err_code |= m_flash_config_write(&m_config, sizeof(app_cfg_flash_config_t));

#else
	//LOG_HEX_DUMP(&m_config, size, "Write E2p");
	memcpy(e2p_data, &m_config.dev_db, sizeof(app_cfg_flash_config_t));
	for (uint16_t addr = 0; addr < 256; addr += 16) {
		err_code =m_env_e2p_write(&E2P_I2C_HANDLE, AT24EEP_ADDR, addr, I2C_MEMADD_SIZE_8BIT, &e2p_data[addr], E2P_WRITE_SIZE);
	    osDelay(5);  // 내부 write cycle 시간
	    if(err_code){
	    	break;
	    }
	}
#endif
    RETURN_IF_ERROR(err_code);
    return 0;
}

int app_config_flash_config_load(app_config_t ** p_config)
{
	uint32_t crc;

#if (USE_CFG_ETH_E2P == 0)
	m_flash_config_read(&m_config, sizeof(app_cfg_flash_config_t));
#else
	uint8_t size;
	uint16_t mem_address;

	mem_address = 0;
	size = sizeof(app_cfg_flash_config_t);
	if(size > E2P_MAX_SIZE){
		LOG_ERR("Check E2P size!! CFG size[%d]", size);
		return 1;
	}
	if(m_env_e2p_read(&E2P_I2C_HANDLE, AT24EEP_ADDR, mem_address, 1, (uint8_t *)&m_config.dev_db, size)){
		LOG_ERR("Get Configuration EEP Read Error\r\n");
		return 1;
	}
#endif
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
	memset(m_config.dev_db.data.dev_cfg.cfg.out_cfg.n_n_grp, 0, sizeof(channel_t)*4);

	m_config.dev_db.data.valid = FLASH_CONFIG_VALID;
	m_config.dev_db.data.crc = crc32_ieee((uint8_t *)&m_config.dev_db.data.dev_cfg, sizeof(app_config_t));
}

int app_config_flash_renew(app_config_t* p_config)
{
	uint32_t err_code;
	uint8_t		e2p_data[E2P_MAX_SIZE];
#if (USE_CFG_ETH_E2P == 1)
	int size;
#endif
	LOG_INF("Write default config\n");

	if (p_config != &m_config.dev_db.data.dev_cfg)
		memcpy(&m_config.dev_db.data.dev_cfg, p_config, sizeof(app_config_t));

	set_default_fgpa_value();

	m_config.dev_db.data.valid = FLASH_CONFIG_VALID;
	m_config.dev_db.data.crc   = crc32_ieee((uint8_t*)&m_config.dev_db.data.dev_cfg, sizeof(app_config_t));
#if (USE_CFG_ETH_E2P == 0)
	err_code = m_flash_config_erase();
	err_code |= m_flash_config_write(&m_config, sizeof(app_cfg_flash_config_t));
#else
    size = sizeof(app_cfg_flash_config_t);
	if(size > E2P_MAX_SIZE){
		LOG_ERR("Check E2P size!! CFG size[%d]", size);
		return 1;
	}

//	LOG_HEX_DUMP(&m_config, size, "Write E2p");
	memcpy(e2p_data, &m_config.dev_db, sizeof(app_cfg_flash_config_t));
	for (uint16_t addr = 0; addr < 256; addr += 16) {
		err_code =m_env_e2p_write(&E2P_I2C_HANDLE, AT24EEP_ADDR, addr, I2C_MEMADD_SIZE_8BIT, &e2p_data[addr], E2P_WRITE_SIZE);
	    osDelay(5);  // 내부 write cycle 시간
	    if(err_code){
	    	break;
	    }
	}
#endif
    RETURN_IF_ERROR(err_code);

	return err_code;
}

int app_config_flash_init(const app_config_t * p_default_config, app_config_t ** p_config)
{
	uint32_t errcode;
#if (USE_CFG_ETH_E2P == 1)
	uint8_t e2p_data[E2P_MAX_SIZE];
	int size;
#endif
	if(app_config_flash_config_load(p_config)){
		LOG_DBG("Configuration Load Error..Write default configuration");

		memcpy(&m_config.dev_db.data.dev_cfg, p_default_config, sizeof(app_config_t));
		m_config.dev_db.data.valid = FLASH_CONFIG_VALID;
		m_config.dev_db.data.crc   = crc32_ieee((uint8_t*)&m_config.dev_db.data.dev_cfg, sizeof(app_config_t));

		set_default_fgpa_value();
#if (USE_CFG_ETH_E2P == 0)
		errcode = m_flash_config_erase();
		errcode |= m_flash_config_write(&m_config, sizeof(app_cfg_flash_config_t));
#else
		size = sizeof(app_cfg_flash_config_t);
		if(size > E2P_MAX_SIZE){
			LOG_ERR("Check E2P size!! CFG size[%d]", size);
			return 1;
		}
		LOG_INF("Config Size!![%d]", size);
//		LOG_HEX_DUMP(&m_config, size, "Write E2p");
		memcpy(e2p_data, &m_config.dev_db, sizeof(app_cfg_flash_config_t));
		for (uint16_t addr = 0; addr < size; addr += 16) {
			LOG_DBG("Write Addr[%d]", addr);
			errcode =m_env_e2p_write(&E2P_I2C_HANDLE, AT24EEP_ADDR, addr, I2C_MEMADD_SIZE_8BIT, &e2p_data[addr], E2P_WRITE_SIZE);
		    osDelay(5);  // 내부 write cycle 시간
		    if(errcode){
		    	break;
		    }
		}
#endif
		RETURN_IF_ERROR(errcode);

		*p_config = &m_config.dev_db.data.dev_cfg;
	}

	return 0;
}
