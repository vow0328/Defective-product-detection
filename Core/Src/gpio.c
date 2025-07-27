/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration
  *          of all used GPIO pins.
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

/* Includes ------------------------------------------------------------------*/
#include "gpio.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, OUTPUT1_Pin|OUTPUT2_Pin|EN1OUT_Pin|SCL1_Pin
                          |DIR3OUTC7_Pin|OUT_Pin|DIR6OUT_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, DIR1OUT_Pin|EN2OUT_Pin|EN6OUT_Pin|INPUT8_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, SDA1_Pin|SCL2_Pin|SDA2_Pin|EN3OUT_Pin
                          |EN5OUTB4_Pin|DIR4OUT_Pin|EN4OUT_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(DIR5OUT_GPIO_Port, DIR5OUT_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : OUTPUT1_Pin OUTPUT2_Pin SCL1_Pin DIR3OUTC7_Pin
                           OUT_Pin DIR6OUT_Pin */
  GPIO_InitStruct.Pin = OUTPUT1_Pin|OUTPUT2_Pin|SCL1_Pin|DIR3OUTC7_Pin
                          |OUT_Pin|DIR6OUT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : EN1OUT_Pin */
  GPIO_InitStruct.Pin = EN1OUT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(EN1OUT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : DIR1OUT_Pin EN6OUT_Pin */
  GPIO_InitStruct.Pin = DIR1OUT_Pin|EN6OUT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : EN2OUT_Pin */
  GPIO_InitStruct.Pin = EN2OUT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(EN2OUT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : DIR2OUT_Pin */
  GPIO_InitStruct.Pin = DIR2OUT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(DIR2OUT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : KEY2_Pin */
  GPIO_InitStruct.Pin = KEY2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(KEY2_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : SDA1_Pin SCL2_Pin SDA2_Pin EN3OUT_Pin
                           EN5OUTB4_Pin DIR4OUT_Pin EN4OUT_Pin */
  GPIO_InitStruct.Pin = SDA1_Pin|SCL2_Pin|SDA2_Pin|EN3OUT_Pin
                          |EN5OUTB4_Pin|DIR4OUT_Pin|EN4OUT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : KEY1_Pin */
  GPIO_InitStruct.Pin = KEY1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(KEY1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : INPUT8_Pin */
  GPIO_InitStruct.Pin = INPUT8_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(INPUT8_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : INPUT4_Pin */
  GPIO_InitStruct.Pin = INPUT4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(INPUT4_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : INPUT5_Pin INPUT2_Pin INPUT3_Pin */
  GPIO_InitStruct.Pin = INPUT5_Pin|INPUT2_Pin|INPUT3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : DIR5OUT_Pin */
  GPIO_InitStruct.Pin = DIR5OUT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(DIR5OUT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : INPUT1_Pin */
  GPIO_InitStruct.Pin = INPUT1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(INPUT1_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */
