/*
 * m_event_def.h
 *
 *  Created on: Feb 6, 2024
 *      Author: catsa
 */

#ifndef SRC_M_EVENT_M_EVENT_DEF_H_
#define SRC_M_EVENT_M_EVENT_DEF_H_
/* Define to prevent recursive inclusion -------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/

/* Private defines -----------------------------------------------------------*/


#ifdef __cplusplus
}
#endif

#include "macros_common.h"

typedef enum
{
	EVT_none,
	EVT_menu_start,
	EVT_send_pwm,
	EVT_send_status,
	EVT_received_data,
	EVT_measure_start,
	EVT_received_tcp,
	EVT_start_strobe,
	EVT_Set_ip,
	EVT_Get_ip,
	EVT_save_config,
	EVT_cancel_config,
	EVT_error_screen,
	EVT_save_ok,
	EVT_load_ok,
	EVT_write_file_config,
	EVT_read_file_config,
	EVT_Changescreen,
	EVT_update_time,
	EVT_start_trigger,
	EVT_stop_trigger,
	EVT_self_test,
	EVT_stop_self_test,
	Evt_stop_demotest,
	EVT_remote_mode,
	EVT_fpga_load_done,
	EVT_set_remotetime,
	EVT_close_screen,
	EVT_input_screen,
	EVT_Delete_file,
	EVT_draw_status,
	EVT_Set_DevID,

} event_type_t;

const char* event_type_2_str(event_type_t evt);

#endif /* SRC_M_EVENT_M_EVENT_DEF_H_ */
