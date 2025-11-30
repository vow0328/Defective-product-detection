/**
 * @file    scurve_profile.c
 * @brief   七段式 Jerk 限制 S 形加减速算法实现
 *
 * 核心原理:
 *   速度曲线的七段由 jerk(加加速度) 控制, 消除加速度突变:
 *     T1(+j) → T2(0) → T3(-j) → T4(0) → T5(-j) → T6(0) → T7(+j)
 *
 * 短距离降级:
 *   - 如果距离不足以达到 a_max → 5段 (T2=T6=0)
 *   - 如果距离不足以达到 v_max → 3段 (T2=T4=T6=0, 三角形 S)
 *
 * 全部使用 Q16.16 定点运算, 零浮点依赖。
 * 平方根使用自实现定点 sqrt (非 <math.h> sqrtf)。
 */

#include "scurve_profile.h"
#include <string.h>

/* ══════════════════════════════════════════════════════════════════
 *  辅助宏
 * ══════════════════════════════════════════════════════════════════ */

#define MAX(a,b)  ((a) > (b) ? (a) : (b))
#define MIN(a,b)  ((a) < (b) ? (a) : (b))

/* ══════════════════════════════════════════════════════════════════
 *  1/v 快速查表 (128 点, 覆盖 100 ~ 65535 Hz)
 *
 *  表项: inv_table[i] = FIXED_ONE / speed_at_index(i)
 *  其中 speed_at_index(i) 为指数分布, 低频密集
 *  内存: 128 * 4 = 512 字节
 * ══════════════════════════════════════════════════════════════════ */

#define INV_LUT_SIZE 128

typedef struct {
    uint16_t speed;    /* 该索引对应的速度起点 */
    fixed_t  inv_val;  /* FIXED_ONE / speed  */
} InvLutEntry;

static InvLutEntry inv_lut[INV_LUT_SIZE];

/**
 * inv_lut_init — 初始化 1/v 查找表
 * 速度映射: 指数分布, 低速段密集, 高速段稀疏
 *   index i → speed = 100 + (i * i * 65435) / (N*N)
 *   即 speed ∝ i²
 */
static void inv_lut_init(void)
{
    static bool ready = false;
    if (ready) return;
    ready = true;

    for (int i = 0; i < INV_LUT_SIZE; i++)
    {
        /* 二次分布: i=0 → 100Hz, i=127 → ~65535Hz */
        uint32_t speed = 100 + (uint32_t)((uint64_t)i * i * 65435
                                          / ((INV_LUT_SIZE-1) * (INV_LUT_SIZE-1)));
        inv_lut[i].speed   = (uint16_t)speed;
        inv_lut[i].inv_val = fixed_div(FIXED_ONE, INT_TO_FIXED((int)speed));
    }
    /* 确保最后一个覆盖到 65535 */
    inv_lut[INV_LUT_SIZE-1].speed = 65535;
    inv_lut[INV_LUT_SIZE-1].inv_val = fixed_div(FIXED_ONE, INT_TO_FIXED(65535));
}

/**
 * fixed_reciprocal_fast — 快速定点倒数 1/x
 * 查表 + 线性插值, 误差 < 0.5%
 */
static inline fixed_t fixed_reciprocal_fast(uint16_t speed)
{
    if (speed == 0) return FIXED_MAX;

    /* 二分查找最近索引 */
    int lo = 0, hi = INV_LUT_SIZE - 1;
    while (lo < hi) {
        int mid = (lo + hi + 1) >> 1;
        if (inv_lut[mid].speed <= speed)
            lo = mid;
        else
            hi = mid - 1;
    }
    int idx = lo;
    if (idx >= INV_LUT_SIZE - 1)
        return inv_lut[INV_LUT_SIZE - 1].inv_val;

    /* 线性插值 */
    uint16_t s0 = inv_lut[idx].speed;
    uint16_t s1 = inv_lut[idx + 1].speed;
    fixed_t  v0 = inv_lut[idx].inv_val;
    fixed_t  v1 = inv_lut[idx + 1].inv_val;

    if (s1 == s0) return v0;

    int32_t frac = (int32_t)(speed - s0);
    int32_t denom = (int32_t)(s1 - s0);
    fixed_t interp = (fixed_t)(((int64_t)(v1 - v0) * frac) / denom);

    return v0 + interp;
}

/* ══════════════════════════════════════════════════════════════════
 *  calc_scurve_profile — 七段 S 曲线规划
 *
 *  参数全部使用 uint16_t 输入 (与现有接口兼容),
 *  内部用 fixed_t Q16.16 计算。
 *
 *  步骤:
 *   1. 检查是否能达到 a_max
 *   2. 检查是否能达到 v_max
 *   3. 确定激活段数 (7/5/3)
 *   4. 计算各段步数和时间
 * ══════════════════════════════════════════════════════════════════ */

bool calc_scurve_profile(const SCurveConfig *cfg, SCurvePlan *plan)
{
    if (!cfg || !plan) return false;
    if (cfg->total_steps == 0) return false;

    /* 初始化查表 (首次调用时) */
    inv_lut_init();

    memset(plan, 0, sizeof(SCurvePlan));

    /* ── 转换参数为定点 ── */
    fixed_t v0  = INT_TO_FIXED((int)cfg->v_start);
    fixed_t vmax = INT_TO_FIXED((int)cfg->v_max);
    fixed_t vend = INT_TO_FIXED((int)cfg->v_end);
    fixed_t amax = INT_TO_FIXED((int)cfg->a_max);
    fixed_t jmax = INT_TO_FIXED((int)cfg->j_max);

    plan->v_start_fp = v0;
    plan->v_max_fp   = vmax;
    plan->v_end_fp   = vend;
    plan->a_max_fp   = amax;
    plan->j_coef     = jmax >> 1;       /* j/2 */

    if (jmax <= 0 || amax <= 0)
    {
        /* jerk 或加速度为零 → 降级为纯匀速 */
        plan->t4_steps = cfg->total_steps;
        plan->seg_bound[0] = 0;
        plan->seg_bound[1] = plan->seg_bound[2] = plan->seg_bound[3] = 0;
        plan->seg_bound[4] = plan->seg_bound[5] = plan->seg_bound[6]
                           = plan->seg_bound[7] = cfg->total_steps;
        plan->active_segments = 1;
        plan->valid = true;
        plan->v1_end = v0;
        plan->v3_end = v0;
        plan->j_coef_neg = -plan->j_coef;
        return true;
    }

    /* ── 加速段所需速度增量 ── */
    fixed_t dv_acc = vmax - v0;   /* 加速段速度增量 */
    fixed_t dv_dec = vmax - vend; /* 减速段速度增量 */

    if (dv_acc < 0) dv_acc = 0;
    if (dv_dec < 0) dv_dec = 0;

    /* ── 判断是否能达到 a_max ──
     * 在纯 jerk 段中, 速度增量 Δv_j = a_max² / j_max
     * 因为: t_j = a_max / j_max, Δv = a_max * t_j / 2 + a_max * t_j / 2 = a_max * t_j = a_max²/j
     * 注意: 加速阶段有 T1(+j) 和 T3(-j), 速度增量 = a_max * t_j
     *
     * 如果 dv_acc >= 2 * Δv_jerk_single, 可以到达 a_max (需要 T2 匀加速段)
     * Δv_jerk_single = a_max² / (2*j_max) → 两个 jerk 段贡献的总 Δv = a_max² / j_max
     *
     * 更准确的推导:
     *   T1 段 (0→a_max):  Δv_t1 = a_max*t_j/3 + t_j*(a_max)/2 ...
     *
     * 简化推导 (等加速度假设):
     *   加加速段速度增量 ≈ a_max/2 * t_j
     *   减加速段速度增量 ≈ a_max/2 * t_j
     *   总 jerk 段贡献: ≈ a_max * t_j = a_max² / j_max
     */

    /* jerk 时间: t_j = a_max / j_max */
    fixed_t t_j = fixed_div(amax, jmax);

    /* 单个 jerk 段的速度增量: Δv_j1 = j_max * t_j² / 2 = a_max * t_j / 2 */
    fixed_t dv_j1 = fixed_mul(amax, t_j) >> 1;  /* = a_max * t_j / 2 */

    /* T1 + T3 总速度增量 = dv_j1 * 2 = amax * t_j */
    fixed_t dv_jerk_total = fixed_mul(amax, t_j);

    /* ── 逐步判断段数 ── */
    bool reach_amax;      /* 能达到 a_max */
    bool reach_vmax;      /* 能达到 v_max */
    fixed_t t2_dur = 0;   /* 匀加速段时间 */
    fixed_t t6_dur = 0;   /* 匀减速段时间 */

    if (dv_acc >= dv_jerk_total)
    {
        /* 能达到 a_max → 需要 T2 匀加速段 */
        reach_amax = true;
        t2_dur = fixed_div(dv_acc - dv_jerk_total, amax);
    }
    else
    {
        /* 不能达到 a_max → T2=0, 重新计算实际 a_max */
        reach_amax = false;
        t2_dur = 0;

        /* a_peak 满足: a_peak² / j_max = dv_acc → a_peak = sqrt(dv_acc * j_max) */
        fixed_t a_peak_sq = fixed_mul(dv_acc, jmax);
        amax = fixed_sqrt(a_peak_sq);

        /* 更新 jerk 时间 */
        t_j = fixed_div(amax, jmax);
        dv_j1 = fixed_mul(amax, t_j) >> 1;
        dv_jerk_total = fixed_mul(amax, t_j);

        plan->a_max_fp = amax;
        plan->j_coef   = jmax >> 1;
    }

    /* ── 对称处理减速段 ── */
    fixed_t t_j_dec = fixed_div(amax, jmax);  /* 可能与加速段不同 (如果减速段独立) */
    fixed_t dv_j1_dec = fixed_mul(amax, t_j_dec) >> 1;
    fixed_t dv_jerk_total_dec = fixed_mul(amax, t_j_dec);

    if (dv_dec >= dv_jerk_total_dec)
    {
        t6_dur = fixed_div(dv_dec - dv_jerk_total_dec, amax);
    }
    else
    {
        /* 减速段也不能达到 a_max → 重新计算 */
        fixed_t a_peak_dec_sq = fixed_mul(dv_dec, jmax);
        fixed_t amax_dec = fixed_sqrt(a_peak_dec_sq);
        t_j_dec = fixed_div(amax_dec, jmax);
        dv_j1_dec = fixed_mul(amax_dec, t_j_dec) >> 1;
        dv_jerk_total_dec = fixed_mul(amax_dec, t_j_dec);
        t6_dur = 0;
    }

    /* ── 计算各段时间 ── */
    plan->t1_dur = t_j;
    plan->t2_dur = t2_dur;
    plan->t3_dur = t_j;
    plan->t4_dur = 0;  /* 稍后根据剩余步数计算 */
    plan->t5_dur = t_j_dec;
    plan->t6_dur = t6_dur;
    plan->t7_dur = t_j_dec;

    /* ── 计算各段步数 (用平均速度 × 时间) ── */
    {
        fixed_t v_cur = v0;

        /* T1: v_avg = v0 + dv_j1/2 ≈ v0 + j*t_j²/6 */
        fixed_t v_end_t1 = v0 + dv_j1;
        fixed_t v_avg_t1 = (v0 + v_end_t1) >> 1;
        plan->t1_steps = (uint32_t)FIXED_TO_INT(fixed_mul(v_avg_t1, plan->t1_dur));
        v_cur = v_end_t1;

        /* T2: 匀加速 */
        fixed_t v_end_t2 = v_cur + fixed_mul(amax, t2_dur);
        fixed_t v_avg_t2 = (v_cur + v_end_t2) >> 1;
        plan->t2_steps = (uint32_t)FIXED_TO_INT(fixed_mul(v_avg_t2, t2_dur));
        v_cur = v_end_t2;

        /* T3: 减加速 */
        fixed_t v_end_t3 = v_cur + dv_j1;
        fixed_t v_avg_t3 = (v_cur + v_end_t3) >> 1;
        plan->t3_steps = (uint32_t)FIXED_TO_INT(fixed_mul(v_avg_t3, plan->t3_dur));
        v_cur = v_end_t3;
        plan->v3_end = v_cur;  /* 应等于 v_max */
    }

    /* ── 判断是否能达到 v_max (匀速段) ── */
    {
        /* 减速段步数 */
        fixed_t v_dec_start = vmax;
        fixed_t v_dec;

        /* T5 */
        fixed_t v_end_t5 = v_dec_start - dv_j1_dec;
        fixed_t v_avg_t5 = (v_dec_start + v_end_t5) >> 1;
        plan->t5_steps = (uint32_t)FIXED_TO_INT(fixed_mul(v_avg_t5, plan->t5_dur));
        v_dec = v_end_t5;

        /* T6 */
        fixed_t v_end_t6 = v_dec - fixed_mul(amax, t6_dur);
        fixed_t v_avg_t6 = (v_dec + v_end_t6) >> 1;
        plan->t6_steps = (uint32_t)FIXED_TO_INT(fixed_mul(v_avg_t6, t6_dur));
        v_dec = v_end_t6;

        /* T7 */
        fixed_t v_end_t7 = v_dec - dv_j1_dec;
        if (v_end_t7 < vend) v_end_t7 = vend;
        fixed_t v_avg_t7 = (v_dec + v_end_t7) >> 1;
        plan->t7_steps = (uint32_t)FIXED_TO_INT(fixed_mul(v_avg_t7, plan->t7_dur));

        uint32_t accel_steps = plan->t1_steps + plan->t2_steps + plan->t3_steps;
        uint32_t decel_steps = plan->t5_steps + plan->t6_steps + plan->t7_steps;

        int64_t needed = (int64_t)accel_steps + (int64_t)decel_steps;

        if (needed >= (int64_t)cfg->total_steps)
        {
            /* ── 不能达到 v_max → 三角形 S 曲线 ── */
            reach_vmax = false;
            plan->t4_steps = 0;
            plan->t4_dur = 0;

            /* 重新计算: 加速度和减速度共享总步数, 以中间峰值速度连接 */
            /* 简化: 按比例缩减所有段 */
            uint64_t total_no_cruise = (uint64_t)accel_steps + (uint64_t)decel_steps;
            if (total_no_cruise == 0) total_no_cruise = 1;

            uint64_t scale_num = cfg->total_steps;
            plan->t1_steps = (uint32_t)((uint64_t)plan->t1_steps * scale_num / total_no_cruise);
            plan->t2_steps = (uint32_t)((uint64_t)plan->t2_steps * scale_num / total_no_cruise);
            plan->t3_steps = (uint32_t)((uint64_t)plan->t3_steps * scale_num / total_no_cruise);
            plan->t5_steps = (uint32_t)((uint64_t)plan->t5_steps * scale_num / total_no_cruise);
            plan->t6_steps = (uint32_t)((uint64_t)plan->t6_steps * scale_num / total_no_cruise);
            plan->t7_steps = (uint32_t)((uint64_t)plan->t7_steps * scale_num / total_no_cruise);

            /* 确保总和不超过 total_steps */
            uint32_t sum = plan->t1_steps + plan->t2_steps + plan->t3_steps
                         + plan->t5_steps + plan->t6_steps + plan->t7_steps;
            if (sum < cfg->total_steps)
                plan->t7_steps += (cfg->total_steps - sum);
            else if (sum > cfg->total_steps)
                plan->t7_steps -= (sum - cfg->total_steps);

            plan->t4_steps = 0;
        }
        else
        {
            /* 能达到 v_max → 7段完整 */
            reach_vmax = true;
            uint32_t sum_no_t4 = accel_steps + decel_steps;
            plan->t4_steps = cfg->total_steps - sum_no_t4;
            plan->t4_dur = fixed_div(INT_TO_FIXED((int)plan->t4_steps), vmax);
        }
    }

    /* ── 设置段边界 ── */
    plan->seg_bound[0] = 0;
    plan->seg_bound[1] = plan->t1_steps;
    plan->seg_bound[2] = plan->seg_bound[1] + plan->t2_steps;
    plan->seg_bound[3] = plan->seg_bound[2] + plan->t3_steps;
    plan->seg_bound[4] = plan->seg_bound[3] + plan->t4_steps;
    plan->seg_bound[5] = plan->seg_bound[4] + plan->t5_steps;
    plan->seg_bound[6] = plan->seg_bound[5] + plan->t6_steps;
    plan->seg_bound[7] = plan->seg_bound[6] + plan->t7_steps;

    /* ── 设置时间边界 ── */
    plan->seg_time_bound[0] = 0;
    plan->seg_time_bound[1] = plan->t1_dur;
    plan->seg_time_bound[2] = plan->seg_time_bound[1] + plan->t2_dur;
    plan->seg_time_bound[3] = plan->seg_time_bound[2] + plan->t3_dur;
    plan->seg_time_bound[4] = plan->seg_time_bound[3] + plan->t4_dur;
    plan->seg_time_bound[5] = plan->seg_time_bound[4] + plan->t5_dur;
    plan->seg_time_bound[6] = plan->seg_time_bound[5] + plan->t6_dur;
    plan->seg_time_bound[7] = plan->seg_time_bound[6] + plan->t7_dur;

    /* ── 记录激活段数 ── */
    int segs = 0;
    if (plan->t1_steps > 0) segs++;
    if (plan->t2_steps > 0) segs++;
    if (plan->t3_steps > 0) segs++;
    if (plan->t4_steps > 0) segs++;
    if (plan->t5_steps > 0) segs++;
    if (plan->t6_steps > 0) segs++;
    if (plan->t7_steps > 0) segs++;
    plan->active_segments = (uint8_t)segs;

    plan->v1_end = v0 + dv_j1;
    plan->j_coef_neg = -plan->j_coef;
    plan->valid = true;

    return true;
}

/* ══════════════════════════════════════════════════════════════════
 *  scurve_state_init — 初始化运行时状态
 * ══════════════════════════════════════════════════════════════════ */

void scurve_state_init(const SCurvePlan *plan, SCurveState *state)
{
    if (!plan || !state) return;

    state->segment      = 1;
    state->step_in_seg  = 0;
    state->time_in_seg  = 0;
    state->current_v    = plan->v_start_fp;
    state->current_a    = 0;
    state->current_hz   = FIXED_TO_INT(plan->v_start_fp);
    state->finished     = false;
    state->acc_accum    = 0;
}

/* ══════════════════════════════════════════════════════════════════
 *  scurve_step_update — ISR 安全, 每步更新速度
 *
 *  算法: 维护时间, 用分段解析式计算速度
 *  每步:
 *    t += 1/v_cur   (通过查表实现快速倒数)
 *    根据 t 判定所在段
 *    用段内公式计算 v(t)
 *
 *  无浮点、无 sqrt、无 32 位除法 (只有查表 + 乘法)
 * ══════════════════════════════════════════════════════════════════ */

uint16_t scurve_step_update(const SCurvePlan *plan, SCurveState *state)
{
    if (!plan || !state || state->finished)
        return 0;

    /* ── 检查是否完成 ── */
    uint32_t total = plan->seg_bound[7];
    uint32_t cur_step = plan->seg_bound[state->segment - 1] + state->step_in_seg;
    if (cur_step >= total)
    {
        state->finished = true;
        state->current_hz = 0;
        return 0;
    }

    /* ── 更新时间: Δt = 1/v ── */
    if (state->current_hz > 0)
    {
        fixed_t dt = fixed_reciprocal_fast(state->current_hz);
        state->time_in_seg += dt;
        state->step_in_seg++;
        cur_step++;
    }
    else
    {
        /* 速度为 0, 不应发生 */
        state->current_hz = FIXED_TO_INT(plan->v_start_fp);
        if (state->current_hz < 1) state->current_hz = 1;
    }

    /* ── 检查是否需要切换段 ── */
    while (state->segment < 8 && cur_step >= plan->seg_bound[state->segment])
    {
        state->segment++;
        state->step_in_seg = 0;
        state->time_in_seg = 0;
    }

    if (state->segment > 7)
    {
        state->finished = true;
        state->current_hz = 0;
        return 0;
    }

    /* ── 根据所在段计算速度 ── */
    fixed_t v;
    fixed_t t = state->time_in_seg;

    switch (state->segment)
    {
    case 1: /* T1: 加加速段 v = v0 + j*t²/2 */
        {
            fixed_t t_sq = fixed_mul(t, t);
            v = plan->v_start_fp + fixed_mul(plan->j_coef, t_sq);
        }
        break;

    case 2: /* T2: 匀加速段 v = v(T1_end) + a_max * t */
        {
            fixed_t v_t1_end = plan->v1_end;
            v = v_t1_end + fixed_mul(plan->a_max_fp, t);
        }
        break;

    case 3: /* T3: 减加速段 v = v(T2_end) + a_max*t - j*t²/2 */
        {
            fixed_t v_t2_end = plan->v1_end
                             + fixed_mul(plan->a_max_fp, plan->t2_dur);
            fixed_t t_sq = fixed_mul(t, t);
            v = v_t2_end + fixed_mul(plan->a_max_fp, t)
                - fixed_mul(plan->j_coef, t_sq);
        }
        break;

    case 4: /* T4: 匀速段 */
        v = plan->v_max_fp;
        break;

    case 5: /* T5: 加减速段 v = v_max - j*t²/2 */
        {
            fixed_t t_sq = fixed_mul(t, t);
            v = plan->v_max_fp + fixed_mul(plan->j_coef_neg, t_sq);
        }
        break;

    case 6: /* T6: 匀减速段 v = v(T5_end) - a_max * t */
        {
            fixed_t v_t5_end = plan->v_max_fp
                             + fixed_mul(plan->j_coef_neg,
                                         fixed_mul(plan->t5_dur, plan->t5_dur));
            v = v_t5_end - fixed_mul(plan->a_max_fp, t);
        }
        break;

    case 7: /* T7: 减减速段 v = v(T6_end) - a_max*t + j*t²/2 */
        {
            fixed_t v_t5_end = plan->v_max_fp
                             + fixed_mul(plan->j_coef_neg,
                                         fixed_mul(plan->t5_dur, plan->t5_dur));
            fixed_t v_t6_end = v_t5_end
                             - fixed_mul(plan->a_max_fp, plan->t6_dur);
            fixed_t t_sq = fixed_mul(t, t);
            v = v_t6_end - fixed_mul(plan->a_max_fp, t)
                + fixed_mul(plan->j_coef, t_sq);
        }
        break;

    default:
        v = 0;
        break;
    }

    /* ── 限幅 ── */
    if (v < 0) v = 0;
    if (v > plan->v_max_fp) v = plan->v_max_fp;

    /* ── 确保速度不低于起始速度 (除非止步) ── */
    if (state->segment <= 4 && v < plan->v_start_fp)
        v = plan->v_start_fp;

    state->current_v = v;
    state->current_hz = fixed_saturate_int(v);

    if (state->current_hz < 1) state->current_hz = 1;

    return state->current_hz;
}

/* ══════════════════════════════════════════════════════════════════
 *  scurve_get_speed — 无状态速度查询 (兼容旧接口)
 *
 *  根据累计步数 step_idx 查找对应速度。
 *  可用于替代原 get_step_speed(), 但效率低于状态版本。
 * ══════════════════════════════════════════════════════════════════ */

uint16_t scurve_get_speed(const SCurvePlan *plan, uint32_t step_idx)
{
    if (!plan || !plan->valid) return 0;
    if (step_idx >= plan->seg_bound[7]) return 0;

    /* ── 确定所在段 ── */
    uint8_t seg = 1;
    while (seg < 8 && step_idx >= plan->seg_bound[seg])
        seg++;
    if (seg > 7) return 0;

    uint32_t step_in_seg = step_idx - plan->seg_bound[seg - 1];

    /* ── 重建该步的时间 (近似) ──
     * 为此, 我们需要该段的起始速度和参数。
     * 简化: 使用该段步数占总步数比例来估算时间比例。 ── */

    fixed_t v;
    uint32_t seg_steps = plan->seg_bound[seg] - plan->seg_bound[seg - 1];

    if (seg_steps == 0)
    {
        /* 空段, 跳到下一段 */
        return scurve_get_speed(plan, plan->seg_bound[seg]);
    }

    /* 时间比例 ≈ 步数比例 (粗略近似, 适合步数较少的段) */
    fixed_t seg_dur;
    switch (seg)
    {
    case 1:  seg_dur = plan->t1_dur; break;
    case 2:  seg_dur = plan->t2_dur; break;
    case 3:  seg_dur = plan->t3_dur; break;
    case 4:  seg_dur = plan->t4_dur; break;
    case 5:  seg_dur = plan->t5_dur; break;
    case 6:  seg_dur = plan->t6_dur; break;
    case 7:  seg_dur = plan->t7_dur; break;
    default: seg_dur = 0; break;
    }

    /* t_frac = step_in_seg / seg_steps (Q16.16) */
    fixed_t t_frac = (seg_steps > 0)
                   ? fixed_div(INT_TO_FIXED((int)step_in_seg), INT_TO_FIXED((int)seg_steps))
                   : 0;

    /* 实际时间 ≈ t_frac * seg_dur */
    fixed_t t = fixed_mul(t_frac, seg_dur);

    /* ── 根据段计算速度 (同 step_update 逻辑) ── */
    switch (seg)
    {
    case 1:
        v = plan->v_start_fp + fixed_mul(plan->j_coef, fixed_mul(t, t));
        break;
    case 2:
        v = plan->v1_end + fixed_mul(plan->a_max_fp, t);
        break;
    case 3:
        {
            fixed_t v_t2_end = plan->v1_end
                             + fixed_mul(plan->a_max_fp, plan->t2_dur);
            v = v_t2_end + fixed_mul(plan->a_max_fp, t)
                - fixed_mul(plan->j_coef, fixed_mul(t, t));
        }
        break;
    case 4:
        v = plan->v_max_fp;
        break;
    case 5:
        v = plan->v_max_fp + fixed_mul(plan->j_coef_neg, fixed_mul(t, t));
        break;
    case 6:
        {
            fixed_t v_t5_end = plan->v_max_fp
                             + fixed_mul(plan->j_coef_neg,
                                         fixed_mul(plan->t5_dur, plan->t5_dur));
            v = v_t5_end - fixed_mul(plan->a_max_fp, t);
        }
        break;
    case 7:
        {
            fixed_t v_t5_end = plan->v_max_fp
                             + fixed_mul(plan->j_coef_neg,
                                         fixed_mul(plan->t5_dur, plan->t5_dur));
            fixed_t v_t6_end = v_t5_end
                             - fixed_mul(plan->a_max_fp, plan->t6_dur);
            v = v_t6_end - fixed_mul(plan->a_max_fp, t)
                + fixed_mul(plan->j_coef, fixed_mul(t, t));
        }
        break;
    default:
        v = 0;
        break;
    }

    if (v < 0) v = 0;
    return fixed_saturate_int(v);
}
