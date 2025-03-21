/*
 * m_normal.c
 *
 *  Created on: Feb 9, 2024
 *      Author: Robyn
 */
/* Includes ------------------------------------------------------------------*/
#include <m_serial.h>
#include "main.h"
#include "cmsis_os.h"

#include "m_normal.h"
#include "m_env.h"
#include "m_serial.h"
#include "m_remote.h"
#include "app_state.h"
#include "app_config.h"
/* Private typedef -----------------------------------------------------------*/
enum{
	eDEMOSTOP,
	eDEMOSTART,
};

typedef struct{
	uint32_t output;
	uint16_t on;
	uint16_t delay;
}Trigger_odder_t;

static struct{
	uint8_t demo_status;
	uint8_t demo_testdone;
}m_cfg={
	.demo_status = eDEMOSTOP,
	.demo_testdone = 0
};
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
osTimerId measureTimerHandle;
event_queue_observer_t menu_event;

osThreadId DemoTestTaskHandle;

static int g_Channels;
uint8_t g_RecvData = 0;
int g_FirmwareVersion = 0;

DEVICE Device;

uint8_t bEX_INPUT[MAX_PWM_CH] = {1,};
int bSerialOnOff[4] = {0, 0, 0, 0};
int nChangeFlag = 0;

osSemaphoreId demoSemaHandle;
/* Private function prototypes -----------------------------------------------*/

void on_measure(void const * argument)
{
	static int old_status = -1;

	g_Channels = m_dipsw_read_Channel();
	if(old_status != Device.nDeviceCurrentStatus){
		LOG_DBG("Status[%d] Channel[%d]", Device.nDeviceCurrentStatus, g_Channels);
		old_status = Device.nDeviceCurrentStatus;
	}
}

static void onPushTcpData(uint8_t *pData, int size)
{
	int i;

	for(i = 0; i < size; i++){
		remote_push_buf(eREMOTE_ETHER, pData[i]);
	}
}

static void onSaveConfg(void)
{
	int err_code;

	//LOG_DBG("Call Save Config");
	err_code = app_config_save();
	if(err_code == 0){
		memcpy(&m_app_state.config, &m_app_cfg->cfg, sizeof(fpga_config_t));
		memcpy(&m_app_state.model,  &m_app_cfg->model, sizeof(app_config_model_t));
	}
	REPORT_IF_ERROR(err_code);
}

static void onStartStrobe(uint8_t Mode)
{
	uint8_t out_mode = eSTROBE_MODE, option = 0;
	uint32_t input = 0, output = 0;
	uint16_t *pDelay, *pOn;
	int i;

	fpga_set_mode(out_mode);
	fpga_select_edge(m_app_state.config.trigger_edge);

	pDelay = m_app_state.config.delay_time;
	pOn = m_app_state.config.on_time;
	for(i = 0; i < MAX_CHANNEL; i++){
		fpga_set_delay_time(i, pDelay[i]);
		fpga_set_on_time(i, pOn[i]);
	}
	//LOG_DBG("input[%x] output[%x] mode[%x]", input, output, out_mode);

	fpga_set_input_channel(out_mode, input);
#if 1	// Mode 진입시 동작 안하게 하는 루틴 Enter mode screen stop  - > run 20241029
	if(Mode != eStrobe_Set){
		fpga_set_output_channel(out_mode, output);
	}else{
		fpga_set_output_channel(out_mode, 0x00000000);
	}
#else
	fpga_set_output_channel(out_mode, output);
#endif

	fpga_select_output(out_mode, option);

	if(Mode == eStrobe_Set)
		push_event1(EVT_fpga_load_done, eREMOTE_NONE);
}

static void onStartSelfTest(uint8_t isStart)
{
	LOG_DBG("%s Self Test", isStart?"Start":"Stop");

	m_cfg.demo_status = isStart;
	if(isStart == eDEMOSTART){
		fpga_select_output(eONE_N, 0x01);
		fpga_set_output_channel(0, 0x0);
		fpga_set_input_channel(0, 0xFFFFFFFF);
		for(int i = 0; i < MAX_CHANNEL; i++){
			fpga_set_delay_time(i, 0);
			fpga_set_on_time(i, 2000);
		}
		osSemaphoreRelease(demoSemaHandle);
	}else{
		while(m_cfg.demo_testdone == 0){
			osDelay(100);
		}
		onStartStrobe(1);
		push_event0(Evt_stop_demotest);
	}
}

static void onSetDeviceID(uint8_t devID)
{
	Device.nDeviceNo = devID;
}

static uint8_t check_test_done(void)
{
	int k;

	for(k = 0; k < 100; k++){
		if(m_cfg.demo_status == eDEMOSTART){
			osDelay(10);
		}else{
			break;
		}
	}
	return k >= 100 ? 0:1;
}

static void demo1st(void)
{
	LOG_DBG("Demo 1ST Start");
	do{
		for(int j = 0; j < MAX_CHANNEL; j++){
			fpga_set_output_channel(0, 1<<j);  // on
			if(check_test_done()){
				break;
			}
		}
	}while(0);
	LOG_DBG("Demo 1ST End");
}

static void demo2nd(uint8_t step, uint32_t out_channel)
{
	uint8_t shift_val;

	LOG_DBG("Demo 2nd Step [%d] Start", step);
	if((step>=1) && (step<=4)){
		shift_val = (step-1) * 8;
	}else if(step == 5 || step == 6){
		shift_val = (step-5) * 16;
	}
	LOG_DBG("Set 8 Channel [%x]", out_channel);
	out_channel <<= shift_val;
	fpga_set_output_channel(0, out_channel);  // on
	for(int i = 0; i< 3; i++){
		if(check_test_done()){
			break;
		}
	}
	LOG_DBG("Demo 2nd End");
}

static void demo3rd(void)
{
	fpga_set_output_channel(0, 0xFFFFFFFF);  // on
	for(int i = 0; i < 3; i++){
		if(check_test_done()){
			break;
		}
	}
}

static void runDemoTest(uint8_t step)
{
	switch(step){
		case 0:
			demo1st();
			break;
		case 1:
		case 2:
		case 3:
		case 4:
			demo2nd(step, 0x000000FF);
			break;
		case 5:
		case 6:
			demo2nd(step, 0x0000FFFF);
			break;
		case 7:
			demo3rd();
			break;
		default:
			break;
	}
}

static void DemoProcTask(void const * argument)
{
	uint8_t TestStep = 0;

	LOG_DBG("Start Demo Processor");
	while(1){
		if(osSemaphoreWait(demoSemaHandle, osWaitForever) == osOK){
			LOG_DBG("Start Demo Test!!!")
			while(m_cfg.demo_status == eDEMOSTART){
				LOG_DBG("Demo Step[%d]", TestStep);
				m_cfg.demo_testdone = 0;
				runDemoTest(TestStep);
				m_cfg.demo_testdone = 1;
				if(++TestStep > 7){
					TestStep = 0;
				}
				osDelay(100);
			}
			TestStep = 0;
		}
		osDelay(100);
	}
}

static void evt_handler(event_t const* evt, void* p_context)
{
	switch (evt->event)
	{
		case EVT_menu_start:
			osTimerStart(measureTimerHandle,200); // 200msec
			break;
		case EVT_received_tcp:
			onPushTcpData(evt->p_event_data, evt->event_data_size);
			break;
		case EVT_save_config:
			onSaveConfg();
			break;
		case EVT_cancel_config:
			// todo : roll back config & setting..
			break;
		case EVT_start_strobe:
			onStartStrobe(evt->param1);
			break;
		case EVT_self_test:
			onStartSelfTest(eDEMOSTART);
			break;
		case EVT_stop_self_test:
			onStartSelfTest(eDEMOSTOP);
			break;
		case EVT_Set_DevID:
			onSetDeviceID(evt->param1);
			break;
		default:
			break;
	}
}

int m_menu_normal_init(void)
{
	LOG_INF("Menu Init");

	osSemaphoreDef(demoSema);
	demoSemaHandle = osSemaphoreCreate(osSemaphore(demoSema), 1);

	osTimerDef(measure_timer, on_measure);
	measureTimerHandle = osTimerCreate(osTimer(measure_timer), osTimerPeriodic, NULL);
	if(measureTimerHandle==NULL){
		LOG_ERR("measureTimer Create Error!!");
		return 1;
	}

	LOG_DBG("Semaphore Handle[%d]", demoSemaHandle);
	if(demoSemaHandle != NULL){
		if(osSemaphoreWait(demoSemaHandle, 0) == osOK){
			LOG_DBG("Skip sema count!!");
		}else{
			LOG_DBG("Sema count 0!!!");
		}
	}

	osThreadDef(demoProc, DemoProcTask, osPriorityNormal, 0, 512);
	DemoTestTaskHandle = osThreadCreate(osThread(demoProc), NULL);
	if(DemoTestTaskHandle == NULL){
		LOG_ERR("Demo Task create error!!");
		return 1;
	}

	menu_event.handler = evt_handler;
	menu_event.p_context = NULL;
	if(m_event_register(&menu_event)){
		LOG_ERR("menu event registered error!!");
		return 1;
	}

	return 0;
}
