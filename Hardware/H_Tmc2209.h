#ifndef __H_TMC2209_H__
#define __H_TMC2209_H__

#include "gpio.h"
#include "math.h"
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

typedef struct
{
    TrapezoidProfile velocity;
    MotionPhase steps;
    uint16_t step_index;
    uint16_t hz_index;
    uint8_t motor_mode;
} StepperMotor;

// 定时器输入时钟：72 MHz，预分频 72 → 定时器计数频率 = 10 kHz
#define TIMER_CLK_HZ 1000000u

void Motor_Init(void);

void Motor_SetSpeed(uint8_t num, uint8_t mode, GPIO_PinState dir, uint16_t hz);
void Motor1_SetSpeed(uint8_t en, GPIO_PinState dir);
void Motor2_SetSpeed(uint8_t en, GPIO_PinState dir);
void Motor3_SetSpeed(uint8_t en, GPIO_PinState dir);

uint16_t Motor1_GetStep();
uint16_t Motor2_GetStep();
uint16_t Motor3_GetStep();
#endif