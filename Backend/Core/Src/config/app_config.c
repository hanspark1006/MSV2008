/*
 * app_config.c
 *
 *  Created on: Jul 18, 2024
 *      Author: catsa
 */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "app_config.h"
#include "app_state.h"
#include "m_remote.h"
/* Private define ------------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static app_config_t def_config={
		.model={
				{
					{192,168,0,5},
					{192,168,0,1},
					5050,
				},
				{eREMOTE_FRONT,eChannel_8,{1},{1}}
		}
};

app_config_t *m_app_cfg = &def_config;
/* Private function prototypes -----------------------------------------------*/
int app_config_init(void)
{
	int err_code;

	//LOG_INF("Config Init Start");
	do{
		err_code = app_config_flash_init(&def_config, &m_app_cfg);
		BREAK_IF_ERROR(err_code);

		LOG_DBG("Load configuration OK!!");
		//LOG_HEX_DUMP(m_app_cfg, sizeof(app_config_t), "Load APP Config");
		return 0;
	}while(0);

	err_code = app_config_reset();
	RETURN_IF_ERROR(err_code);

	return err_code;
}

int app_config_reset(void)
{
	int err_code;

	LOG_INF("Reset Configuration");

	memcpy(m_app_cfg, &def_config, sizeof(app_config_t));

	LOG_DBG("Set Default configuration");
	err_code = app_config_flash_renew(m_app_cfg);
	RETURN_IF_ERROR(err_code);

	return 0;
}
#if 1
int app_config_save_dbg(const char *call_func, int line)
#else
int app_config_save(void)
#endif
{
	int err_code;
#if 1
	LOG_INF("Save configuration[%s][%d]", call_func, line);
#else
	LOG_INF("Save configuration");
#endif
	err_code = app_config_flash_config_store(m_app_cfg);
	if(err_code){
		push_event0(EVT_error_screen);
		RETURN_IF_ERROR(err_code);
	}

	//LOG_HEX_DUMP(m_app_cfg, sizeof(app_config_t), "Save APP Config");
	push_event0(EVT_save_ok);
	return err_code;
}

int app_config_save_file(int file_num, char *file_name)
{
	int err_code;

	LOG_INF("Save configuration to file");

	err_code = app_config_flash_save_config(m_app_cfg, file_name, file_num);
	RETURN_IF_ERROR(err_code);

	push_event0(EVT_save_ok);
	return err_code;
}

int app_config_load_file(int file_num)
{
	int err_code;

	LOG_INF("Load configuration to file");

	err_code = app_config_flash_read_config(m_app_cfg, file_num);
	RETURN_IF_ERROR(err_code);

	push_event0(EVT_load_ok);
	return err_code;
}

int app_config_remove_file(int file_num)
{
	int err_code;

	LOG_INF("Remove configuration to file");

	err_code = app_config_flash_remove_config(file_num);
	RETURN_IF_ERROR(err_code);

	push_event0(EVT_Delete_file);
	return err_code;
}

int app_config_read_file(uint8_t file_list[][MAX_SAVE_NAME_LEN])
{
	return app_config_flash_load_file_list(file_list);
}
