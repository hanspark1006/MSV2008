/* Includes ------------------------------------------------------------------*/
#include "main.h"
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
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void trigger_init(void)
{
	for(int cnt = 0; cnt < MAX_CHANNEL; cnt++){
		HAL_GPIO_WritePin(trigger[cnt].Port, trigger[cnt].pin, GPIO_PIN_SET);
	}
}

void trigger_set(uint8_t signal)
{
	int cnt;
	
	if(signal == 0){
		for(cnt = 0; cnt < MAX_CHANNEL; cnt++){
			HAL_GPIO_WritePin(trigger[cnt].Port, trigger[cnt].pin, GPIO_PIN_SET);
		}
		return ;
	}
	
	for(cnt = 0; cnt < MAX_CHANNEL; cnt++){
		if((signal >> cnt) & 0x01){
			HAL_GPIO_WritePin(trigger[cnt].Port, trigger[cnt].pin, GPIO_PIN_RESET);
		}
	}
}
