/**
 * @file    fixed_math.h
 * @brief   Q15/Q31 定点数运算库 — 替代浮点运算，提高 STM32F103 实时性能
 *
 * 格式说明：
 *   fixed_t  (int32_t): Q16.16 — 1位符号 + 15位整数 + 16位小数, 范围 ±32767.99998
 *   fixed16_t(int16_t): Q8.8  — 1位符号 + 7位整数  + 8位小数,  范围 ±127.996
 *
 * 乘法需用 int64 中间结果防止溢出，除法需左移后再除。
 */

#ifndef _FIXED_MATH_H
#define _FIXED_MATH_H

#include <stdint.h>
#include <stdbool.h>

/* ── 类型定义 ─────────────────────────────────────────────────── */

typedef int32_t  fixed_t;    /* Q16.16 主定点类型 */
typedef int16_t  fixed16_t;  /* Q8.8   辅助定点类型 */

/* ── 移位常量 ──────────────────────────────────────────────────── */

#define FIXED_FRAC_BITS   16
#define FIXED_ONE         (1L << FIXED_FRAC_BITS)       /* 1.0  = 65536      */
#define FIXED_HALF        (1L << (FIXED_FRAC_BITS - 1)) /* 0.5  = 32768      */
#define FIXED_MAX         ((fixed_t)0x7FFFFFFF)          /* 最大正数          */
#define FIXED_MIN         ((fixed_t)0x80000000)          /* 最小负数          */
#define FIXED_PI          ((fixed_t)205887)              /* π ≈ 3.14159       */
#define FIXED_TWO_PI      ((fixed_t)411775)              /* 2π                 */
#define FIXED_PI_DIV_2    ((fixed_t)102944)              /* π/2                */

#define FIXED16_FRAC_BITS 8
#define FIXED16_ONE       (0x0100)

/* ── 转换宏 ────────────────────────────────────────────────────── */

/** 整数 → fixed_t (不会溢出，因为 x 应 ≤ 32767 才安全) */
#define INT_TO_FIXED(x)   ((fixed_t)((int32_t)(x) << FIXED_FRAC_BITS))

/** fixed_t → 整数 (向零舍入) */
#define FIXED_TO_INT(x)   ((int16_t)((x) >> FIXED_FRAC_BITS))

/** float → fixed_t (仅用于编译期初始化，运行时避免使用) */
#define FLOAT_TO_FIXED(x) ((fixed_t)((x) * (float)FIXED_ONE + 0.5f))

/** fixed_t → float (仅调试用) */
#define FIXED_TO_FLOAT(x) ((float)(x) / (float)FIXED_ONE)

/** fixed_t 取绝对值 */
#define FIXED_ABS(x)      (((x) < 0) ? (-(x)) : (x))

/* ── 基本运算 (内联 / 宏) ──────────────────────────────────────── */

/**
 * fixed_mul — 定点乘法 (Q16.16 × Q16.16 → Q16.16)
 * 使用 int64 中间结果，结果四舍五入
 */
static inline fixed_t fixed_mul(fixed_t a, fixed_t b)
{
    return (fixed_t)(((int64_t)a * (int64_t)b + FIXED_HALF) >> FIXED_FRAC_BITS);
}

/**
 * fixed_div — 定点除法 (Q16.16 ÷ Q16.16 → Q16.16)
 * 先将被除数左移再除，结果四舍五入。除数不能为 0。
 */
static inline fixed_t fixed_div(fixed_t a, fixed_t b)
{
    if (b == 0) return (a >= 0) ? FIXED_MAX : FIXED_MIN;
    return (fixed_t)(((int64_t)a << FIXED_FRAC_BITS) / (int64_t)b);
}

/**
 * fixed_mul_int — 定点 × 整数 → 定点 (整数不缩放)
 */
static inline fixed_t fixed_mul_int(fixed_t a, int32_t b)
{
    return (fixed_t)((int64_t)a * b);
}

/**
 * fixed_div_int — 定点 ÷ 整数 → 定点
 */
static inline fixed_t fixed_div_int(fixed_t a, int32_t b)
{
    if (b == 0) return (a >= 0) ? FIXED_MAX : FIXED_MIN;
    return (fixed_t)(a / b);
}

/**
 * fixed_mul_q15 — Q16.16 × Q15(0.15) → Q16.16
 * b 是 0.15 格式（范围 [-1, 1)），常用于乘以归一化因子
 */
static inline fixed_t fixed_mul_q15(fixed_t a, int16_t b)
{
    return (fixed_t)(((int64_t)a * b + 0x2000) >> 15);
}

/**
 * fixed_saturate — 饱和到 int16_t 范围
 */
static inline int16_t fixed_saturate_int(fixed_t x)
{
    if (x > (fixed_t)(32767 << FIXED_FRAC_BITS)) return 32767;
    if (x < 0) return 0;
    return (int16_t)FIXED_TO_INT(x);
}

/* ── 高级数学函数 ──────────────────────────────────────────────── */

/**
 * fixed_sqrt — 定点平方根 (Newton-Raphson 迭代)
 * 输入/输出均为 Q16.16 格式。输入必须 ≥ 0。
 * 平均执行时间 ~15μs @72MHz
 */
fixed_t fixed_sqrt(fixed_t x);

/**
 * fixed_sqrt_fast — 快速平方根 (查表 + 线性插值)
 * 使用 256 点查找表，精度 ±0.1%，执行时间 ~2μs @72MHz
 * 输入: Q16.16, 范围 0 ~ 65535<<16 (支持 uint16_t 范围的平方)
 */
fixed_t fixed_sqrt_fast(fixed_t x);

/**
 * fixed_sin — 定点正弦 (查表法)
 * 输入: Q16.16 弧度, 自动归一到 [0, 2π)
 * 输出: Q1.15 格式 (范围 [-1, 1)), 即 int16_t
 */
int16_t fixed_sin(fixed_t rad);

/**
 * fixed_cos — 定点余弦 (查表法)
 * 等价于 fixed_sin(rad + π/2)
 */
int16_t fixed_cos(fixed_t rad);

/* ── 实用辅助 ─────────────────────────────────────────────────── */

/**
 * uint16_sqrt — 整数平方根
 * 用于 uint16_t 范围的速度计算，结果也为 uint16_t
 */
uint16_t uint16_sqrt(uint32_t x);

/**
 * 初始化平方根查找表 (系统启动时调用一次)
 */
void fixed_math_init(void);

#endif /* _FIXED_MATH_H */
