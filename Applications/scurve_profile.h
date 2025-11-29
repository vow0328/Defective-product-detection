/**
 * @file    scurve_profile.h
 * @brief   七段式 Jerk 限制 S 形加减速算法
 *
 * 速度曲线七段划分：
 *   T1: 加加速段 (+jerk, a: 0 → +a_max)
 *   T2: 匀加速段 (0 jerk, a = +a_max)
 *   T3: 减加速段 (-jerk, a: +a_max → 0)
 *   T4: 匀速段   (0 jerk, a = 0, v = v_max)
 *   T5: 加减速段 (-jerk, a: 0 → -a_max)
 *   T6: 匀减速段 (0 jerk, a = -a_max)
 *   T7: 减减速段 (+jerk, a: -a_max → 0)
 *
 * 短距离自动降级：
 *   距离不足 → 5段 (T2=0, T6=0，跳过匀加/减速)
 *   距离更短 → 3段 (T2=T4=T6=0，三角形 S 曲线)
 */

#ifndef _SCURVE_PROFILE_H
#define _SCURVE_PROFILE_H

#include <stdint.h>
#include <stdbool.h>
#include "fixed_math.h"

/* ── S 曲线配置参数 ────────────────────────────────────────────── */

typedef struct
{
    uint16_t v_start;       /* 起始速度 (steps/s)                     */
    uint16_t v_max;         /* 最大速度 (steps/s)                     */
    uint16_t v_end;         /* 结束速度 (steps/s), 通常为 0           */
    uint16_t a_max;         /* 最大加速度 (steps/s²)                  */
    uint16_t j_max;         /* 最大加加速度 / jerk (steps/s³)         */
    uint32_t total_steps;   /* 总步数                                */
} SCurveConfig;

/* ── 规划结果 (预计算, 不进入 ISR 热路径) ───────────────────────── */

typedef struct
{
    /* 各段步数 (0 表示该段不存在) */
    uint32_t t1_steps, t2_steps, t3_steps;
    uint32_t t4_steps;
    uint32_t t5_steps, t6_steps, t7_steps;

    /* 各段累计步数边界 (用于快速判断当前所在段) */
    uint32_t seg_bound[8];  /* [0]=0, [1]=T1, [2]=T1+T2, ... [7]=total */

    /* ── 时间轴参数 (fixed_t Q16.16 秒) ── */
    fixed_t  t1_dur, t2_dur, t3_dur;
    fixed_t  t4_dur;
    fixed_t  t5_dur, t6_dur, t7_dur;

    fixed_t  seg_time_bound[8];  /* 各段累计时间边界                */

    /* ── 关键速度值 (fixed_t) ── */
    fixed_t  v_start_fp;
    fixed_t  v_max_fp;
    fixed_t  v_end_fp;
    fixed_t  v1_end;         /* T1 结束时的速度                      */
    fixed_t  v3_end;         /* T3 结束时的速度 (= v_max)            */

    /* ── 预计算系数 (避免 ISR 中重复计算) ── */
    fixed_t  j_coef;         /* jerk/2, 用于 v = v0 + j*t²/2        */
    fixed_t  j_coef_neg;     /* -jerk/2, 用于减速段                  */
    fixed_t  a_max_fp;       /* 加速度 (Q16.16)                     */

    /* ── 标志位 ── */
    uint8_t  active_segments; /* 实际激活段数: 7, 5, 或 3           */
    bool     valid;            /* 规划是否成功                       */
} SCurvePlan;

/* ── 运行时状态 (ISR 中更新) ───────────────────────────────────── */

typedef struct
{
    uint8_t  segment;        /* 当前段号 1~7                        */
    uint32_t step_in_seg;    /* 当前段内步数                        */
    fixed_t  time_in_seg;    /* 当前段内累计时间 (Q16.16)           */
    fixed_t  current_v;      /* 当前速度 (Q16.16)                   */
    fixed_t  current_a;      /* 当前加速度 (Q16.16)                 */
    uint16_t current_hz;     /* 当前速度整数 (steps/s)              */
    bool     finished;       /* 运动是否完成                        */
    int32_t  acc_accum;      /* 加速度累加器 (用于平滑更新)         */
} SCurveState;

/* ── API 函数 ──────────────────────────────────────────────────── */

/**
 * calc_scurve_profile — 计算七段 S 曲线规划
 *
 * @param  cfg   输入: 运动参数 (v_start, v_max, v_end, a_max, j_max, total_steps)
 * @param  plan  输出: 预计算的分段步数、时间、速度边界
 * @return       true=规划成功, false=参数不合理
 *
 * 自动处理短距离降级:
 *   - 无法达到 a_max → 缩减为 5 段 (无匀加速段)
 *   - 无法达到 v_max → 缩减为 3 段 (三角形 S 曲线)
 */
bool calc_scurve_profile(const SCurveConfig *cfg, SCurvePlan *plan);

/**
 * scurve_state_init — 初始化 S 曲线运行时状态
 * 在运动开始前调用一次
 */
void scurve_state_init(const SCurvePlan *plan, SCurveState *state);

/**
 * scurve_step_update — 每步更新速度 (ISR 安全, 无浮点/无除法)
 *
 * @param  plan   预计算的 S 曲线规划
 * @param  state  运行时状态 (会被更新)
 * @return        当前步对应的速度 (steps/s, uint16_t)
 *
 * 算法: 维护 current_v (Q16.16), 用当前段方程更新
 *   T1/T3/T5/T7: v 按二次曲线变化 (只用乘法和加法)
 *   T2/T6:       v 按线性变化
 *   T4:          v 保持恒定
 *
 * 利用关系: dv/dn ≈ a/v  (其中 n 为步数)
 *   即: v_new = v_old + (a * FIXED_ONE) / v_old
 *   用 1/v 查表避免除法
 */
uint16_t scurve_step_update(const SCurvePlan *plan, SCurveState *state);

/**
 * scurve_get_speed — 根据累计步数查速度 (无状态版本)
 * 用于替代原 get_step_speed(), 兼容旧接口
 *
 * @param  plan       预计算的 S 曲线规划
 * @param  step_idx   当前步索引 (0-based)
 * @return            速度 (steps/s)
 */
uint16_t scurve_get_speed(const SCurvePlan *plan, uint32_t step_idx);

#endif /* _SCURVE_PROFILE_H */
