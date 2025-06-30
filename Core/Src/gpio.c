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
  HAL_GPIO_WritePin(GPIOC, JDOUT4_Pin|EN2OUT_Pin|COM2OUT_Pin|DIR3OUT_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, JDOUT3_Pin|JDOUT2_Pin|JDOUT1_Pin|DIR1OUT_Pin
                          |EN1OUT_Pin|COM1OUT_Pin|DIR2OUT_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, EN3OUT_Pin|COM3OUT_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : INPUT8_Pin INPUT7_Pin INPUT6_Pin INPUT5_Pin */
  GPIO_InitStruct.Pin = INPUT8_Pin|INPUT7_Pin|INPUT6_Pin|INPUT5_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : INPUT4_Pin INPUT3_Pin INPUT2_Pin INPUT1_Pin */
  GPIO_InitStruct.Pin = INPUT4_Pin|INPUT3_Pin|INPUT2_Pin|INPUT1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : JDOUT4_Pin EN2OUT_Pin COM2OUT_Pin DIR3OUT_Pin */
  GPIO_InitStruct.Pin = JDOUT4_Pin|EN2OUT_Pin|COM2OUT_Pin|DIR3OUT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : JDOUT3_Pin */
  GPIO_InitStruct.Pin = JDOUT3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(JDOUT3_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : JDOUT2_Pin JDOUT1_Pin DIR1OUT_Pin EN1OUT_Pin
                           COM1OUT_Pin DIR2OUT_Pin */
  GPIO_InitStruct.Pin = JDOUT2_Pin|JDOUT1_Pin|DIR1OUT_Pin|EN1OUT_Pin
                          |COM1OUT_Pin|DIR2OUT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : EN3OUT_Pin COM3OUT_Pin */
  GPIO_InitStruct.Pin = EN3OUT_Pin|COM3OUT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */
