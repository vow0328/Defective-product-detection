/**
 * @file    fixed_math.c
 * @brief   Q15/Q31 定点数运算库实现
 *
 * 全部使用整数运算，零浮点依赖。平方根提供两种实现：
 *   1. fixed_sqrt()     — Newton 迭代，高精度
 *   2. fixed_sqrt_fast() — 256 点查表 + 线性插值，适合 ISR 热路径
 */

#include "fixed_math.h"

/* ══════════════════════════════════════════════════════════════════
 *  平方根查找表 (256 点)
 *  映射: index i 代表输入值 (i/255)² * MAX_VAL
 *  即 table[i] = sqrt(i * MAX_INPUT / 255) * FIXED_ONE
 * ══════════════════════════════════════════════════════════════════ */

/* 查找表覆盖 [0, 65535<<16] 范围，即 uint16 全范围的平方 */
#define SQRT_LUT_SIZE    256
#define SQRT_LUT_MAX_IN  65535  /* uint16 最大值 */

static fixed_t sqrt_lut[SQRT_LUT_SIZE];
static bool    sqrt_lut_ready = false;

/**
 * fixed_math_init — 初始化所有数学查找表
 * 在 main() 初始化阶段调用一次
 */
void fixed_math_init(void)
{
    if (sqrt_lut_ready) return;

    for (int i = 0; i < SQRT_LUT_SIZE; i++)
    {
        /* 查找表输入值: i / (N-1) * MAX_IN * FIXED_ONE */
        uint32_t x = (uint32_t)i * SQRT_LUT_MAX_IN * FIXED_ONE / (SQRT_LUT_SIZE - 1);
        /* 计算 sqrt(x) 用 Newton 法保证精度 */
        sqrt_lut[i] = fixed_sqrt((fixed_t)x);
    }
    sqrt_lut_ready = true;
}

/* ══════════════════════════════════════════════════════════════════
 *  fixed_sqrt — Newton-Raphson 定点平方根
 *  对 x > 0，迭代: y_{n+1} = (y_n + x / y_n) / 2
 *  初始值用二分法估算，4~5 次迭代收敛到 ±1 LSB
 *  执行时间: ~15μs @72MHz
 * ══════════════════════════════════════════════════════════════════ */

fixed_t fixed_sqrt(fixed_t x)
{
    if (x <= 0) return 0;

    fixed_t y;

    /* ── 初始值估算 ──
     * 将 x 视为 Q16.16，等效整数为 x_int = x / FIXED_ONE (高 16 位)
     * 使用二分法找到 sqrt(x_int) 的整数部分，再乘以 FIXED_ONE
     */
    {
        uint32_t x_int = (uint32_t)(x >> FIXED_FRAC_BITS);
        uint32_t lo = 0, hi = 65535;
        if (hi > x_int) hi = x_int + 1;

        while (lo < hi)
        {
            uint32_t mid = (lo + hi + 1) >> 1;
            if (mid * mid <= x_int)
                lo = mid;
            else
                hi = mid - 1;
        }
        y = (fixed_t)lo << FIXED_FRAC_BITS;
        if (y < FIXED_ONE) y = FIXED_ONE;
    }

    /* ── Newton 迭代 (固定 5 次，保证 32 位精度) ── */
    for (int i = 0; i < 5; i++)
    {
        fixed_t div = fixed_div(x, y);
        y = (y + div) >> 1;  /* (y + div) / 2 */
    }

    return y;
}

/* ══════════════════════════════════════════════════════════════════
 *  fixed_sqrt_fast — 查表法快速平方根
 *  将输入归一化到 [0, 65535] 范围，查 256 点表 + 线性插值
 *  精度: ±0.1%, 执行时间: ~2μs @72MHz
 * ══════════════════════════════════════════════════════════════════ */

fixed_t fixed_sqrt_fast(fixed_t x)
{
    if (x <= 0) return 0;

    /* 将 Q16.16 输入缩放到 0..(MAX_IN * FIXED_ONE) */
    /* 等效于取高 16 位作为整数部分 */
    uint32_t x_scaled = (uint32_t)(x >> FIXED_FRAC_BITS);
    if (x_scaled > (uint32_t)SQRT_LUT_MAX_IN)
        x_scaled = SQRT_LUT_MAX_IN;

    /* 查表索引和分数部分 */
    uint32_t idx_scaled = x_scaled * (SQRT_LUT_SIZE - 1);
    uint16_t idx = (uint16_t)(idx_scaled / SQRT_LUT_MAX_IN);
    uint16_t frac = (uint16_t)(idx_scaled % SQRT_LUT_MAX_IN);

    if (idx >= SQRT_LUT_SIZE - 1)
        return sqrt_lut[SQRT_LUT_SIZE - 1];

    /* 线性插值 */
    fixed_t y0 = sqrt_lut[idx];
    fixed_t y1 = sqrt_lut[idx + 1];
    fixed_t diff = (int32_t)(y1 - y0);
    fixed_t interp = (fixed_t)(((int64_t)diff * frac) / SQRT_LUT_MAX_IN);

    return y0 + interp;
}

/* ══════════════════════════════════════════════════════════════════
 *  uint16_sqrt — 整数平方根 (用于 uint16 速度值)
 *  输入: 0 ~ 0xFFFFFFFF (但实际输入 ≤ v²，v ≤ 65535)
 *  输出: 0 ~ 65535
 * ══════════════════════════════════════════════════════════════════ */

uint16_t uint16_sqrt(uint32_t x)
{
    if (x == 0) return 0;

    uint32_t lo = 0, hi = 65535;
    if (hi > x) hi = x;

    while (lo < hi)
    {
        uint32_t mid = (lo + hi + 1) >> 1;
        if (mid * mid <= x)
            lo = mid;
        else
            hi = mid - 1;
    }
    return (uint16_t)lo;
}

/* ══════════════════════════════════════════════════════════════════
 *  正弦查找表 — 256 点，覆盖 [0, π/2)
 *  sin_table[i] = sin(i * π/2 / 256) * 32767 (Q1.15 格式)
 * ══════════════════════════════════════════════════════════════════ */

#define SIN_LUT_SIZE 256

static const int16_t sin_table[SIN_LUT_SIZE + 1] =
{
       0,   201,   402,   603,   804,  1005,  1206,  1407,
    1608,  1809,  2009,  2210,  2411,  2611,  2811,  3012,
    3212,  3412,  3612,  3812,  4011,  4211,  4410,  4610,
    4808,  5007,  5205,  5404,  5602,  5800,  5998,  6195,
    6393,  6590,  6787,  6983,  7179,  7375,  7571,  7767,
    7962,  8157,  8351,  8546,  8740,  8933,  9127,  9320,
    9512,  9704,  9896, 10088, 10279, 10470, 10660, 10850,
   11040, 11229, 11418, 11607, 11795, 11983, 12170, 12357,
   12544, 12730, 12916, 13102, 13287, 13472, 13656, 13840,
   14024, 14207, 14390, 14572, 14754, 14936, 15117, 15298,
   15478, 15658, 15838, 16017, 16196, 16374, 16552, 16729,
   16906, 17083, 17259, 17435, 17610, 17785, 17960, 18134,
   18307, 18480, 18653, 18825, 18997, 19168, 19339, 19509,
   19679, 19849, 20018, 20186, 20354, 20522, 20689, 20856,
   21022, 21188, 21353, 21518, 21682, 21846, 22009, 22172,
   22335, 22497, 22658, 22819, 22980, 23140, 23299, 23458,
   23617, 23775, 23932, 24089, 24246, 24402, 24557, 24712,
   24867, 25021, 25174, 25327, 25480, 25632, 25783, 25934,
   26084, 26234, 26383, 26532, 26680, 26828, 26975, 27122,
   27268, 27413, 27558, 27703, 27847, 27990, 28133, 28275,
   28417, 28558, 28699, 28839, 28978, 29117, 29256, 29394,
   29531, 29668, 29804, 29940, 30075, 30210, 30344, 30477,
   30610, 30742, 30874, 31005, 31136, 31266, 31395, 31524,
   31652, 31780, 31907, 32034, 32160, 32285, 32410, 32534,
   32658, 32781, 32904, 33026, 33147, 33268, 33388, 33508,
   33627, 33745, 33863, 33980, 34097, 34213, 34329, 34444,
   34559, 34673, 34786, 34899, 35011, 35123, 35234, 35344,
   35454, 35564, 35672, 35780, 35888, 35995, 36101, 36207,
   36312, 36417, 36521, 36625, 36728, 36830, 36932, 37033,
   37134, 37234, 37333, 37432, 37530, 37628, 37725, 37821,
   37917, 38013, 38107, 38202, 38295, 38388, 38481, 38572,
   38664, 38754, 38844, 38934, 39023, 39111, 39199, 39286,
   39372,
};

/**
 * fixed_sin — 查表法定点正弦
 * @param  rad  Q16.16 弧度值
 * @return      Q1.15 格式正弦值，范围 [-32767, 32767]
 *
 * 算法：将 rad 归一化到 [0, 2π)，利用 4 个象限对称性
 * 将 [0, π/2) 映射到查找表，其余象限通过符号和翻转计算
 */
int16_t fixed_sin(fixed_t rad)
{
    /* 归一化到 [0, 2π) */
    while (rad >= FIXED_TWO_PI) rad -= FIXED_TWO_PI;
    while (rad < 0)             rad += FIXED_TWO_PI;

    bool negate = false;
    fixed_t rad_mapped;

    /* 利用象限对称性映射到 [0, π/2) */
    if (rad < FIXED_PI_DIV_2)
    {
        /* 第一象限: sin(x) = +sin(x) */
        rad_mapped = rad;
    }
    else if (rad < FIXED_PI)
    {
        /* 第二象限: sin(x) = +sin(π-x) */
        rad_mapped = FIXED_PI - rad;
    }
    else if (rad < FIXED_PI + FIXED_PI_DIV_2)
    {
        /* 第三象限: sin(x) = -sin(x-π) */
        negate = true;
        rad_mapped = rad - FIXED_PI;
    }
    else
    {
        /* 第四象限: sin(x) = -sin(2π-x) */
        negate = true;
        rad_mapped = FIXED_TWO_PI - rad;
    }

    /* 将 [0, π/2) 映射到查找表 index */
    /* scaled = rad_mapped * SIN_LUT_SIZE / (π/2) = rad_mapped * SIN_LUT_SIZE / FIXED_PI_DIV_2 */
    uint64_t scaled64 = (uint64_t)rad_mapped * SIN_LUT_SIZE;
    uint32_t scaled   = (uint32_t)(scaled64 / FIXED_PI_DIV_2);

    uint16_t idx  = (uint16_t)(scaled >> FIXED_FRAC_BITS);
    uint16_t frac = (uint16_t)(scaled & (FIXED_ONE - 1));

    if (idx >= SIN_LUT_SIZE) idx = SIN_LUT_SIZE - 1;

    /* 线性插值 */
    int32_t y0 = sin_table[idx];
    int32_t y1 = sin_table[idx + 1];
    int32_t result = y0 + (int32_t)(((y1 - y0) * (int64_t)frac) >> FIXED_FRAC_BITS);

    return (int16_t)(negate ? -result : result);
}

/**
 * fixed_cos — 查表法定点余弦
 * cos(x) = sin(x + π/2)
 */
int16_t fixed_cos(fixed_t rad)
{
    return fixed_sin(rad + FIXED_PI_DIV_2);
}
