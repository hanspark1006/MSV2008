/* Includes ------------------------------------------------------------------*/
#include "cmsis_os.h"
#include "main.h"
#include "lcd.h"

/** @addtogroup STM8S_StdPeriph_Driver
  * @{
  */
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define CGRAM_address_start 0x40
#define LCD_Data_Port	GPIOB
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static char Custom[48] =
{
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Blank
	0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, // 1column  |
	0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, // 2columns ||
	0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, // 3columns |||
	0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, // 4columns ||||
	0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, // 5columns |||||
};
/* Private function prototypes -----------------------------------------------*/
void LCD_LOAD_CGRAM(char tab[], uint8_t charnum)
{
	uint8_t index;
	/* Each character contains 8 definition values*/
	charnum = (uint8_t)(charnum * 8);
	for (index = 0;index < charnum;index++)
	{
		/* Store values in LCD*/
		LCD_printchar(tab[index]);
		osDelay(1);
	}
}

void LCD_ENABLE (void)
{
	HAL_GPIO_WritePin(LCD_EN_GPIO_Port, LCD_EN_Pin, GPIO_PIN_SET);
	osDelay(2);
	HAL_GPIO_WritePin(LCD_EN_GPIO_Port, LCD_EN_Pin, GPIO_PIN_RESET);
}

static void Send4Bit(uint8_t data)
{
    HAL_GPIO_WritePin(LCD_Data_Port, LCD_D4_Pin, (data & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_Data_Port, LCD_D5_Pin, (data & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_Data_Port, LCD_D6_Pin, (data & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_Data_Port, LCD_D7_Pin, (data & 0x08) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    LCD_ENABLE();
}

void LCD_CMD(unsigned char cmd_data)
{
	HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_RESET);

	Send4Bit((cmd_data>>4)&0x0F);
	Send4Bit(cmd_data & 0x0F);

	osDelay(2);
}

void LCD_printchar(unsigned char ascode)
{
	HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_SET);

	Send4Bit((ascode>>4)&0x0F);
	Send4Bit(ascode & 0x0F);

	osDelay(2);
}

void LCD_printstring(char *text)
{
	//LOG_DBG("Print String[%s]", text);
	do
	{
		LCD_printchar(*text++);
	}while (*text != '\n');
}

void LCD_CLEAR_DISPLAY(void)
{
	LCD_CMD(0x01);
	osDelay(2);
}

void LCD_2ndROW(void)
{
	LCD_CMD(0xC0);
}

void LCD_HOME(void)
{
	LCD_CMD(0x02);
	osDelay(2);
}

void LCD_LSHIFT(void)
{
	LCD_CMD(0x18);
}

void LCD_RSHIFT(void)
{
	LCD_CMD(0x1C);
}

void LCD_DISP_ON(void)
{
	LCD_CMD(0x0C);
}

void LCD_DISP_OFF(void)
{
	LCD_CMD(0x08);
}

void LCD_DISP_CURSOR(void)
{
	LCD_CMD(0x0E); // Cursor ON / Blink ON
}

void LCD_LOCATE(uint8_t row, uint8_t column)
{
	column--;
	switch (row)
	{
		case 1:
			/* Set cursor to 1st row address and add index*/
			LCD_CMD(column |= 0x80);
			break;
		case 2:
			/* Set cursor to 2nd row address and add index*/
			LCD_CMD(column |= 0x40 | 0x80);
			break;
		default:
			break;
	}
}

void LCD_printf(const char *fmt, ...)
{
	int i;
	uint32_t text_size, letter;
	static char text_buffer[32];
	va_list args;

	va_start(args, fmt);
	text_size = vsprintf(text_buffer, fmt, args);

	// Process the string
	for (i = 0; i < text_size; i++){
		letter = text_buffer[i];

		if (letter == 10){
			break;
		}else{
			if ((letter > 0x1F) && (letter < 0x80)){
				LCD_printchar(letter);
			}
		}
	}
}

void LCD_INIT(void)
{
	HAL_GPIO_WritePin(LCD_EN_GPIO_Port, LCD_EN_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_RW_GPIO_Port, LCD_RW_Pin, GPIO_PIN_RESET);

	//Initialization of HD44780-based LCD (4-bit HW)
	LCD_CMD(0x33);
	osDelay(4);
	LCD_CMD(0x32);
	osDelay(4);
	//Function Set 4-bit mode DL=0 N=1(2Lie 5*8 dots) F=0(Don't care)
	LCD_CMD(0x28);
	//Display On/Off Control curso x, blink x
	LCD_CMD(0x0C);

	//Entry mode set, Increment, display not shift
	LCD_CMD(0x06);

	LCD_CLEAR_DISPLAY();
	//Minimum delay to wait before driving LCD module
	osDelay(200);

	LCD_CMD(CGRAM_address_start);
	LCD_LOAD_CGRAM(Custom, 6);
}
