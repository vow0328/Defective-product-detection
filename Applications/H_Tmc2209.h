#ifndef __H_TMC2209_H__
#define __H_TMC2209_H__

#include "gpio.h"

#define motor2 GPIOB
#define motor3 GPIOC
#define EN2 GPIO_PIN_13
#define EN3 GPIO_PIN_2
#define DIR2 GPIO_PIN_15
#define DIR3 GPIO_PIN_5
#define STEP2 TIM2CH1
#define STEP3 TIM3CH1

#define MAX_SPEED_LEVEL 39

void Motor_Init(void);

void Motor1_SetSpeed(uint8_t en, GPIO_PinState dir, uint8_t level);
void Motor2_SetSpeed(uint8_t en, GPIO_PinState dir, uint8_t level);
void Motor3_SetSpeed(uint8_t en, GPIO_PinState dir, uint8_t level);
#endif