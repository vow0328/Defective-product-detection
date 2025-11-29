/**
 * @file    torque_comp.c
 * @brief   前馈式力矩补偿实现
 *
 * 补偿原理:
 *   PWM 占空比直接影响 TMC2209 驱动器的 VREF 等效电压。
 *   高占空比 → 更大的平均电流 → 补偿高速段转矩损失。
 *
 * 默认补偿曲线 (可标定):
 *   速度 0~500:   占空比 50% (基准)
 *   速度 500~2000: 占空比 50%→65% (线性)
 *   速度 2000~5000: 占空比 65%→75% (线性)
 *   速度 >5000:    占空比 75% (最大值)
 */

#include "torque_comp.h"
#include <string.h>

/* ══════════════════════════════════════════════════════════════════
 *  全局补偿配置
 * ══════════════════════════════════════════════════════════════════ */

TorqueCompConfig torque_config[TORQUE_AXES + 1];  /* [0] 未使用 */

/* ── 默认补偿曲线 ── */
static const TorqueCompPoint default_points[TORQUE_COMP_POINTS] =
{
    {    0, 50 },  /* 零速: 50% 基准占空比 */
    {  800, 55 },  /* 低速: 轻微补偿       */
    { 3000, 65 },  /* 中速: 明显补偿       */
    { 8000, 75 },  /* 高速: 最大补偿       */
};

/* ══════════════════════════════════════════════════════════════════
 *  torque_comp_init
 * ══════════════════════════════════════════════════════════════════ */

void torque_comp_init(void)
{
    for (int axis = 1; axis <= TORQUE_AXES; axis++)
    {
        torque_config[axis].enabled   = true;
        torque_config[axis].base_duty = 50;
        torque_config[axis].max_duty  = 75;
        memcpy(torque_config[axis].points, default_points,
               sizeof(default_points));
    }
}

/* ══════════════════════════════════════════════════════════════════
 *  torque_comp_get_duty — 线性插值查表
 * ══════════════════════════════════════════════════════════════════ */

uint8_t torque_comp_get_duty(uint8_t axis, uint16_t speed)
{
    if (axis < 1 || axis > TORQUE_AXES) return 50;
    if (!torque_config[axis].enabled) return 50;

    const TorqueCompPoint *pts = torque_config[axis].points;
    uint8_t base = torque_config[axis].base_duty;
    uint8_t max  = torque_config[axis].max_duty;

    /* ── 低于第一个点 ── */
    if (speed <= pts[0].speed)
        return base;

    /* ── 线性插值查找 ── */
    for (int i = 0; i < TORQUE_COMP_POINTS - 1; i++)
    {
        if (speed >= pts[i].speed && speed <= pts[i + 1].speed)
        {
            uint16_t s0 = pts[i].speed;
            uint16_t s1 = pts[i + 1].speed;
            uint8_t  d0 = pts[i].duty_pct;
            uint8_t  d1 = pts[i + 1].duty_pct;

            if (s1 == s0) return d0;

            /* 线性插值 */
            uint32_t frac = (uint32_t)(speed - s0) * (d1 - d0) / (s1 - s0);
            uint8_t duty = (uint8_t)(d0 + frac);

            /* 限幅 */
            if (duty > max) duty = max;
            if (duty < base) duty = base;
            return duty;
        }
    }

    /* ── 高于最后一个点 ── */
    return pts[TORQUE_COMP_POINTS - 1].duty_pct;
}

/* ══════════════════════════════════════════════════════════════════
 *  torque_comp_set_point
 * ══════════════════════════════════════════════════════════════════ */

void torque_comp_set_point(uint8_t axis, uint8_t index,
                           uint16_t speed, uint8_t duty)
{
    if (axis < 1 || axis > TORQUE_AXES) return;
    if (index >= TORQUE_COMP_POINTS) return;

    /* 限幅 */
    if (duty < 30)  duty = 30;
    if (duty > 95)  duty = 95;

    torque_config[axis].points[index].speed    = speed;
    torque_config[axis].points[index].duty_pct = duty;
}

/* ══════════════════════════════════════════════════════════════════
 *  torque_comp_set_enable
 * ══════════════════════════════════════════════════════════════════ */

void torque_comp_set_enable(uint8_t axis, bool enable)
{
    if (axis < 1 || axis > TORQUE_AXES) return;
    torque_config[axis].enabled = enable;
}

/* ══════════════════════════════════════════════════════════════════
 *  torque_comp_calibrate — 简易自动标定
 *
 *  手动标定模式: 通过串口指令依次测试各速度点，
 *  人工观察丢步情况，主机设定补偿量。
 *  本函数提供框架，实际丢步检测依赖外部反馈。
 * ══════════════════════════════════════════════════════════════════ */

int torque_comp_calibrate(uint8_t axis, uint16_t v_start,
                          uint16_t v_end, uint16_t v_step)
{
    if (axis < 1 || axis > TORQUE_AXES) return -1;
    if (v_step == 0 || v_start > v_end) return -2;

    /* 标定点数限制 */
    uint16_t range = v_end - v_start;
    uint8_t num_points = (uint8_t)(range / v_step) + 1;
    if (num_points > TORQUE_COMP_POINTS)
        num_points = TORQUE_COMP_POINTS;

    /* 均匀分布标定点 */
    uint16_t step = range / (num_points - 1);
    if (step < 1) step = 1;

    for (uint8_t i = 0; i < num_points; i++)
    {
        uint16_t speed = v_start + (uint16_t)i * step;
        if (speed > v_end) speed = v_end;

        /* 默认线性增长占空比: 50% → 75% */
        uint8_t duty = (uint8_t)(50 + (uint32_t)i * 25 / (num_points - 1));

        torque_config[axis].points[i].speed    = speed;
        torque_config[axis].points[i].duty_pct = duty;
    }

    /* 填充剩余点为最大值 */
    for (uint8_t i = num_points; i < TORQUE_COMP_POINTS; i++)
    {
        torque_config[axis].points[i].speed    = v_end;
        torque_config[axis].points[i].duty_pct = torque_config[axis].max_duty;
    }

    return 0;
}
