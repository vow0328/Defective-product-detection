#ifndef __H_TMC2209_H__
#define __H_TMC2209_H__

#include "gpio.h"

#define MAX_SPEED_LEVEL 39

void Motor_Init(void);

void Motor1_SetSpeed(uint8_t en, GPIO_PinState dir, uint8_t level);
void Motor2_SetSpeed(uint8_t en, GPIO_PinState dir, uint8_t level);
void Motor3_SetSpeed(uint8_t en, GPIO_PinState dir, uint8_t level);

uint32_t Motor1_GetStep();
uint32_t Motor2_GetStep();
uint32_t Motor3_GetStep();
#endif