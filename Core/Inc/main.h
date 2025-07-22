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
#define KEY2_Pin GPIO_PIN_0
#define KEY2_GPIO_Port GPIOC
#define KEY1_Pin GPIO_PIN_1
#define KEY1_GPIO_Port GPIOC
#define OUTPUT8_Pin GPIO_PIN_2
#define OUTPUT8_GPIO_Port GPIOC
#define OUTPUT7_Pin GPIO_PIN_3
#define OUTPUT7_GPIO_Port GPIOC
#define OUTPUT6_Pin GPIO_PIN_0
#define OUTPUT6_GPIO_Port GPIOA
#define OUTPUT5_Pin GPIO_PIN_1
#define OUTPUT5_GPIO_Port GPIOA
#define DIR5OUT_Pin GPIO_PIN_2
#define DIR5OUT_GPIO_Port GPIOA
#define EN5OUT_Pin GPIO_PIN_4
#define EN5OUT_GPIO_Port GPIOA
#define OUTPUT4_Pin GPIO_PIN_5
#define OUTPUT4_GPIO_Port GPIOA
#define OUTPUT3_Pin GPIO_PIN_6
#define OUTPUT3_GPIO_Port GPIOA
#define OUTPUT2_Pin GPIO_PIN_7
#define OUTPUT2_GPIO_Port GPIOA
#define OUTPUT1_Pin GPIO_PIN_4
#define OUTPUT1_GPIO_Port GPIOC
#define DIR4OUT_Pin GPIO_PIN_5
#define DIR4OUT_GPIO_Port GPIOC
#define EN4OUT_Pin GPIO_PIN_1
#define EN4OUT_GPIO_Port GPIOB
#define DIR3OUT_Pin GPIO_PIN_2
#define DIR3OUT_GPIO_Port GPIOB
#define EN3OUT_Pin GPIO_PIN_11
#define EN3OUT_GPIO_Port GPIOB
#define INPUT8_Pin GPIO_PIN_12
#define INPUT8_GPIO_Port GPIOB
#define INPUT7_Pin GPIO_PIN_13
#define INPUT7_GPIO_Port GPIOB
#define INPUT6_Pin GPIO_PIN_14
#define INPUT6_GPIO_Port GPIOB
#define INPUT5_Pin GPIO_PIN_15
#define INPUT5_GPIO_Port GPIOB
#define DIR2OUT_Pin GPIO_PIN_6
#define DIR2OUT_GPIO_Port GPIOC
#define EN2OUT_Pin GPIO_PIN_8
#define EN2OUT_GPIO_Port GPIOC
#define DIR1OUT_Pin GPIO_PIN_9
#define DIR1OUT_GPIO_Port GPIOC
#define EN1OUT_Pin GPIO_PIN_9
#define EN1OUT_GPIO_Port GPIOA
#define INPUT4_Pin GPIO_PIN_10
#define INPUT4_GPIO_Port GPIOA
#define INPUT3_Pin GPIO_PIN_11
#define INPUT3_GPIO_Port GPIOA
#define INPUT2_Pin GPIO_PIN_12
#define INPUT2_GPIO_Port GPIOA
#define INPUT1_Pin GPIO_PIN_15
#define INPUT1_GPIO_Port GPIOA
#define SCL_Pin GPIO_PIN_3
#define SCL_GPIO_Port GPIOB
#define SDA_Pin GPIO_PIN_4
#define SDA_GPIO_Port GPIOB
#define DIR6OUT_Pin GPIO_PIN_6
#define DIR6OUT_GPIO_Port GPIOB
#define EN6OUT_Pin GPIO_PIN_8
#define EN6OUT_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
