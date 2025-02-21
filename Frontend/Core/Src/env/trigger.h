/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TRIGGER_H
#define __TRIGGER_H

/* Includes ------------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */
void trigger_init(void);
void trigger_set(uint8_t signal);
#endif /* __TRIGGER_H */
