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
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "log.h"
#include "m_event_queue.h"
#include "errno.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;
extern UART_HandleTypeDef huart6;
/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
#define CCMRAM __attribute__((section(".ccmram")))
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define OPLED_Pin GPIO_PIN_13
#define OPLED_GPIO_Port GPIOC
#define STATUS_LED_Pin GPIO_PIN_14
#define STATUS_LED_GPIO_Port GPIOC
#define ETH_NSS_Pin GPIO_PIN_4
#define ETH_NSS_GPIO_Port GPIOA
#define OP2_Pin GPIO_PIN_12
#define OP2_GPIO_Port GPIOB
#define OP1_Pin GPIO_PIN_13
#define OP1_GPIO_Port GPIOB
#define FPGA_INIT_Pin GPIO_PIN_3
#define FPGA_INIT_GPIO_Port GPIOB
#define FPGA_DONE_Pin GPIO_PIN_4
#define FPGA_DONE_GPIO_Port GPIOB
#define FPGA_RST_Pin GPIO_PIN_5
#define FPGA_RST_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
#define MCU_VERSION "V0.1"
#define FPGA_VERSION "V1.0"
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
