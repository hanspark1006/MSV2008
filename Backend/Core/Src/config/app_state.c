/*
 * app_state.c
 *
 *  Created on: Jul 19, 2024
 *      Author: catsa
 */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "app_config.h"
#include "app_state.h"
/* Private define ------------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
app_state_t m_app_state;
/* Private function prototypes -----------------------------------------------*/
void app_state_init(void)
{
	memcpy(&m_app_state.config, &m_app_cfg->cfg, sizeof(fpga_config_t));
	memcpy(&m_app_state.model,  &m_app_cfg->model, sizeof(app_config_model_t));
}
