/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __UART_H
#define __UART_H

/* Includes ------------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */
int uart_init(void);
void uart_task(void);
int uart_SendFront(uint8_t cmd, uint8_t ch, uint32_t data);

#endif /* __UART_H */
