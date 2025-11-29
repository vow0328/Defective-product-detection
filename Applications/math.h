/**
 * @file    math.h
 * @brief   运动控制数学库 — 梯形 & S 曲线加减速
 *
 * 保留原有梯形加减速接口 (向后兼容),
 * 新增 S 曲线配置和规划结构。
 *
 * 注意: 原浮点 sqrtf 实现仅用于梯形兼容模式,
 * 新代码请使用 fixed_math.h 中的定点运算。
 */

#ifndef _MATH_H
#define _MATH_H

#include <stdint.h>
#include <stdbool.h>

/* ── 原有梯形加减速结构 (向后兼容) ──────────────────────────────── */

typedef struct
{
    uint16_t v_start;      /* 起始速度 (步/秒)                    */
    uint16_t v_max;        /* 最大速度 (步/秒)                    */
    uint16_t acc;          /* 加速度 (步/秒²)                     */
    uint16_t total_steps;  /* 总步数                              */
} TrapezoidVelocity;

typedef struct
{
    uint16_t accel_steps;   /* 加速段步数                         */
    uint16_t decel_steps;   /* 减速段步数                         */
    uint16_t cruise_steps;  /* 匀速段步数                         */
} MotionStep;

/* ── API ────────────────────────────────────────────────────────── */

/**
 * calc_trapezoid_profile — 计算梯形加减速规划 (原接口, 保持不变)
 */
MotionStep calc_trapezoid_profile(TrapezoidVelocity profile);

/**
 * get_step_speed — 根据当前步数获取对应速度 (原接口, 保持不变)
 *
 * 注意: 此函数内部使用 float sqrtf, 不推荐在 ISR 中调用。
 * 新代码建议使用 scurve_step_update() (纯定点运算)。
 */
uint16_t get_step_speed(int step_idx, MotionStep phase,
                        TrapezoidVelocity profile);

#endif /* _MATH_H */
