#ifndef __H_TMC2209_H__
#define __H_TMC2209_H__

#include "gpio.h"

#define MAX_SPEED_LEVEL 39


typedef enum
{
    Constant_speed = 1, // 恒速模式
    Constant_step = 2,  // 定步模式
    STOP_mode = 3       // 停止模式
} MotorMode;

typedef struct
{
    uint8_t mode;
    uint16_t hz;
    uint16_t current_step;
    uint16_t target_step;
} MotorStruct;

// 加速度/减速度：单位 Hz/ms （每 1ms SysTick 增/减多少 Hz）
#define ACCEL_HZ 100u
// 定时器输入时钟：72 MHz，预分频 72 → 定时器计数频率 = 10 kHz
#define TIMER_CLK_HZ 1000000u

void Motor_Init(void);

void Motor_SetSpeed(uint8_t num, uint8_t mode, GPIO_PinState dir, uint16_t hz);
void Motor1_SetSpeed(uint8_t en, GPIO_PinState dir, uint16_t level);
void Motor2_SetSpeed(uint8_t en, GPIO_PinState dir, uint16_t level);
void Motor3_SetSpeed(uint8_t en, GPIO_PinState dir, uint16_t level);
void Motor4_SetSpeed(uint8_t en, GPIO_PinState dir, uint16_t level);
void Motor5_SetSpeed(uint8_t en, GPIO_PinState dir, uint16_t level);
void Motor6_SetSpeed(uint8_t en, GPIO_PinState dir, uint16_t level);

uint16_t Motor1_GetStep();
uint16_t Motor2_GetStep();
uint16_t Motor3_GetStep();
uint16_t Motor4_GetStep();
uint16_t Motor5_GetStep();
uint16_t Motor6_GetStep();
#endif