/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "stdio.h"
#include "trigger.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define TRG1_Pin GPIO_PIN_15
#define TRG1_GPIO_Port GPIOA
#define TRG2_Pin GPIO_PIN_3
#define TRG2_GPIO_Port GPIOB
#define TRG3_Pin GPIO_PIN_4
#define TRG3_GPIO_Port GPIOB
#define TRG4_Pin GPIO_PIN_5
#define TRG4_GPIO_Port GPIOB
#define TRG5_Pin GPIO_PIN_6
#define TRG5_GPIO_Port GPIOB
#define TRG6_Pin GPIO_PIN_7
#define TRG6_GPIO_Port GPIOB
#define TRG7_Pin GPIO_PIN_8
#define TRG7_GPIO_Port GPIOB
#define TRG8_Pin GPIO_PIN_9
#define TRG8_GPIO_Port GPIOB

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static struct{
	GPIO_TypeDef 	*Port;
	uint16_t		pin;
}trigger[]={
		{TRG1_GPIO_Port, TRG1_Pin},
		{TRG2_GPIO_Port, TRG2_Pin},
		{TRG3_GPIO_Port, TRG3_Pin},
		{TRG4_GPIO_Port, TRG4_Pin},
		{TRG5_GPIO_Port, TRG5_Pin},
		{TRG6_GPIO_Port, TRG6_Pin},
		{TRG7_GPIO_Port, TRG7_Pin},
		{TRG8_GPIO_Port, TRG8_Pin}
};
osTimerId trigger_TimerHandle;
uint8_t trigger_watchCnt[8];
uint8_t trigger_status[8];
uint8_t trigger_channel = 0;
/* Private function prototypes -----------------------------------------------*/
void trigger_tmr_callback(void const *argument);
/* Private functions ---------------------------------------------------------*/

void trigger_init(void)
{
	osTimerDef(trigger_Timer, trigger_tmr_callback);
	trigger_TimerHandle = osTimerCreate(osTimer(trigger_Timer), osTimerPeriodic, NULL);
	if(trigger_TimerHandle == NULL){
		LOG_ERR("trigger timer Create Error");
		return ;
	}

	for(int cnt = 0; cnt < trigger_channel; cnt++){
		HAL_GPIO_WritePin(trigger[cnt].Port, trigger[cnt].pin, GPIO_PIN_SET);
	}

	osTimerStart(trigger_TimerHandle, 1000);
}

#define TRIGGER_IN_WATCH_TIMEOUT 2
void trigger_tmr_callback(void const *argument)
{
	for(int cnt = 0; cnt < trigger_channel; cnt++){
		if(trigger_status[cnt]){
			if(trigger_watchCnt[cnt]++ >= TRIGGER_IN_WATCH_TIMEOUT){
				trigger_watchCnt[cnt] = 0;
				HAL_GPIO_WritePin(trigger[cnt].Port, trigger[cnt].pin, GPIO_PIN_SET);
				trigger_status[cnt] = 0;
			}
		}
	}
}

void trigger_set_channel(uint8_t ch)
{
	uint8_t cnt;

	LOG_DBG("Trigger Set channel num[%d]", ch);
	for(cnt = 0; cnt < MAX_CHANNEL; cnt++){
		HAL_GPIO_WritePin(trigger[cnt].Port, trigger[cnt].pin, GPIO_PIN_SET);
		trigger_status[cnt] = 0;
	}
	trigger_channel = ch;
}

void trigger_set(uint8_t signal)
{
	int cnt;
	
	LOG_DBG("Trigger [%x] channel[%d]", signal, trigger_channel);
	if(signal == 0){
		for(cnt = 0; cnt < trigger_channel; cnt++){
			HAL_GPIO_WritePin(trigger[cnt].Port, trigger[cnt].pin, GPIO_PIN_SET);
			trigger_status[cnt] = 0;
		}
		return ;
	}
	
	for(cnt = 0; cnt < trigger_channel; cnt++){
		if((signal >> cnt) & 0x01){
			if(trigger_status[cnt] == 0){
				HAL_GPIO_WritePin(trigger[cnt].Port, trigger[cnt].pin, GPIO_PIN_RESET);
				trigger_status[cnt] = 1;
			}
			trigger_watchCnt[cnt] = 0;
		}
	}
}
