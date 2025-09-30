
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "string.h"
#include "app_config.h"
#include "m_flash.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define CONFIG_VALID		0x16UL
#define FRONT_VERSION				25
#define BACKEND_VERSION				25

/* Private macro -------------------------------------------------------------*/
#define FREQ_140KHz	140
/* Private variables ---------------------------------------------------------*/
static config_t def_config={
		.valid=CONFIG_VALID,
		.remote_mode = eREMOTE_NONE,
		.out_mode = eONE_ONE,
#if	ENABLE_DIMMING_MODE
		.op_mode = eSTROBE_MODE,
#endif
		.dev_id = 1,
		.ch_num = e8CH_MODE,
		.on_time={1000,1000,1000,1000,1000,1000,1000,1000,1000},
		.delay_time={0,0,0,0,0,0,0,0,0},
		.edge=eFALLING,
#if	ENABLE_DIMMING_MODE
		.duty_level={100,100,100,100,100,100,100,100,100},
		.period_time = FREQ_140KHz,
#endif
		.ether={{192,168,0,5},5050},
		.company=" YNS-Vision     ",
		.frontVer=FRONT_VERSION,
		.backendVer=BACKEND_VERSION
};

config_t m_flash_cfg, *m_app_config;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void app_set_default_config(uint8_t sel_ch)
{
	LOG_INF("Set Default Configuration...");
	//LOG_HEX_DUMP(&m_flash_cfg, sizeof(config_t), "Before Config Data");
	memcpy(&m_flash_cfg, &def_config, sizeof(config_t));
	m_flash_cfg.ch_num = sel_ch;
	m_flash_config_write((void *)&m_flash_cfg, sizeof(config_t));
	//LOG_HEX_DUMP(&m_flash_cfg, sizeof(config_t), "After Config Data");
}

void app_config_init(void)
{
	if(app_read_config(&m_flash_cfg)){
		app_set_default_config(e8CH_MODE);
	}
	LOG_DBG("Load Remote mode[%d]", m_flash_cfg.remote_mode);
	LOG_DBG("Load OP mode[%d]", m_flash_cfg.op_mode);
	m_app_config = &m_flash_cfg;
	//LOG_HEX_DUMP(&m_flash_cfg, sizeof(config_t), "Config Data");
}

int app_save_config(void)
{
	LOG_DBG("Save Config OP Mode[%d]", m_flash_cfg.op_mode);
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
