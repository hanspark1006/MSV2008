/* Includes ------------------------------------------------------------------*/
#include "cmsis_os.h"
#include "main.h"
#include "uart.h"
#include "trigger.h"
/* Private define ------------------------------------------------------------*/
#define MAX_QUEUE_SIZE	128
#define FRONT_UART		(&huart1)

#define CMD_HEAD		0xDF
#define CMD_TAIL		0xFEFF

#define RES_OK				0xD0
#define RES_ERROR			0xD1
#define TRIGGER_EVT_CMD		0x33
#define TRIGGER_DATA_IDX	2
/* Private typedef -----------------------------------------------------------*/
typedef struct _QUEUE{
	uint16_t _head;
	uint16_t _tail;
	uint8_t _data[MAX_QUEUE_SIZE];
}QUEUE;

typedef struct{
	uint8_t cmd_buffer[CMD_MAX_SIZE];
	uint16_t cmd_pos;
	uint32_t cmd_state;
}serial_cfg_t;

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
osThreadId serialTaskHandle;
static uint8_t Front_rx_buf[1];
static QUEUE	que={0,};
static serial_cfg_t config;
/* Private function prototypes -----------------------------------------------*/
//static void SerialTask(void const * argument);
/* Private functions ---------------------------------------------------------*/
static void push_buf(uint8_t ch)
{
	int pos = (que._head + 1) % MAX_QUEUE_SIZE;

	if(pos == que._tail) return ;

	que._data[que._head] = ch;
	que._head = pos;
}

static uint8_t pop_buf(uint8_t *ch)
{
	if(que._head == que._tail) return 0;

	*ch = que._data[que._tail];
	que._tail = (que._tail + 1)%MAX_QUEUE_SIZE;

	return 1;
}

static uint8_t find_cmd(void)
{
	uint8_t data = 0, len = 0;

	while(pop_buf(&data)){
		if((config.cmd_pos == 0)&&(data != CMD_HEAD)){
			continue;
		}
		if(config.cmd_pos < CMD_MAX_SIZE){
			config.cmd_buffer[config.cmd_pos++] = data;
		}
		config.cmd_state = ((config.cmd_state<<8)|data);
		if(config.cmd_state == CMD_TAIL){
			len = config.cmd_pos;
			config.cmd_state = 0;
			config.cmd_pos = 0;
		}
	}
	return len;
}

//static void SerialTask(void const * argument)
void uart_task(void)
{
	uint16_t cmd_size;
	while(1){
		cmd_size = find_cmd();
		if(cmd_size){
			if(config.cmd_buffer[1] == TRIGGER_EVT_CMD){
				trigger_set(config.cmd_buffer[TRIGGER_DATA_IDX]);
			}else if(config.cmd_buffer[1] == RES_ERROR){
				// todo : error 처리
			}
		}
		osDelay(10);
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	/* Console */
	if ( huart->Instance == USART1 ){
		push_buf(Front_rx_buf[0]);
		HAL_UART_Receive_IT(FRONT_UART, &Front_rx_buf[0], 1 );
	}
}

int uart_SendFront(uint8_t cmd, uint8_t ch, uint32_t data)
{
	int error;
	uint8_t send_data[CMD_MAX_SIZE]={0xEF, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0xFF};

	send_data[2] = cmd;
	send_data[3] = ch;
	send_data[4] = ((data & 0xFF00) >> 24);
	send_data[5] = ((data & 0xFF00) >> 16);
	send_data[6] = ((data & 0xFF00) >> 8);
	send_data[7] = (data & 0x00FF);

	error  = HAL_UART_Transmit(FRONT_UART, send_data, CMD_MAX_SIZE, 1000);
	if(error != HAL_OK){
		LOG_ERR("Send Command Error");
		return -EIO;
	}

	return 0;
}

int uart_init(void)
{	
	HAL_UART_Receive_IT(FRONT_UART, &Front_rx_buf[0], 1);

//	osThreadDef(serialTask, SerialTask, osPriorityNormal, 0, 256);
//	serialTaskHandle = osThreadCreate(osThread(serialTask), NULL);
//	if(serialTaskHandle == NULL){
//		LOG_ERR("Front Serial Task Create Error");
//		return -1;
//	}
	return 0;
}
