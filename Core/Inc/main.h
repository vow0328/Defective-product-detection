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
#define INPUT8_Pin GPIO_PIN_0
#define INPUT8_GPIO_Port GPIOC
#define INPUT7_Pin GPIO_PIN_1
#define INPUT7_GPIO_Port GPIOC
#define INPUT6_Pin GPIO_PIN_2
#define INPUT6_GPIO_Port GPIOC
#define INPUT5_Pin GPIO_PIN_3
#define INPUT5_GPIO_Port GPIOC
#define INPUT4_Pin GPIO_PIN_0
#define INPUT4_GPIO_Port GPIOA
#define INPUT3_Pin GPIO_PIN_1
#define INPUT3_GPIO_Port GPIOA
#define INPUT2_Pin GPIO_PIN_4
#define INPUT2_GPIO_Port GPIOA
#define INPUT1_Pin GPIO_PIN_5
#define INPUT1_GPIO_Port GPIOA
#define JDOUT4_Pin GPIO_PIN_5
#define JDOUT4_GPIO_Port GPIOC
#define JDOUT3_Pin GPIO_PIN_0
#define JDOUT3_GPIO_Port GPIOB
#define JDOUT2_Pin GPIO_PIN_1
#define JDOUT2_GPIO_Port GPIOB
#define JDOUT1_Pin GPIO_PIN_2
#define JDOUT1_GPIO_Port GPIOB
#define DIR1OUT_Pin GPIO_PIN_10
#define DIR1OUT_GPIO_Port GPIOB
#define EN1OUT_Pin GPIO_PIN_12
#define EN1OUT_GPIO_Port GPIOB
#define COM1OUT_Pin GPIO_PIN_13
#define COM1OUT_GPIO_Port GPIOB
#define DIR2OUT_Pin GPIO_PIN_15
#define DIR2OUT_GPIO_Port GPIOB
#define EN2OUT_Pin GPIO_PIN_7
#define EN2OUT_GPIO_Port GPIOC
#define COM2OUT_Pin GPIO_PIN_8
#define COM2OUT_GPIO_Port GPIOC
#define DIR3OUT_Pin GPIO_PIN_9
#define DIR3OUT_GPIO_Port GPIOC
#define EN3OUT_Pin GPIO_PIN_9
#define EN3OUT_GPIO_Port GPIOA
#define COM3OUT_Pin GPIO_PIN_10
#define COM3OUT_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
