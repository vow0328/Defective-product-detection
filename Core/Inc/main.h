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

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define COM2IN_Pin GPIO_PIN_14
#define COM2IN_GPIO_Port GPIOC
#define EN2IN_Pin GPIO_PIN_15
#define EN2IN_GPIO_Port GPIOC
#define DIR2IN_Pin GPIO_PIN_1
#define DIR2IN_GPIO_Port GPIOA
#define TX2_Pin GPIO_PIN_2
#define TX2_GPIO_Port GPIOA
#define RX2_Pin GPIO_PIN_3
#define RX2_GPIO_Port GPIOA
#define DIR3IN_Pin GPIO_PIN_5
#define DIR3IN_GPIO_Port GPIOA
#define EN3IN_Pin GPIO_PIN_7
#define EN3IN_GPIO_Port GPIOA
#define COM3IN_Pin GPIO_PIN_0
#define COM3IN_GPIO_Port GPIOB
#define TX3_Pin GPIO_PIN_10
#define TX3_GPIO_Port GPIOB
#define RX3_Pin GPIO_PIN_11
#define RX3_GPIO_Port GPIOB
#define COM1IN_Pin GPIO_PIN_14
#define COM1IN_GPIO_Port GPIOB
#define EN1IN_Pin GPIO_PIN_15
#define EN1IN_GPIO_Port GPIOB
#define DIR1IN_Pin GPIO_PIN_9
#define DIR1IN_GPIO_Port GPIOA
#define OUT1_Pin GPIO_PIN_4
#define OUT1_GPIO_Port GPIOB
#define OUT2_Pin GPIO_PIN_5
#define OUT2_GPIO_Port GPIOB
#define OUT3_Pin GPIO_PIN_6
#define OUT3_GPIO_Port GPIOB
#define OUT4_Pin GPIO_PIN_7
#define OUT4_GPIO_Port GPIOB
#define SCL_Pin GPIO_PIN_8
#define SCL_GPIO_Port GPIOB
#define SDA_Pin GPIO_PIN_9
#define SDA_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
