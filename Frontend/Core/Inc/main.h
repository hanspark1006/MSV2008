/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "log.h"
#include "macros_common.h"
#include "errno.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define STATUS_LED_Pin GPIO_PIN_13
#define STATUS_LED_GPIO_Port GPIOC
#define DOWN_SW_Pin GPIO_PIN_6
#define DOWN_SW_GPIO_Port GPIOA
#define UP_SW_Pin GPIO_PIN_7
#define UP_SW_GPIO_Port GPIOA
#define ENTER_SW_Pin GPIO_PIN_0
#define ENTER_SW_GPIO_Port GPIOB
#define MODE_SW_Pin GPIO_PIN_1
#define MODE_SW_GPIO_Port GPIOB
#define LCD_D7_Pin GPIO_PIN_12
#define LCD_D7_GPIO_Port GPIOB
#define LCD_D6_Pin GPIO_PIN_13
#define LCD_D6_GPIO_Port GPIOB
#define LCD_D5_Pin GPIO_PIN_14
#define LCD_D5_GPIO_Port GPIOB
#define LCD_D4_Pin GPIO_PIN_15
#define LCD_D4_GPIO_Port GPIOB
#define LCD_EN_Pin GPIO_PIN_8
#define LCD_EN_GPIO_Port GPIOA
#define LCD_RW_Pin GPIO_PIN_11
#define LCD_RW_GPIO_Port GPIOA
#define LCD_RS_Pin GPIO_PIN_12
#define LCD_RS_GPIO_Port GPIOA
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

/* USER CODE BEGIN Private defines */
#define MAX_CHANNEL		8
#define CMD_MAX_SIZE	10
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
