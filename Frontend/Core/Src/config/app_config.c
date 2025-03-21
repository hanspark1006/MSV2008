
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "string.h"
#include "app_config.h"
#include "m_flash.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define CONFIG_VALID		0x10UL
#define FRONT_VERSION				1
#define BACKEND_VERSION				1

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static config_t def_config={
		.valid=CONFIG_VALID,
		.mode = eONE_ONE,
		.dev_id = 1,
		.ch_num = 4,
		.on_time={1000,1000,1000,1000,1000,1000,1000,1000},
		.delay_time={0,0,0,0,0,0,0,0},
		.edge={eFALLING,eFALLING,eFALLING,eFALLING,eFALLING,eFALLING,eFALLING,eFALLING},
		.ether={{192,168,0,5},5050},
		.company=" YNS-Vision     ",
		.frontVer=1,
		.backendVer=1
};

config_t m_flash_cfg, *m_app_config;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void app_set_default_config(void)
{
	LOG_INF("Set Default Configuration...");
	//LOG_HEX_DUMP(&m_flash_cfg, sizeof(config_t), "Before Config Data");
	memcpy(&m_flash_cfg, &def_config, sizeof(config_t));
	m_flash_config_write((void *)&m_flash_cfg, sizeof(config_t));
	//LOG_HEX_DUMP(&m_flash_cfg, sizeof(config_t), "After Config Data");
}

void app_config_init(void)
{
	if(app_read_config(&m_flash_cfg)){
		app_set_default_config();
	}
	m_app_config = &m_flash_cfg;
	LOG_HEX_DUMP(&m_flash_cfg, sizeof(config_t), "Config Data");
}

int app_save_config(void)
{
	return m_flash_config_write((void *)&m_flash_cfg, sizeof(config_t));
}

uint8_t app_read_config(config_t *pConfig)
{
	m_flash_config_read((void *)pConfig, sizeof(config_t));
	if(pConfig->valid != CONFIG_VALID){
		LOG_ERR("Configuration is not valid!!!");
		return 1;
	}
	return 0;
}
