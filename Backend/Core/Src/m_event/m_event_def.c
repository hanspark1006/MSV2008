/*
 * m_event_def.c
 *
 *  Created on: Feb 6, 2024
 *      Author: catsa
 */
#include "main.h"
#include "m_event_def.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
const char* event_type_2_str(event_type_t evt)
{
    switch (evt)
    {
    	case_str(EVT_none)
		case_str(EVT_menu_start)
    	case_str(EVT_send_pwm)
		case_str(EVT_send_status)
    	case_str(EVT_received_data)
		case_str(EVT_start_strobe)
    	case_str(EVT_measure_start)
		case_str(EVT_received_tcp)
		case_str(EVT_Set_ip)
		case_str(EVT_Get_ip)
		case_str(EVT_save_config)
		case_str(EVT_error_screen)
		case_str(EVT_save_ok)
		case_str(EVT_load_ok)
		case_str(EVT_write_file_config)
		case_str(EVT_read_file_config)
		case_str(EVT_Changescreen)
		case_str(EVT_update_time)
		case_str(EVT_start_trigger)
		case_str(EVT_stop_trigger)
		case_str(EVT_self_test)
		case_str(EVT_stop_self_test)
		case_str(Evt_stop_demotest)
		case_str(EVT_remote_mode)
		case_str(EVT_fpga_load_done)
		case_str(EVT_set_remotetime)
		case_str(EVT_close_screen)
		case_str(EVT_input_screen)
		case_str(EVT_Delete_file)
		case_str(EVT_draw_status)
    	default :
			break;
    }

    return "Unknown event";
}

