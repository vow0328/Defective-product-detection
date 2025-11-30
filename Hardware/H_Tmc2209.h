#ifndef __H_TMC2209_H__
#define __H_TMC2209_H__

#include "gpio.h"
#include "math.h"
#include "scurve_profile.h"
#include "torque_comp.h"

typedef enum
{
    Constant_speed = 1, // 恒速模式
    Constant_step = 2,  // 定步模式
    STOP_mode = 3,      // 停止模式
    SCurve_step = 4     // S 曲线定步模式
} MotorMode;

typedef enum
{
    PROFILE_TRAPEZOID = 0,  // 梯形加减速 (原算法)
    PROFILE_SCURVE   = 1    // 七段 S 曲线 (新算法)
} ProfileType;

typedef struct
{
    uint8_t  mode;
    uint8_t  en;
    GPIO_PinState dir;
    uint16_t hz;            // 当前速度 (steps/s)
    uint16_t current_step;
    uint16_t target_step;
    uint16_t arr;            // 定时器自动重装值

    /* ── 梯形 profile (兼容旧接口) ── */
    MotionStep steps;
    TrapezoidVelocity velocity;

    /* ── S 曲线 profile ── */
    ProfileType profile_type;    // 当前使用的曲线类型
    SCurveConfig scurve_cfg;     // S 曲线配置参数
    SCurvePlan  scurve_plan;     // S 曲线预计算结果
    SCurveState scurve_state;    // S 曲线运行时状态
    uint8_t     comp_duty;       // 当前补偿后占空比 (%)
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

void Motor_Set(uint8_t num, uint8_t mode, GPIO_PinState dir, uint16_t hz, uint16_t vstart, uint16_t vmax, uint16_t acc);
void Motor_SetSpeed(uint8_t num);

uint16_t Motor_GetStep(uint8_t num);

#endif /* __H_TMC2209_H__ */