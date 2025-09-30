/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "lcd.h"

#include "app_config.h"
#include "screen.h"
#include "apps.h"
#include "uart.h"
#include "trigger.h"
/* Private typedef -----------------------------------------------------------*/
typedef struct{
	ScreenID_t sc_id;
	uint16_t key;
	uint8_t	status;	
}Screen_param_t;

typedef union{
	uint8_t array_time[2];
	uint16_t u16_time;
}uChtime;

typedef union{
	uint8_t array_port[2];
	uint16_t u16_port;
}uPort_t;

typedef ScreenID_t (*screen_func_t) (Screen_param_t param);

/* Private define ------------------------------------------------------------*/
enum{
	eCH_CHSelect,
	eCH_AllCH,
	eCH_OnTIME,
	eCH_AllOnTIME,
	eCH_DelayTIME,
	eCH_AllDelayTIME,
	eCH_TriggerSet,
	eCH_AllTriggerSet,
	eSEL_OutMode,
	eSET_OutMode,
#if	ENABLE_DIMMING_MODE
	eSET_OpMode,
	eDuty_levle,
	eDraw_OpMode,
#endif
	eRS_DevID,
	eIP_Set,
	ePORT_Set,
	//eSelf_Test,
	eRemote_Mode,
	eUSER_Mode,
	eFactor_Mode,
	eSEL_OUT_Ch,
	eVer_Disp,
	eCH_MAXStep
};

typedef enum{
	eCMD_ReadVersion=0x10,
	eCMD_OutMode=0xB1,
	eCMD_Ontime,
	eCMD_Delaytime,
	eCMD_InputCh,
	eCMD_OutCh,			// 0xB5
	eCMD_Trigger,
	eCMD_TriggerSet,
	eCMD_InputEdge,
	eCMD_Factory,		// 0xB9
	eCMD_Status,
	eCMD_CloseRemote,
#if	ENABLE_DIMMING_MODE
	eCMD_SetOPMode,
	eCMD_SetDuty,		// 0xBd
	eCMD_SetPeriod,
#endif
	eCMD_FR_SetRemote = 0xE0,
	eCMD_FR_SelfTest,
	eCMD_FR_SetDevID,
	eCMD_FR_SetIP,
	eCMD_FR_SetPort,
}BackEnd_Cmd_t;

#define MAX_DEV_ID	10
#define LINE_CHARACTER_MAX	16
#define LCD_PRINT_MAX		20

#define MIN_PORT	1024
#define MAX_PORT	65536

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static const char charSet[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789 -_";
#define CHARSET_LENGTH (sizeof(charSet) - 1)
/* Private function prototypes -----------------------------------------------*/
static ScreenID_t save_data(void);
static void send_cmd_2_backend(BackEnd_Cmd_t cmd, uint8_t ch, uint8_t *data);
ScreenID_t channel_screen_func(Screen_param_t param);
ScreenID_t set_changetime_func(Screen_param_t param);
//ScreenID_t set_triggermode_func(Screen_param_t param);
//ScreenID_t operation_screen_func(Screen_param_t param);
ScreenID_t blink_text_screen_func(Screen_param_t param);
ScreenID_t uart_screen_func(Screen_param_t param);
ScreenID_t ip_screen_func(Screen_param_t param);
ScreenID_t port_screen_func(Screen_param_t param);
//ScreenID_t self_test_screen_func(Screen_param_t param);
ScreenID_t remote_screen_func(Screen_param_t param);
ScreenID_t user_screen_func(Screen_param_t param);
ScreenID_t factory_screen_func(Screen_param_t param);
ScreenID_t select_ch_screen_func(Screen_param_t param);
ScreenID_t set_level_screen_func(Screen_param_t param);

#if 0
ScreenID_t version_screen_func(Screen_param_t param);
#endif
static struct _screen_func{
	ScreenID_t 		id;
	screen_func_t func;
}screen_func_list[]={
	{eCHANNEL_SC, channel_screen_func},
	{eSET_ON_TIME_SC,set_changetime_func},
	{eSET_DELAY_TIME_SC,set_changetime_func},
	{eSET_TRIGGER_SC, blink_text_screen_func},
#if	ENABLE_DIMMING_MODE
	{eSET_LEVEL_SC, set_level_screen_func},
#endif
	{eOUTMODE_SC, blink_text_screen_func},
#if	ENABLE_DIMMING_MODE
	{eOPMODE_SC, blink_text_screen_func},
#endif
	{eUARTMODE_SC, uart_screen_func},
	{eETH_IP_SC, ip_screen_func},
	{eETH_PORT_SC, port_screen_func},
//	{eTESTMODE_SC, self_test_screen_func},
	{eREMOTE_SC, remote_screen_func},
	{eUSERMODE_SC, user_screen_func},
	{eFACTORY_SC, factory_screen_func},
	{eCHSELECT_SC, select_ch_screen_func},
#if 0
	{eVERSION_SC, version_screen_func}
#endif
};
#define MAX_SC_LIST_NUM 13

static struct{
	uint8_t sync_ok;
	uint8_t set_mode;
	uint8_t remote_mode;
	uint8_t cur_ch;
	uint8_t sel_max_ch;
	int8_t cursor;
	int8_t input_num[5];
	uint8_t digit_num;
	uint16_t set_time;
	uint8_t enable_blink;
	uint8_t blink;
	uint8_t blink_row;
	char blink_text[2][LCD_PRINT_MAX];
	int8_t ipaddr[4][3];
	uint8_t ip_idx;
	uint8_t ip_digit;
	int8_t port[5];
}m_cfg={
	.sync_ok = 0,
	.set_mode = 0,
	.remote_mode = eREMOTE_NONE,
	.cur_ch = eCH1,
};

config_t load_cfg;
/* Private functions ---------------------------------------------------------*/
const char *backend_cmd_2_str(BackEnd_Cmd_t cmd)
{
	switch(cmd)
	{
		case_str(eCMD_ReadVersion)
		case_str(eCMD_OutMode)
		case_str(eCMD_Ontime)
		case_str(eCMD_Delaytime)
		case_str(eCMD_InputCh)
		case_str(eCMD_OutCh)
		case_str(eCMD_Trigger)
		case_str(eCMD_TriggerSet)
		case_str(eCMD_InputEdge)
		case_str(eCMD_Factory)
		case_str(eCMD_Status)
		case_str(eCMD_CloseRemote)
#if	ENABLE_DIMMING_MODE
		case_str(eCMD_SetOPMode)
		case_str(eCMD_SetDuty)
		case_str(eCMD_SetPeriod)
#endif
		case_str(eCMD_FR_SetRemote)
		case_str(eCMD_FR_SelfTest)
		case_str(eCMD_FR_SetDevID)
		case_str(eCMD_FR_SetIP)
		case_str(eCMD_FR_SetPort)
		default:
			break;
	}
	return "Unknown Command";
}

void screen_init(void)
{
	char line_buf[LCD_PRINT_MAX];

	memset(&m_cfg, 0, sizeof(m_cfg));
	memcpy(&load_cfg, m_app_config, sizeof(config_t));
	m_cfg.sel_max_ch = (load_cfg.ch_num+1)*2;
	trigger_set_channel(m_cfg.sel_max_ch);
	m_cfg.remote_mode = load_cfg.remote_mode;
	sprintf(line_buf,"%s\n", load_cfg.company);
	LOG_DBG("Model : %s Ch mode[%d]", line_buf, m_cfg.sel_max_ch);
	LCD_LOCATE(1,2);
	LCD_printstring(line_buf);
	LCD_LOCATE(2,1);
#if 0
	LCD_printstring("StrobeController\n");
#else
	LOG_DBG("Front:%d Main:%d",load_cfg.frontVer,load_cfg.backendVer);
	sprintf(line_buf, "F : %1d.%1d  M : %1d.%1d\n", load_cfg.frontVer/10, load_cfg.frontVer%10, load_cfg.backendVer/10, load_cfg.backendVer%10);
	LCD_printstring(line_buf);
#endif
}

static void send_cmd_2_backend(BackEnd_Cmd_t cmd, uint8_t ch, uint8_t *value)
{
	LOG_DBG("Send Cmd[%s]", backend_cmd_2_str(cmd));
	uart_SendFront(cmd, ch, value);
}

void screen_sync_complete(uint8_t MainVer, uint8_t FpgaVer)
{
	LOG_DBG("Receive Main Ver[%d] Fpga[%d]", MainVer, FpgaVer);
	load_cfg.backendVer = MainVer;
	m_cfg.sync_ok = 1;
}

void screen_send_config(void)
{
	int cnt = 0;
	uint8_t  out_ch = 0;
	uChtime set_time;
	uint8_t data[4] = {0,0,0,0};
	uPort_t uport;

	while(1){
		//LOG_DBG("Send Sync Data");
		send_cmd_2_backend(eCMD_ReadVersion, 0, 0);
		if(m_cfg.sync_ok){
			//LOG_DBG("Receive Sync Data");
			break;
		}
		osDelay(100);
	}
	data[0] = load_cfg.edge;
	send_cmd_2_backend(eCMD_InputEdge, cnt, data); // channel 1

#if	ENABLE_DIMMING_MODE
	LOG_DBG("Load Config Mode[%s]", load_cfg.op_mode == eSTROBE_MODE?"STROBE":"DIMMING");
	data[0] = load_cfg.op_mode;
	send_cmd_2_backend(eCMD_SetOPMode, m_cfg.sel_max_ch, data);

	set_time.u16_time = load_cfg.period_time;
	data[0] = set_time.array_time[1];
	data[1] = set_time.array_time[0];
	send_cmd_2_backend(eCMD_SetPeriod, 0, data);

	if(load_cfg.op_mode == eDIMMING_MODE){
		for(cnt = 0; cnt < m_cfg.sel_max_ch; cnt++){
			data[0] = load_cfg.duty_level[cnt];
			send_cmd_2_backend(eCMD_SetDuty, cnt, data);
			out_ch |= 1<<cnt;
		}
	}
#endif
	for(cnt = 0; cnt < m_cfg.sel_max_ch; cnt++){
		set_time.u16_time = load_cfg.on_time[cnt];
		data[0] = set_time.array_time[1];
		data[1] = set_time.array_time[0];
		send_cmd_2_backend(eCMD_Ontime, cnt, data);
		set_time.u16_time = load_cfg.delay_time[cnt];
		data[0] = set_time.array_time[1];
		data[1] = set_time.array_time[0];
		send_cmd_2_backend(eCMD_Delaytime, cnt, data);
		out_ch |= 1<<cnt;
	}

	memset(data,0,4);
	if(load_cfg.out_mode == eONE_ONE){
		data[3] = 0xFF;
	}else{
		data[3] = 0x01;
	}
	send_cmd_2_backend(eCMD_InputCh, 0, data);
	memset(data,0,4);
	data[3] = out_ch;
	send_cmd_2_backend(eCMD_OutCh, m_cfg.sel_max_ch, data);
	data[0] = load_cfg.out_mode;
	data[1] = 0x01;
	send_cmd_2_backend(eCMD_OutMode, cnt, data);

	memcpy(data, load_cfg.ether.ipaddr, 4);
	send_cmd_2_backend(eCMD_FR_SetIP, 0, data);

	uport.u16_port = load_cfg.ether.port;
	memcpy(data, uport.array_port, 2);
	send_cmd_2_backend(eCMD_FR_SetPort, 0, data);
}

void screen_blink(void)
{
	if(m_cfg.enable_blink){
		LCD_LOCATE(m_cfg.blink_row,1);
		m_cfg.blink ^= 1;
		//LOG_DBG("Blink%d: %s", m_cfg.blink, m_cfg.blink_text[m_cfg.blink]);
		LCD_printstring(m_cfg.blink_text[m_cfg.blink]);
	}
}

ScreenID_t screen_process(ScreenID_t screen_id, Key_t key)
{
	int menu_list_size = MAX_SC_LIST_NUM;
	int i;	
	Screen_param_t sc_param;

	LOG_INF("%s : key[%s] Set_mode[%d]", screen_id_2_str(screen_id), key_id_2_str(key), m_cfg.set_mode);
	sc_param.sc_id = screen_id;
	sc_param.key = key;
	for(i = 0; menu_list_size; i++){
		if(screen_id == screen_func_list[i].id){
			return screen_func_list[i].func(sc_param);			
		}
	}
	return eMAX_SCREEN_ID;
}

static int draw_title(uint8_t step)
{
	char line_1_buf[LCD_PRINT_MAX]={0,}, line_2_buf[LCD_PRINT_MAX]={0,};
	uint8_t ch = m_cfg.cur_ch;
	uint8_t disp_ch = m_cfg.cur_ch+1;

	LOG_INF("start Draw title [%d] ch[%d]\r\n", step, ch);

	switch(step){
		case eCH_CHSelect:
		case eCH_AllCH:
			if(step == eCH_CHSelect){
				sprintf(line_1_buf, "CH%d    Operation\n", disp_ch);
			}else{
				sprintf(line_1_buf, "ALL    Operation\n");
			}
			sprintf(line_2_buf, "OnTime : %5dus\n", load_cfg.on_time[ch]);
			break;
		case eCH_OnTIME:
		case eCH_AllOnTIME:
			if(step == eCH_OnTIME){
				sprintf(line_1_buf, "CH%d On-Time     \n", disp_ch);
			}else{
				sprintf(line_1_buf, "ALL  On-Time    \n");
			}
			sprintf(line_2_buf, "Value : %5d us\n", load_cfg.on_time[ch]);
			break;
		case eCH_DelayTIME:
		case eCH_AllDelayTIME:
			if(step == eCH_DelayTIME){
				sprintf(line_1_buf, "CH%d Delay-Time  \n", disp_ch);
			}else{
				sprintf(line_1_buf, "ALL  Delay-Time  \n");
			}
			sprintf(line_2_buf, "Value : %5d us\n", load_cfg.delay_time[ch]);
			break;
		case eCH_TriggerSet:
		case eCH_AllTriggerSet:
			if(step == eCH_TriggerSet){
				sprintf(line_1_buf, "CH%d Trigger Set \n", disp_ch);
			}else{
				sprintf(line_1_buf, "ALL  Trigger Set \n");
			}
			sprintf(line_2_buf, "Value : %s \n", load_cfg.edge?"Rising ":"Falling");
			memcpy(m_cfg.blink_text[0], line_2_buf, 17);
			sprintf(m_cfg.blink_text[1],"Value :         \n");
			break;
		case eSEL_OutMode:
			sprintf(line_1_buf, "Mode Select     \n");
			sprintf(line_2_buf, "      %s     \n", load_cfg.out_mode?"1 : N":"1 : 1");
			break;
		case eSET_OutMode:
			sprintf(line_1_buf, "Mode Selection  \n");
			sprintf(line_2_buf, "      %s     \n", load_cfg.out_mode?"1 : N":"1 : 1");
			break;
#if	ENABLE_DIMMING_MODE
		case eSET_OpMode:
			sprintf(line_1_buf, "Strobe Select   \n");
			sprintf(line_2_buf, "%s             \n", load_cfg.op_mode?"PWM":"One");
			break;
#if 0
		case eDraw_OpMode:
			sprintf(line_1_buf, "Strobe Select   \n");
			if(load_cfg.duty_level[ch] >= 100){
				sprintf(line_2_buf, "%s        100 %%\n", load_cfg.op_mode?"One":"PWM");
			}
			sprintf(line_2_buf, "%s         %2d %%\n", load_cfg.op_mode?"One":"PWM", load_cfg.duty_level[ch]);
			break;
#endif
		case eDuty_levle:
			sprintf(line_1_buf, "Level Set Mode  \n");
			sprintf(line_2_buf, "Value : %03d%%    \n", load_cfg.duty_level[ch]);
			break;
#endif
		case eRS_DevID:
			sprintf(line_1_buf, "RS232 Operation  \n");
			sprintf(line_2_buf, "Device ID : %02d  \n", load_cfg.dev_id);
			break;
		case eIP_Set:
			sprintf(line_1_buf, "Communication  \n");
			sprintf(line_2_buf, "%03d.%03d.%03d.%03d \n", load_cfg.ether.ipaddr[0], load_cfg.ether.ipaddr[1], load_cfg.ether.ipaddr[2], load_cfg.ether.ipaddr[3]);
			break;
		case ePORT_Set:
			sprintf(line_1_buf, "Communication  \n");
			sprintf(line_2_buf, "Port : %05d    \n", load_cfg.ether.port);
			break;
		case eRemote_Mode:
			sprintf(line_1_buf, "Communication   \n");
			if(m_cfg.remote_mode == eREMOTE_RS232){
				sprintf(line_2_buf, "RS232...        \n");
			}else{
				sprintf(line_2_buf, "Ethernet...     \n");
			}
			break;
//		case eSelf_Test:
//			sprintf(line_1_buf, "Test Operation  \n");
//			sprintf(line_2_buf, "Testing...      \n");
			break;
		case eUSER_Mode:
			sprintf(line_1_buf, " %s\n", load_cfg.company);
			sprintf(line_2_buf, "StrobeController\n");
			break;
		case eFactor_Mode:
			sprintf(line_1_buf, "Factory mode    \n");
			sprintf(line_2_buf, "Reset...        \n");
			break;
		case eSEL_OUT_Ch:
			sprintf(line_1_buf, "CH2 CH4 CH6 CH8 \n");
			sprintf(line_2_buf, "                \n");
			break;
		case eVer_Disp:
			sprintf(line_1_buf, "F - FW : Ver %1d.%1d\n", load_cfg.frontVer/10, load_cfg.frontVer%10);
			sprintf(line_2_buf, "M - FW : Ver %1d.%1d\n", load_cfg.backendVer/10, load_cfg.backendVer%10);
			break;
		default:
			LOG_ERR("Step Error!!![%d]", step);
			return 1;
	}
	LOG_DBG("Line 1 [%s]", line_1_buf);
	LOG_DBG("Line 2 [%s]", line_2_buf);
	LCD_LOCATE(1,1);
	LCD_printstring(line_1_buf);
	LCD_LOCATE(2,1);
	LCD_printstring(line_2_buf);
	//LOG_INF("End Draw title\r\n");

	return 0;
}

static void split_num(uint16_t value, int8_t *array)
{
	int num = value, i;

	//LOG_DBG("Value : %d", value);
	if(value == 0){
		memset(array, 0, m_cfg.digit_num+1);
		return ;
	}
	for(i = m_cfg.digit_num; i >= 0; i--){
		array[i] = -1;  // init value
		if(num){
			array[i] = num%10;
			//LOG_DBG("Split num[%d] => %d", i, array[i]);
			num/=10;
		}
	}
}

static int combine_array(int8_t *array)
{
	int i;
	int value = 0;
	
	for(i = 0; i <= m_cfg.digit_num; i++){
		//LOG_DBG("%d:%d", i, array[i]);
		if(array[i] >= 0){
			value = (value*10)+array[i];
			//LOG_DBG("Value[%d]", value);
		}
	}
	
	return value;
}


ScreenID_t channel_screen_func(Screen_param_t param)
{
	static uint8_t Info_Ch = 0xFF, Info_idx = 0;
	uint8_t Info_max = 3;
#if	ENABLE_DIMMING_MODE
	uint8_t Info_list_dimming[]={eCH_OnTIME, eCH_DelayTIME, eCH_TriggerSet, eDuty_levle};
#endif
	uint8_t Info_list_strobe[]={eCH_OnTIME, eCH_DelayTIME, eCH_TriggerSet};
	uint8_t *Info_list = Info_list_strobe;
	uint8_t display_max = m_cfg.sel_max_ch +1;
	int error;
//	uint8_t sel_ch_list[][9]={  {eCH1, eCH2, eCHALL},
//								{eCH1, eCH2, eCH3, eCH4, eCHALL},
//								{eCH1, eCH2, eCH3, eCH4, eCH5, eCH6, eCHALL},
//								{eCH1, eCH2, eCH3, eCH4, eCH5, eCH6, eCH7, eCH8, eCHALL}};
#if	ENABLE_DIMMING_MODE
	if(load_cfg.op_mode == eDIMMING_MODE){
		Info_list = Info_list_dimming;
		Info_max = 4;
	}
#endif
	m_cfg.cursor = 0;
	switch(param.key){
		case eKey_Idle:
		case eKey_ExitRemote:
			if(m_cfg.cur_ch < eCH1 || m_cfg.cur_ch > eCHALL){
				m_cfg.cur_ch = eCH1;
			}
			break;
		case eKey_Up:
			if(m_cfg.cur_ch == eCHALL){
				m_cfg.cur_ch = eCH1;
			}else{
				m_cfg.cur_ch = (m_cfg.cur_ch+1) % display_max;
			}
			if(m_cfg.cur_ch == m_cfg.sel_max_ch){
				m_cfg.cur_ch = eCHALL;
			}
			break;
		case eKey_Down:
			if(m_cfg.cur_ch == eCHALL){
				m_cfg.cur_ch = m_cfg.sel_max_ch - 1;
			}else{
				m_cfg.cur_ch = (m_cfg.cur_ch-1 + display_max) % display_max;
			}
			if(m_cfg.cur_ch == m_cfg.sel_max_ch){
				m_cfg.cur_ch = eCHALL;
			}
			break;
		case eKey_Enter:
			if(Info_Ch != m_cfg.cur_ch){
				Info_idx = 0;
				Info_Ch = m_cfg.cur_ch;
			}

			if(m_cfg.cur_ch == eCHALL){
				if(load_cfg.op_mode == eDIMMING_MODE){
					if(Info_list[Info_idx] !=  eDuty_levle){
						error = draw_title(Info_list[Info_idx]+1);
					}else{
						error = draw_title(Info_list[Info_idx]);
					}
				}else if(Info_idx < Info_max){
					error = draw_title(Info_list[Info_idx]+1);
				}
			}else{
				error = draw_title(Info_list[Info_idx]);
			}

			if(error){
				LOG_ERR("draw title error [%s][%d] cur_ch[%d] info idx[%d] info max[%d]", __func__, __LINE__, m_cfg.cur_ch, Info_idx, Info_max);
			}
			Info_idx++;
			if(Info_idx >= Info_max){
				Info_idx = 0;
			}
			return eMAX_SCREEN_ID;
		case eKey_SetMode:
			if(m_cfg.cur_ch == eCHALL){
				error = draw_title(eCH_OnTIME+1);
			}else{
				error = draw_title(eCH_OnTIME);
			}
			if(error){
				LOG_ERR("draw title error [%s][%d]", __func__, __LINE__);
			}
			m_cfg.set_time = load_cfg.on_time[m_cfg.cur_ch];
			m_cfg.cursor = 13;
			m_cfg.digit_num = 4;  // 0 ~ 4
			//m_cfg.set_mode =1;
			//LOG_DBG("Delay Set Ch[%d] time[%d]", m_cfg.cur_ch, m_cfg.set_time);
			split_num(m_cfg.set_time, m_cfg.input_num);			
			LCD_LOCATE(2, m_cfg.cursor);

			LCD_DISP_CURSOR();
			return eSET_ON_TIME_SC;
		default:

			break;
	}

	if(m_cfg.cur_ch != eCHALL){
		error = draw_title(eCH_CHSelect);
	}else{
		error = draw_title(eCH_CHSelect+1);
	}
	if(error){
		LOG_ERR("draw title error [%s][%d]", __func__, __LINE__);
	}
	//LOG_INF("Return ch scree function\r\n");
	return eMAX_SCREEN_ID;
}

ScreenID_t set_changetime_func(Screen_param_t param)
{
	char /*line_1_buf[LCD_PRINT_MAX],*/ line_2_buf[LCD_PRINT_MAX];
	ScreenID_t ret_sc = param.sc_id;
	uint8_t old_cursor = m_cfg.cursor;
	uint8_t digit_idx = (m_cfg.cursor - 9);
	uint8_t change_value = 0, channel = m_cfg.cur_ch;
	BackEnd_Cmd_t cmd;
	uChtime set_time;
	uint8_t data[4]={0};
	int error;
	
	LOG_DBG("Cursor[%d] digit[%d] key[%s]", m_cfg.cursor, digit_idx, key_id_2_str(param.key));
	switch(param.key){
		case eKey_Up:
			if((m_cfg.input_num[0] == 5) && (digit_idx != 0)){
				return ret_sc;
			}
			if(digit_idx == 0){
				m_cfg.input_num[digit_idx]++;
				if(m_cfg.input_num[digit_idx] > 5){
					m_cfg.input_num[digit_idx] = 0;
				}
				memset(&m_cfg.input_num[1], 0, 4);
			}else{
				m_cfg.input_num[digit_idx]++;
				if(m_cfg.input_num[digit_idx] > 9){
					m_cfg.input_num[digit_idx] = 0;
				}
			}
			//LOG_HEX_DUMP(m_cfg.input_num, 5, "Up");
			change_value=1;
			break;
		case eKey_Down:
			if((m_cfg.input_num[0] == 5) && (digit_idx != 0)){
				return ret_sc;
			}
			if(digit_idx == 0){
				m_cfg.input_num[digit_idx]--;
				if(m_cfg.input_num[digit_idx] < 0){
					m_cfg.input_num[digit_idx] = 5;
				}
				memset(&m_cfg.input_num[1], 0, 4);
			}else{
				m_cfg.input_num[digit_idx]--;
				if(m_cfg.input_num[digit_idx] < 0){
					m_cfg.input_num[digit_idx] = 9;
				}
			}
			//LOG_HEX_DUMP(m_cfg.input_num, 5, "Down");
			change_value=1;
			break;
		case eKey_CursorUp:
			m_cfg.cursor--;
			if(m_cfg.cursor < 9){
				m_cfg.cursor = 13;
			}
			break;
		case eKey_CursorDown:
			m_cfg.cursor++;
			if(m_cfg.cursor > 13){
				m_cfg.cursor = 9;
			}		
			break;
		case eKey_Enter:
			if(param.sc_id == eSET_ON_TIME_SC){
				load_cfg.on_time[channel] = combine_array(m_cfg.input_num);
				cmd = eCMD_Ontime;
				set_time.u16_time = (uint32_t)load_cfg.on_time[channel];
			}else{
				load_cfg.delay_time[channel] = combine_array(m_cfg.input_num);
				cmd = eCMD_Delaytime;
				set_time.u16_time  = (uint32_t)load_cfg.delay_time[channel];
			}

			if(channel != eCHALL){
				data[0] = set_time.array_time[1];
				data[1] = set_time.array_time[0];
				send_cmd_2_backend(cmd, channel, data);
			}else{
				for(int i = 0; i < m_cfg.sel_max_ch;i++){
					if(cmd == eCMD_Ontime){
						load_cfg.on_time[i] = load_cfg.on_time[channel];
					}else{
						load_cfg.delay_time[i] = load_cfg.delay_time[channel];
					}
					data[0] = set_time.array_time[1];
					data[1] = set_time.array_time[0];
					send_cmd_2_backend(cmd, i, data);
				}
			}
			if(save_data() != eMAX_SCREEN_ID){
				ret_sc = eERROR_SC;
				return ret_sc;
			}
		case eKey_Mode:
			if(param.sc_id == eSET_ON_TIME_SC){
				LOG_DBG("Change delay screen");
				if(channel == eCHALL){
					error = draw_title(eCH_DelayTIME+1);
				}else{
					error = draw_title(eCH_DelayTIME);
				}
				if(error){
					LOG_ERR("draw title error [%s][%d]", __func__, __LINE__);
				}
				ret_sc = eSET_DELAY_TIME_SC;
				m_cfg.set_time = load_cfg.delay_time[channel];
				m_cfg.cursor = 13;
				m_cfg.digit_num = 4;  // 0 ~ 4
				//LOG_DBG("Delay Set Ch[%d] time[%d]", channel, m_cfg.set_time);
				split_num(m_cfg.set_time, m_cfg.input_num);
				//LOG_HEX_DUMP(m_cfg.input_num, m_cfg.digit_num, "Set delay");
				LCD_LOCATE(2, m_cfg.cursor);
				change_value = 1;
			}else if(param.sc_id ==eSET_DELAY_TIME_SC){
				if(channel == eCHALL){
					error = draw_title(eCH_TriggerSet+1);
				}else{
					error = draw_title(eCH_TriggerSet);
				}
				ret_sc = eSET_TRIGGER_SC;
				LCD_DISP_ON(); // Cursor off
			}
			if(error){
				LOG_ERR("draw title error [%s][%d]", __func__, __LINE__);
			}
//			else if((param.sc_id == eSET_TRIGGER_SC) && (load_cfg.op_mode == eDIMMING_MODE)){ // Next SC => set level
//				draw_title(eDuty_levle);
//				ret_sc = eSET_LEVEL_SC;
//				LCD_DISP_CURSOR();
//			}
			break;
	}

	if(change_value){
		//LOG_DBG("Change Value Ch[%d] time[%d]", channel+1, combine_array(m_cfg.input_num));
//		if(channel == eCHALL){
//			if(ret_sc == eSET_DELAY_TIME_SC){
//				sprintf(line_1_buf, "ALL  Delay-Time  \n");
//			}
//		}else{
//			if(param.sc_id == eSET_ON_TIME_SC){
//				sprintf(line_1_buf, "CH%d On-Time     \n", channel+1);
//			}else{
//				sprintf(line_1_buf, "CH%d Delay-Time  \n", channel+1);
//			}
//		}
//		LCD_LOCATE(1,1);
//		LCD_printstring(line_1_buf);

		sprintf(line_2_buf, "Value : %05d us\n", combine_array(m_cfg.input_num));
		LCD_LOCATE(2,1);
		LCD_printstring(line_2_buf);
		LCD_LOCATE(2,m_cfg.cursor);
	}
	if(m_cfg.cursor != old_cursor){
		LCD_LOCATE(2, m_cfg.cursor);
	}
	return ret_sc;
}

static ScreenID_t save_data(void)
{
	memcpy(m_app_config, &load_cfg, sizeof(config_t));
	if(app_save_config() < 0){
		return eERROR_SC;
	}else{
		//draw_title(eCH_CHSelect);
		return eMAX_SCREEN_ID;
	}
}

static void run_remote_mode(void)
{
	uint8_t data[4] = {0};
	apps_set_remote_mode();

	data[0] = m_cfg.remote_mode;
	send_cmd_2_backend(eCMD_FR_SetRemote, 0, data);
}

ScreenID_t blink_text_screen_func(Screen_param_t param)
{
	ScreenID_t ret_sc = param.sc_id;
	uint8_t old_value, data[4]={0};
	static BackEnd_Cmd_t cmd = 0;
	int error;
#if	ENABLE_DIMMING_MODE
	uChtime set_time;
#endif
	
	if(ret_sc == eSET_TRIGGER_SC){
		old_value = load_cfg.edge;
	}else if(ret_sc == eOUTMODE_SC){
		old_value = load_cfg.out_mode;
	}
#if	ENABLE_DIMMING_MODE
	else if(ret_sc == eOPMODE_SC){
		old_value = load_cfg.op_mode;
	}
#endif
	LOG_DBG("Text SC Ret SC[%s] value[%d", screen_id_2_str(ret_sc), old_value);
	switch(param.key){
		case eKey_Up:
		case eKey_Down:
			if(m_cfg.set_mode == 0){
				return ret_sc;
			}
			old_value ^= 1;
			if(ret_sc == eSET_TRIGGER_SC){
				sprintf(m_cfg.blink_text[0], "Value : %s \n", (old_value==eFALLING)?"Falling":"Rising ");
				load_cfg.edge = old_value;
				cmd = eCMD_InputEdge;
			}else if(ret_sc == eOUTMODE_SC){
				sprintf(m_cfg.blink_text[0], "      %s     \n", (old_value==eONE_ONE)?"1 : 1" : "1 : N");
				load_cfg.out_mode = old_value;
				cmd = eCMD_OutMode;
			}
#if	ENABLE_DIMMING_MODE
			else if(ret_sc == eOPMODE_SC){
				sprintf(m_cfg.blink_text[0], "%s pulse       \n", (old_value==eSTROBE_MODE)?"One" : "PWM");
				load_cfg.op_mode = old_value;
				cmd = eCMD_SetOPMode;
			}
#endif
			LOG_DBG("Change Value old[%d] %s", old_value, m_cfg.blink_text[0]);
			LCD_LOCATE(m_cfg.blink_row,1);
			LCD_printstring(m_cfg.blink_text[0]);
			break;			
		case eKey_Enter:
			if(m_cfg.set_mode){
				m_cfg.enable_blink = 0;
				m_cfg.set_mode = 0;
				apps_set_blink_enable(m_cfg.enable_blink);
				if((ret_sc == eSET_TRIGGER_SC) && (m_cfg.cur_ch == eCHALL)){
					for(int i =0; i < m_cfg.sel_max_ch;i++){
						load_cfg.edge = load_cfg.edge;
					}
				}
				ret_sc = save_data();
				if(ret_sc != eMAX_SCREEN_ID){
					return eERROR_SC;
				}
#if 0		// 20250822 Dimming 모드 변경시 출력이 나가지 않음 -> period, duty 셋팅후 모드 변경해야 한다고 함.
				LOG_INF("Set Mode cmd[%d][%d]", cmd, old_value);
				data[0] = old_value;
				data[1] = 0x01; // 1:N Ext Mode..
 				send_cmd_2_backend(cmd, m_cfg.sel_max_ch, data);
#endif
 				if(cmd == eCMD_OutMode){
 					if(old_value == eONE_ONE){
						data[3] = 0xFF;
					}else{
						data[3] = 0x01;
					}
					send_cmd_2_backend(eCMD_InputCh, m_cfg.sel_max_ch, data);
 				}
#if	ENABLE_DIMMING_MODE
 				else if(cmd == eCMD_SetOPMode){
 					if(load_cfg.op_mode == eDIMMING_MODE){

 						set_time.u16_time = load_cfg.period_time;
 						data[0] = set_time.array_time[1];
 						data[1] = set_time.array_time[0];
 						send_cmd_2_backend(eCMD_SetPeriod, 0, data);

 						for(int cnt = 0; cnt < m_cfg.sel_max_ch; cnt++){
							data[0] = load_cfg.duty_level[cnt];
							send_cmd_2_backend(eCMD_SetDuty, cnt, data);
 						}

 						data[0] = load_cfg.edge;
 						data[1] = 0x01; // 1:N Ext Mode..
 						send_cmd_2_backend(eCMD_InputEdge, m_cfg.sel_max_ch, data); // strobe -> dimming 변환시 출력 안나가는 문제..
 					}
 				}
				if(param.sc_id == eSET_TRIGGER_SC){
					if(load_cfg.op_mode == eSTROBE_MODE){
						ret_sc = eCHANNEL_SC; // channel setting completed...
						m_cfg.cur_ch = eCH1;  // Default Channel 1
						error = draw_title(eCH_CHSelect);
					}else if(load_cfg.op_mode == eDIMMING_MODE){
						ret_sc = eSET_LEVEL_SC;
						error = draw_title(eDuty_levle);
					}
					if(error){
						LOG_ERR("draw title error [%s][%d]", __func__, __LINE__);
					}
				}else{
					ret_sc = eCHANNEL_SC; // channel setting completed...
					m_cfg.cur_ch = eCH1;  // Default Channel 1
					error = draw_title(eCH_CHSelect);
					if(error){
						LOG_ERR("draw title error [%s][%d]", __func__, __LINE__);
					}
				}

#else
				ret_sc = eMAX_SCREEN_ID; // channel setting completed...
				m_cfg.cur_ch = eCH1;  // Default Channel 1
				draw_title(eCH_CHSelect);
#endif
				LOG_INF("Set Mode cmd[%d][%d]", cmd, old_value);
				data[0] = old_value;
				data[1] = 0x01; // 1:N Ext Mode..
 				send_cmd_2_backend(cmd, m_cfg.sel_max_ch, data);

				LOG_DBG("Return SC[%s]", screen_id_2_str(ret_sc));
			}
			break;
		case eKey_SetMode:
		case eKey_Idle:
			if(param.key == eKey_SetMode){
				if(ret_sc == eOUTMODE_SC){
					sprintf(m_cfg.blink_text[0], "      %s     \n", (old_value==eONE_N)?"1 : N" : "1 : 1");
					sprintf(m_cfg.blink_text[1], "                \n");
					error = draw_title(eSET_OutMode);
					cmd = eCMD_OutMode;
					if(error){
						LOG_ERR("draw title error [%s][%d]", __func__, __LINE__);
					}
				}
#if	ENABLE_DIMMING_MODE
				else if(ret_sc == eOPMODE_SC){
					sprintf(m_cfg.blink_text[0], "%s Pulse       \n", (old_value==eSTROBE_MODE)?"One":"PWM");
					sprintf(m_cfg.blink_text[1], "                \n");
					cmd = eCMD_SetOPMode;
				}
#endif
			}else if(param.key == eKey_Idle){
				if(ret_sc == eSET_TRIGGER_SC){
					sprintf(m_cfg.blink_text[0], "Value : %s \n", (old_value==eRISING)?"Rising ":"Falling");
					sprintf(m_cfg.blink_text[1], "Value :         \n");
					cmd = eCMD_InputEdge;
				}
			}else{
				LOG_DBG("Unknown..");
			}
			m_cfg.set_mode = 1;
			m_cfg.blink = 0;
			m_cfg.blink_row = 2;
			m_cfg.enable_blink = 1;

			apps_set_blink_enable(m_cfg.enable_blink);
			break;
		case eKey_Mode:
			if(ret_sc == eSET_TRIGGER_SC){
				if(m_cfg.cur_ch == eCHALL){
					error = draw_title(eCH_TriggerSet+1);
				}else{
					error = draw_title(eCH_TriggerSet);
				}
				if(error){
					LOG_ERR("draw title error [%s][%d]", __func__, __LINE__);
				}
			}else if(ret_sc == eOUTMODE_SC){
				error = draw_title(eSEL_OutMode);
				if(error){
					LOG_ERR("draw title error [%s][%d]", __func__, __LINE__);
				}
			}
#if	ENABLE_DIMMING_MODE
			else if(ret_sc == eOPMODE_SC){
				error = draw_title(eSET_OpMode);
				if(error){
					LOG_ERR("draw title error [%s][%d]", __func__, __LINE__);
				}
			}
#endif
			break;
		default:
			break;
	}	

	return ret_sc;
}
#if	ENABLE_DIMMING_MODE
ScreenID_t set_level_screen_func(Screen_param_t param)
{
	ScreenID_t ret_sc = param.sc_id;
	uint8_t old_value = load_cfg.duty_level[m_cfg.cur_ch], data[4]={0};
	char line_buf[LCD_PRINT_MAX];
	uint8_t channel = m_cfg.cur_ch;
	int error;

	LOG_DBG("Cur CH[%d] Duty Level:%d", channel, old_value);
	switch(param.key){
		case eKey_Up:
			if(load_cfg.duty_level[channel]++ >= 100){
				load_cfg.duty_level[channel] = 0;
			}
			break;
		case eKey_Down:
			if(load_cfg.duty_level[channel]-- <= 0){
				load_cfg.duty_level[channel] = 100;
			}
			break;
		case eKey_Enter:
			if(m_cfg.set_mode){
				LCD_DISP_ON();	// Cursor off
				m_cfg.set_mode = 0;
				if(channel != eCHALL){
					data[0] = load_cfg.duty_level[channel];
					send_cmd_2_backend(eCMD_SetDuty, channel, data);
				}else{
					LOG_DBG("All Channel DutyCh[%d][%d]", channel, load_cfg.duty_level[channel])
					for(int i = 0; i < m_cfg.sel_max_ch;i++){
						load_cfg.duty_level[i] = load_cfg.duty_level[channel];
						data[0] = load_cfg.duty_level[channel];
						send_cmd_2_backend(eCMD_SetDuty, i, data);
					}
				}
				ret_sc = save_data();
				if(ret_sc == eERROR_SC){
					return ret_sc;
				}
			}
			ret_sc = eCHANNEL_SC; // channel setting completed...
			error = draw_title(eCH_CHSelect);
			if(error){
				LOG_ERR("draw title error [%s][%d]", __func__, __LINE__);
			}
			break;
		case eKey_Idle:
			m_cfg.set_mode = 1;
			m_cfg.cursor = 11;
			LCD_LOCATE(2, m_cfg.cursor);
			LCD_DISP_CURSOR();  // Cursor on
			break;
		case eKey_Mode:

			break;
		default:
			break;
	}
	if(old_value != load_cfg.duty_level[channel]){
		//draw_title(eDuty_levle);
		sprintf(line_buf, "Value : %03d    %%\n", load_cfg.duty_level[channel]);
		LCD_LOCATE(2,1);
		LCD_printstring(line_buf);
		data[0] = load_cfg.duty_level[channel];
		if(channel != eCHALL){
			send_cmd_2_backend(eCMD_SetDuty, channel, data);
		}else{
			// Todo : All Channel
		}
	}
	return ret_sc;
}
#endif
ScreenID_t uart_screen_func(Screen_param_t param)
{
	ScreenID_t ret_sc = param.sc_id;
	uint8_t old_value = load_cfg.dev_id, data[4]={0};
	int error;

	switch(param.key){
		case eKey_Up:
			if(load_cfg.dev_id++ >= MAX_DEV_ID){
				load_cfg.dev_id = 1;
			}			
			break;
		case eKey_Down:
			if(load_cfg.dev_id-- <= 1){
				load_cfg.dev_id = MAX_DEV_ID;
			}			
			break;
		case eKey_Enter:
			m_cfg.remote_mode = eREMOTE_RS232;
			if(m_cfg.set_mode){
				LCD_DISP_ON();	// Cursor off
				m_cfg.set_mode = 0;

				data[0] = m_cfg.remote_mode;
				send_cmd_2_backend(eCMD_FR_SetDevID, 0, data);
			}
			load_cfg.remote_mode = eREMOTE_RS232;
			ret_sc = save_data();
			if(ret_sc == eERROR_SC){
				return ret_sc;
			}
			ret_sc = eREMOTE_SC;
			run_remote_mode();
			break;			
		case eKey_SetMode:
			m_cfg.set_mode = 1;
			m_cfg.cursor = 14;
			error = draw_title(eRS_DevID);
			if(error){
				LOG_ERR("draw title error[%d]", __LINE__);
			}
			LCD_LOCATE(2, m_cfg.cursor);
			LCD_DISP_CURSOR();  // Cursor on		
			break;
		case eKey_Mode:
			if(m_cfg.set_mode == 0){
				error = draw_title(eRS_DevID);
				if(error){
					LOG_ERR("draw title error[%d]", __LINE__);
				}
			}
			break;
		default:
			break;
	}
	if(old_value != load_cfg.dev_id){
		error = draw_title(eRS_DevID);
		if(error){
			LOG_ERR("draw title error %d", __LINE__);
		}
	}
	return ret_sc;
}

static void check_octet(uint8_t *check_value, uint8_t isUp)
{
	uint16_t addr = combine_array((int8_t *)check_value);
	uint8_t cur_digit = m_cfg.ip_digit;

	LOG_DBG("isup[%d] Addr[%x] cur_dig[%d]", isUp, addr, cur_digit);

	if(addr > 255){
		if(cur_digit == 0){
			check_value[0] = 0;
		}else{
			if(isUp){
				check_value[cur_digit] = 0;
			}else{ // 29x or 259
				if((cur_digit == 1) &&(check_value[2] > 5)){
					check_value[cur_digit] = 4;
				}else{
					check_value[cur_digit] = 5;
				}
			}
		}
	}	
}

ScreenID_t ip_screen_func(Screen_param_t param)
{
	ScreenID_t ret_sc = param.sc_id;	
	uint8_t temp_addr[3]={0};
	char line_buf[LCD_PRINT_MAX];
	int i;
	uint8_t is_up = 0;
	uint8_t old_cursor = m_cfg.cursor;
	
	//LOG_DBG("Enter cursor[%d] ip_idx[%d] digit[%d]", m_cfg.cursor, m_cfg.ip_idx, m_cfg.ip_digit);
	switch(param.key){
		case eKey_Up:
		case eKey_Down:
			if(m_cfg.set_mode == 0){
				return ret_sc;
			}
			//LOG_INF("ip_idx[%d]",m_cfg.ip_idx);
			//memcpy(temp_addr, m_cfg.ipaddr[m_cfg.ip_idx],3);
			for(i = 0; i < 3; i++){
				if(m_cfg.ipaddr[m_cfg.ip_idx][i] < 0){  //If init value is less than 0, it must be set to 0
					temp_addr[i] = 0;
				}else{
					temp_addr[i] = m_cfg.ipaddr[m_cfg.ip_idx][i];
				}
				//LOG_INF("[%d]:Temp[%d] ipAddr[%d]", i, temp_addr[i], m_cfg.ipaddr[m_cfg.ip_idx][i]);
			}

			if(param.key == eKey_Up){
				if(temp_addr[m_cfg.ip_digit] == 9){
					temp_addr[m_cfg.ip_digit] = 0;
				}else{
					temp_addr[m_cfg.ip_digit]++;
				}
				is_up = 1;
			}else{
				if(temp_addr[m_cfg.ip_digit] == 0){
					temp_addr[m_cfg.ip_digit] = 9;
				}else{
					temp_addr[m_cfg.ip_digit]--;
				}
			}

			check_octet(temp_addr, is_up);
			//LOG_INF("digit[%d] Temp [%d][%d][%d]", m_cfg.ip_digit, temp_addr[0], temp_addr[1], temp_addr[2])
			memcpy(m_cfg.ipaddr[m_cfg.ip_idx], temp_addr, 3);
			sprintf(line_buf, "%03d.%03d.%03d.%03d \n",	(uint8_t)combine_array(m_cfg.ipaddr[0]),
														(uint8_t)combine_array(m_cfg.ipaddr[1]),
														(uint8_t)combine_array(m_cfg.ipaddr[2]),
														(uint8_t)combine_array(m_cfg.ipaddr[3]));
			//LOG_INF("IP : %s", line_buf);
			LCD_LOCATE(2,1);
			LCD_printstring(line_buf);
			LCD_LOCATE(2,m_cfg.cursor);
			break;
		case eKey_CursorUp:
			if(m_cfg.set_mode == 0){
				return ret_sc;
			}
			m_cfg.cursor--;
			if(m_cfg.cursor<=0){
				m_cfg.cursor = 15;
				m_cfg.ip_idx = 3;
				m_cfg.ip_digit=2;
				break;
			}

			m_cfg.ip_digit--;

			if(m_cfg.cursor%4 == 0){
				m_cfg.cursor--;
				m_cfg.ip_idx--;
				m_cfg.ip_digit=2;
			}
			break;
		case eKey_CursorDown:
			if(m_cfg.set_mode == 0){
				return ret_sc;
			}
			m_cfg.cursor++;
			if(m_cfg.cursor>15){
				m_cfg.cursor = 1;
				m_cfg.ip_idx = 0;
				m_cfg.ip_digit =0;
				break;
			}

			m_cfg.ip_digit++;

			if(m_cfg.cursor%4 == 0){
				m_cfg.cursor++;
				m_cfg.ip_idx++;
				m_cfg.ip_digit = 0;
			}
			break;
		case eKey_Enter:
			if(m_cfg.set_mode){
				LCD_DISP_ON();	// Cursor off
				for(i = 0; i < 4; i++){
					load_cfg.ether.ipaddr[i] = combine_array(m_cfg.ipaddr[i]);
				}
				ret_sc = save_data();
				m_cfg.set_mode = 0;
				if(ret_sc == eERROR_SC){
					return ret_sc;
				}
				send_cmd_2_backend(eCMD_FR_SetIP, 0, load_cfg.ether.ipaddr);
				ret_sc = eETH_PORT_SC;
			}else{
				//push_event0(EVT_Remote_Ether);
				load_cfg.remote_mode = eREMOTE_ETHER;
				ret_sc = save_data();
				m_cfg.remote_mode = eREMOTE_ETHER;
				ret_sc = eREMOTE_SC;
				run_remote_mode();
			}
			break;			
		case eKey_SetMode:
			m_cfg.digit_num = 2;
			for(i = 0; i < 4; i++){
				split_num(load_cfg.ether.ipaddr[i], m_cfg.ipaddr[i]);
			}
			m_cfg.cursor = 15;
			m_cfg.ip_idx = 3;
			m_cfg.ip_digit = 2;
			m_cfg.set_mode = 1;
			LCD_LOCATE(2, m_cfg.cursor);
			LCD_DISP_CURSOR();  // Cursor on		
			draw_title(eIP_Set);			
			break;
		case eKey_Mode:
		case eKey_Idle:
			if(m_cfg.set_mode == 0){
				draw_title(eIP_Set);
			}
			break;
		default:
			break;
	}

	//LOG_DBG("Exit cursor[%d] ip_idx[%d] digit[%d]", m_cfg.cursor, m_cfg.ip_idx, m_cfg.ip_digit);
	if(m_cfg.cursor != old_cursor){
		LCD_LOCATE(2, m_cfg.cursor);
	}
	return ret_sc;
}

ScreenID_t port_screen_func(Screen_param_t param)
{
	ScreenID_t ret_sc = param.sc_id;
	uint8_t old_cursor = m_cfg.cursor;
	int8_t temp_port[5];
	char line_buf[LCD_PRINT_MAX];
	int	new_port, direction = 1;
	uint8_t send_data[4]={0,};
	uPort_t uport;
	
	switch(param.key){
		case eKey_Up:
		case eKey_Down:
			if(m_cfg.set_mode == 0){
				return ret_sc;
			}
			memcpy(temp_port, m_cfg.port, 5);

			if(param.key == eKey_Up){
				temp_port[m_cfg.ip_digit] = (temp_port[m_cfg.ip_digit]+1)%10;
			}else{
				temp_port[m_cfg.ip_digit] = (temp_port[m_cfg.ip_digit]-1+10)%10;
				direction = -1;
			}

			new_port = combine_array(temp_port);
//			LOG_DBG("New port[%05d] digit[%d] Value[%d]", new_port, m_cfg.ip_digit, temp_port[m_cfg.ip_digit]);
			while((new_port < MIN_PORT)||(new_port > MAX_PORT)){
				temp_port[m_cfg.ip_digit] = temp_port[m_cfg.ip_digit] + direction;
				if(temp_port[m_cfg.ip_digit] > 9){
					temp_port[m_cfg.ip_digit] = 0;
				}else if(temp_port[m_cfg.ip_digit] < 0){
					temp_port[m_cfg.ip_digit] = 9;
				}
				new_port = combine_array(temp_port);
			}

			memcpy(m_cfg.port, temp_port, 5);
			sprintf(line_buf, "Port : %05d    \n", new_port);
			LCD_LOCATE(2,1);
			LCD_printstring(line_buf);
			LCD_LOCATE(2,m_cfg.cursor);
			break;
		case eKey_CursorUp:
			if(m_cfg.set_mode == 0){
				return ret_sc;
			}
			m_cfg.cursor--;
			if(m_cfg.cursor<8){
				m_cfg.cursor = 12;
				m_cfg.ip_digit=4;
			}else{
				m_cfg.ip_digit--;
			}
			break;
		case eKey_CursorDown:
			if(m_cfg.set_mode == 0){
				return ret_sc;
			}
			m_cfg.cursor++;
			if(m_cfg.cursor>12){
				m_cfg.cursor = 8;
				m_cfg.ip_digit =0;
			}else{
				m_cfg.ip_digit++;
			}
			break;
		case eKey_Enter:
			if(m_cfg.set_mode){
				LCD_DISP_ON();	// Cursor off
				load_cfg.ether.port = combine_array(m_cfg.port);

				uport.u16_port = load_cfg.ether.port;
				memcpy(send_data, uport.array_port, 2);
				send_cmd_2_backend(eCMD_FR_SetPort, 0, send_data);
			}
			load_cfg.remote_mode = eREMOTE_ETHER;
			m_cfg.set_mode = 0;
			ret_sc = save_data();
			if(ret_sc == eERROR_SC){
				return ret_sc;
			}
			m_cfg.remote_mode = eREMOTE_ETHER;
			ret_sc = eREMOTE_SC;
			run_remote_mode();
			break;
		case eKey_SetMode:
			m_cfg.set_mode = 1;
			m_cfg.cursor = 12;
			m_cfg.digit_num = 4;
			m_cfg.ip_digit = 4;
			split_num(load_cfg.ether.port, m_cfg.port);
			LCD_LOCATE(2, m_cfg.cursor);
			LCD_DISP_CURSOR();  // Cursor on		
			//draw_title(ePORT_Set);
			break;
		case eKey_Mode:
		case eKey_Idle:
			if(m_cfg.set_mode == 0){
				draw_title(ePORT_Set);
			}
			break;
	}
	
	if(m_cfg.cursor != old_cursor){
		LCD_LOCATE(2, m_cfg.cursor);
	}	
	return ret_sc;
}

//ScreenID_t self_test_screen_func(Screen_param_t param)
//{
//	ScreenID_t ret_sc = eMAX_SCREEN_ID;
//
//	switch(param.key){
//		case eKey_Mode:
//			draw_title(eSelf_Test);
//			break;
//		case eKey_Enter:
//			send_cmd_2_backend(eCMD_FR_SelfTest, 0, 0);
//			break;
//	}
//	return ret_sc;
//}

ScreenID_t remote_screen_func(Screen_param_t param)
{
	ScreenID_t ret_sc = param.sc_id;
	uint8_t data[4]={0};

	LOG_DBG("Remote Key[%s]",  key_id_2_str(param.key));
	switch(param.key){
		case eKey_ExitRemote:
			send_cmd_2_backend(eCMD_CloseRemote, 0, data);
			screen_send_config();
			load_cfg.remote_mode = eREMOTE_NONE;
			save_data();
			ret_sc = eCHANNEL_SC;
			break;
		case eKey_Mode:
		case eKey_Idle:
			draw_title(eRemote_Mode);
			break;
	}
	
	return ret_sc;
}

static int findCharIndex(char c) {
    for (int i = 0; i < CHARSET_LENGTH; i++) {
        if (charSet[i] == c) return i;
    }
    return 0;
}

static void increaseChar(char *ch) {
    int idx = findCharIndex(*ch);

    *ch = charSet[(idx + 1) % CHARSET_LENGTH];
}

static void decreaseChar(char *ch) {
    int idx = findCharIndex(*ch);
    *ch = charSet[(idx - 1 + CHARSET_LENGTH) % CHARSET_LENGTH];
}

ScreenID_t user_screen_func(Screen_param_t param)
{
	ScreenID_t ret_sc = param.sc_id;
	uint8_t old_cursor = m_cfg.cursor;
	char line_buf[LCD_PRINT_MAX];

	switch(param.key){
		case eKey_Up:
		case eKey_Down:
			if(eKey_Up){
				increaseChar((char *)&load_cfg.company[m_cfg.cursor]);
			}else{
				decreaseChar((char *)&load_cfg.company[m_cfg.cursor]);
			}
			sprintf(line_buf, "%s\n", load_cfg.company);
			LCD_LOCATE(1,1);
			LCD_printstring(line_buf);
			LCD_LOCATE(1,m_cfg.cursor+1);
			break;
		case eKey_CursorUp:
			m_cfg.cursor = (m_cfg.cursor - 1 + LINE_CHARACTER_MAX) % LINE_CHARACTER_MAX;
			break;
		case eKey_CursorDown:
			m_cfg.cursor = (m_cfg.cursor + 1 ) % LINE_CHARACTER_MAX;
			break;
		case eKey_Enter:
			LCD_DISP_ON();	// Cursor off
			m_cfg.set_mode = 0;
			ret_sc = save_data();
			if(ret_sc == eERROR_SC){
				return ret_sc;
			}
			break;
		case eKey_User:
			m_cfg.set_mode = 1;
			m_cfg.cursor = 2;
			LCD_LOCATE(1, m_cfg.cursor+1);
			LCD_DISP_CURSOR();  // Cursor on
			draw_title(eUSER_Mode);
			break;
	}
	LOG_DBG("Cursor new[%d] old[%d]", m_cfg.cursor, old_cursor);
	if(m_cfg.cursor != old_cursor){
		LCD_LOCATE(1, m_cfg.cursor+1);
	}
	return ret_sc;
}

ScreenID_t factory_screen_func(Screen_param_t param)
{
	ScreenID_t ret_sc = eMAX_SCREEN_ID;
	uint8_t data[4] ={0};
	LOG_DBG("Factory screen Key[%s]", key_id_2_str(param.key))
	switch(param.key){
		case eKey_Enter:
			send_cmd_2_backend(eCMD_Factory, 0, data);
			app_set_default_config(load_cfg.ch_num);
			memset(&m_cfg, 0, sizeof(m_cfg));
			memcpy(&load_cfg, m_app_config, sizeof(config_t));
			m_cfg.sel_max_ch = (load_cfg.ch_num+1)*2;
			trigger_set_channel(m_cfg.sel_max_ch);
			draw_title(eCH_CHSelect);
			break;
		case eKey_Factory:
			draw_title(eFactor_Mode);
			ret_sc = param.sc_id;
			break;
	}
	return ret_sc;
}

static void select_ch_set_blink(void)
{
	uint8_t ch_disp[4][LCD_PRINT_MAX] = {{"    CH4 CH6 CH8 \n"}, {"CH2     CH6 CH8 \n"},
										 {"CH2 CH4     CH8 \n"}, {"CH2 CH4 CH6     \n"}};

	sprintf(m_cfg.blink_text[0], "CH2 CH4 CH6 CH8 \n");
	sprintf(m_cfg.blink_text[1], "%s", ch_disp[m_cfg.cursor]);
}

ScreenID_t select_ch_screen_func(Screen_param_t param)
{
	ScreenID_t ret_sc = param.sc_id;
	uint8_t change_cursor = 0, out_ch = 0, data[4]={0,0,0,0};

	switch(param.key){
		case eKey_Up:
			m_cfg.cursor = (m_cfg.cursor - 1 + SEL_MAX_CH) % SEL_MAX_CH;
			change_cursor = 1;
			break;
		case eKey_Down:
			m_cfg.cursor = (m_cfg.cursor + 1 ) % SEL_MAX_CH;
			change_cursor = 1;
			break;
		case eKey_Enter:
			if(m_cfg.set_mode){
				m_cfg.enable_blink = 0;
				apps_set_blink_enable(m_cfg.enable_blink);
				load_cfg.ch_num = m_cfg.cursor ;
				m_cfg.sel_max_ch = (load_cfg.ch_num+1)*2;
				m_cfg.set_mode = 0;
				m_cfg.cur_ch = eCH1;
				trigger_set_channel(m_cfg.sel_max_ch);
				ret_sc = save_data();
				if(ret_sc == eERROR_SC){
					return ret_sc;
				}
				for(int i = 0; i < m_cfg.sel_max_ch;i++){
					out_ch |= 1<<i;
				}
				LOG_DBG("Set Out channel[%d][%x]",  m_cfg.sel_max_ch, out_ch);
				data[3] = out_ch;
				send_cmd_2_backend(eCMD_OutCh, m_cfg.sel_max_ch, data);
			}
			draw_title(eCH_CHSelect);
			ret_sc = eMAX_SCREEN_ID;
			break;
		case eKey_ChSel:
			m_cfg.cursor = load_cfg.ch_num;
			m_cfg.blink = 0;
			m_cfg.blink_row = 1;
			m_cfg.enable_blink = 1;
			m_cfg.set_mode = 1;
			apps_set_blink_enable(m_cfg.enable_blink);
			change_cursor = 1;
			draw_title(eSEL_OUT_Ch);
			break;
		default:
			LOG_DBG("Skip Key Action");
			break;
	}
	if(change_cursor){
		select_ch_set_blink();
	}

	return ret_sc;
}
#if 0
ScreenID_t version_screen_func(Screen_param_t param)
{
	ScreenID_t ret_sc = eMAX_SCREEN_ID;
	
	switch(param.key){
		case eKey_Enter:
			draw_title(eCH_CHSelect);
			break;
		case eKey_Version:
			draw_title(eKey_Version);
			break;
	}
	return ret_sc;
}
#endif
