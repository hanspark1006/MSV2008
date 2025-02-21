
/* Define to prevent recursive inclusion -------------------------------------*/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LCD_H
#define __LCD_H

/* Includes ------------------------------------------------------------------*/

/** @addtogroup STM8S_StdPeriph_Driver
  * @{
  */

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/
/* Exported macros ------------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

/** @addtogroup HD44780_Exported_Functions
  * @{
  */
void LCD_LOAD_CGRAM(char tab[], uint8_t charnum);
void LCD_PWRON (void);
void LCD_PWROFF (void);
void LCD_INIT(void);
void LCD_printchar(unsigned char ascode);
void LCD_printstring(char *text);
void LCD_CMD(unsigned char data);
void LCD_CLEAR_DISPLAY(void);
void LCD_2ndROW(void);
void LCD_HOME(void);
void LCD_LSHIFT(void);
void LCD_RSHIFT(void);
void LCD_DISP_ON(void);
void LCD_DISP_OFF(void);
void LCD_DISP_CURSOR(void);
void LCD_LOCATE(uint8_t row, uint8_t column);
void LCD_printf(const char *fmt, ...);


#endif /* __LCD_H */
