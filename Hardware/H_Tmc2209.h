#ifndef __H_TMC2209_H__
#define __H_TMC2209_H__

#include "gpio.h"

#define MAX_SPEED_LEVEL 39

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

typedef enum
{
    Constant_speed = 1, // 恒速模式
    Constant_step = 2,  // 定步模式
    STOP_mode = 3       // 停止模式
} MotorMode;

void Motor_Init();
void Motor1_Set(uint8_t en, GPIO_PinState dir, uint16_t hz,uint32_t step);

uint16_t Motor1_GetStep();
uint16_t Motor2_GetStep();
uint16_t Motor3_GetStep();
#endif