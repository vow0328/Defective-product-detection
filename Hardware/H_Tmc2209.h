#ifndef __H_TMC2209_H__
#define __H_TMC2209_H__

#include "gpio.h"
#include "math.h"

typedef enum {
    Constant_speed = 1, // 恒速模式
    Constant_step = 2,  // 定步模式
    STOP_mode = 3       // 停止模式
} MotorMode;

typedef struct {
    uint8_t mode;
    uint8_t en;
    GPIO_PinState dir;
    uint16_t hz;
    uint16_t current_step;
    uint16_t target_step;
    uint16_t arr;
    MotionStep steps;
    TrapezoidVelocity velocity;
} MotorStruct;

// 外部声明全局数组
extern GPIO_TypeDef *en_ports[];
extern uint16_t en_pins[];
extern GPIO_TypeDef *dir_port[];
extern uint16_t dir_pins[];
extern TIM_HandleTypeDef *motor_tim[];
extern uint32_t motor_channel[];

// 如果未定义 MOTOR_COUNT，则定义为 6
#ifndef MOTOR_COUNT
#define MOTOR_COUNT 6
#endif

// 定时器输入时钟：72 MHz，预分频 72 → 定时器计数频率 = 10 kHz
#define TIMER_CLK_HZ 1000000u

void stepper_init(MotorStruct *Motor, uint16_t v_start, uint16_t v_max, uint16_t acc, uint16_t steps);

void Motor_Set(uint8_t num, uint8_t mode, GPIO_PinState dir, uint16_t hz,uint16_t vstart,uint16_t vmax,uint16_t acc);
void Motor_SetSpeed(uint8_t num);

uint16_t Motor_GetStep(uint8_t num);

#endif /* __H_TMC2209_H__ */