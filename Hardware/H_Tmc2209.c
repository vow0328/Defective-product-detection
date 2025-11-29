/**
 * @file    H_Tmc2209.c
 * @brief   步进电机控制 (TMC2209 驱动)
 *
 * 支持三种运动模式:
 *   Constant_speed (1): 恒速
 *   Constant_step  (2): 梯形加减速定步 (兼容旧协议)
 *   SCurve_step    (4): 七段 S 曲线定步 (新协议)
 *   STOP_mode      (3): 停止
 *
 * 特性:
 *   - 采用 Q16.16 定点数替代浮点 sqrtf, ISR 中零浮点运算
 *   - 集成前馈式力矩补偿, 根据速度动态调节 PWM 占空比
 *   - 最高 40kHz 6 轴并行脉冲输出
 */

#include "include.h"

/* ══════════════════════════════════════════════════════════════════
 *  全局变量
 * ══════════════════════════════════════════════════════════════════ */

MotorStruct Motor[10] = {0};

/* ══════════════════════════════════════════════════════════════════
 *  stepper_init — 梯形加减速初始化 (兼容旧接口)
 * ══════════════════════════════════════════════════════════════════ */

void stepper_init(MotorStruct *motor, uint16_t v_start, uint16_t v_max,
                  uint16_t acc, uint16_t steps)
{
    motor->velocity.v_start     = v_start;
    motor->velocity.v_max       = v_max;
    motor->velocity.acc         = acc;
    motor->velocity.total_steps = motor->target_step = steps;
    motor->steps                = calc_trapezoid_profile(motor->velocity);
    motor->current_step         = 0;
    motor->profile_type         = PROFILE_TRAPEZOID;
}

/* ══════════════════════════════════════════════════════════════════
 *  scurve_stepper_init — S 曲线初始化
 * ══════════════════════════════════════════════════════════════════ */

static void scurve_stepper_init(MotorStruct *motor, uint16_t v_start,
                                uint16_t v_max, uint16_t v_end,
                                uint16_t a_max, uint16_t j_max,
                                uint16_t steps)
{
    motor->scurve_cfg.v_start     = v_start;
    motor->scurve_cfg.v_max       = v_max;
    motor->scurve_cfg.v_end       = v_end;
    motor->scurve_cfg.a_max       = a_max;
    motor->scurve_cfg.j_max       = j_max;
    motor->scurve_cfg.total_steps = steps;

    motor->target_step = steps;
    motor->current_step = 0;
    motor->profile_type = PROFILE_SCURVE;

    if (!calc_scurve_profile(&motor->scurve_cfg, &motor->scurve_plan))
    {
        /* S 曲线规划失败 → 降级为梯形 */
        stepper_init(motor, v_start, v_max, a_max, steps);
        return;
    }

    scurve_state_init(&motor->scurve_plan, &motor->scurve_state);
    motor->hz = motor->scurve_state.current_hz;
}

/* ══════════════════════════════════════════════════════════════════
 *  Motor_Set — 设置电机运行参数
 *
 *  参数:
 *    num    : 电机编号 (1~6)
 *    mode   : 运动模式 (1=恒速, 2=梯形定步, 3=停止, 4=S曲线定步)
 *    dir    : 方向
 *    hz     : 恒速模式=速度, 定步模式=步数
 *    vstart : 起始速度 (steps/s)
 *    vmax   : 最大速度 (steps/s)
 *    vacc   : 加速度 (steps/s²) / 或 jerk (steps/s³) 取决于模式
 * ══════════════════════════════════════════════════════════════════ */

void Motor_Set(uint8_t num, uint8_t mode, GPIO_PinState dir, uint16_t hz,
               uint16_t vstart, uint16_t vmax, uint16_t vacc)
{
    Motor[num].en = ENABLE;

    switch (mode)
    {
    case Constant_speed:
        Motor[num].mode = Constant_speed;
        Motor[num].hz   = hz;
        break;

    case Constant_step:
        Motor[num].mode = Constant_step;
        stepper_init(&Motor[num], vstart, vmax, vacc, hz);
        Motor[num].hz = get_step_speed(Motor[num].current_step,
                                       Motor[num].steps,
                                       Motor[num].velocity);
        break;

    case SCurve_step:
        /* S 曲线模式: vacc = jerk (加加速度), 减速度使用 acc=2400 默认 */
        Motor[num].mode = SCurve_step;
        if (vacc < 100) vacc = 500;  /* jerk 下限保护 */
        scurve_stepper_init(&Motor[num], vstart, vmax, 0,  /* v_end=0 */
                            2400,        /* a_max 暂用固定值 */
                            vacc,        /* j_max = vacc 参数复用 */
                            hz);         /* hz = 步数 */
        break;

    case STOP_mode:
        Motor[num].mode = STOP_mode;
        Motor[num].en   = 0;
        Motor[num].hz   = 0;
        HAL_TIM_Base_Stop_IT(motor_tim[num]);
        HAL_TIM_PWM_Stop(motor_tim[num], motor_channel[num]);
        return;

    default:
        break;
    }

    Motor[num].dir = dir;
    Motor[num].arr = (TIMER_CLK_HZ / (1 + Motor[num].hz)) - 1;

    /* ── 力矩补偿: 计算初始占空比 ── */
    Motor[num].comp_duty = torque_comp_get_duty(num, Motor[num].hz);

    Motor_SetSpeed(num);
}

/* ══════════════════════════════════════════════════════════════════
 *  Motor_SetSpeed — 应用速度设置到硬件
 *
 *  设置 EN/DIR 引脚, 更新定时器 ARR 和比较值,
 *  应用力矩补偿占空比, 启动 PWM 和中断。
 * ══════════════════════════════════════════════════════════════════ */

void Motor_SetSpeed(uint8_t num)
{
    if (num == 0 || num > MOTOR_COUNT)
        return;

    /* 使能和方向 */
    HAL_GPIO_WritePin(en_ports[num], en_pins[num],
                      (Motor[num].en == ENABLE) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(dir_port[num], dir_pins[num], Motor[num].dir);

    /* ── 力矩补偿: 调整占空比 ── */
    uint8_t duty_pct = Motor[num].comp_duty;
    if (duty_pct < 30) duty_pct = 50;  /* 下限保护 */
    if (duty_pct > 90) duty_pct = 75;  /* 上限保护 */

    /* 比较值 = ARR * (100 - duty%) / 100  (低电平占比) */
    /* 即: duty%=50 → compare = ARR/2; duty%=75 → compare = ARR/4 */
    uint16_t arr_val   = Motor[num].arr;
    uint16_t cmp_val   = (uint16_t)((uint32_t)arr_val * duty_pct / 100);

    __HAL_TIM_SET_AUTORELOAD(motor_tim[num], arr_val);
    __HAL_TIM_SET_COMPARE(motor_tim[num], motor_channel[num], cmp_val);
    __HAL_TIM_SET_COUNTER(motor_tim[num], 0);

    /* 启动 PWM 和中断 */
    HAL_TIM_PWM_Start(motor_tim[num], motor_channel[num]);
    HAL_TIM_Base_Start_IT(motor_tim[num]);
}

/* ══════════════════════════════════════════════════════════════════
 *  Motor_GetStep — 获取电机当前步数
 * ══════════════════════════════════════════════════════════════════ */

uint16_t Motor_GetStep(uint8_t num)
{
    switch (num)
    {
    case 1:  return Motor[1].current_step;
    case 2:  return Motor[2].current_step;
    case 3:  return Motor[3].current_step;
    case 4:  return Motor[4].current_step;
    case 5:  return Motor[5].current_step;
    case 6:  return Motor[6].current_step;
    default: return 0;
    }
}

/* ══════════════════════════════════════════════════════════════════
 *  HAL_TIM_PeriodElapsedCallback — 定时器更新中断回调
 *
 *  ISR 热路径 — 对性能敏感。已优化:
 *   - S 曲线: 纯定点运算 (无 sqrtf, 无浮点)
 *   - 梯形: 保留原浮点路径以兼容 (可配置切换到定点)
 *   - 力矩补偿: 每 8 步更新一次占空比 (减少计算开销)
 * ══════════════════════════════════════════════════════════════════ */

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    for (int i = 1; i <= MOTOR_COUNT; i++)
    {
        if (htim != motor_tim[i]) continue;

        /* ── 恒速模式: 无需处理 ── */
        if (Motor[i].mode == Constant_speed)
            break;

        /* ── 停止模式: 安全关闭 ── */
        if (Motor[i].mode == STOP_mode)
        {
            HAL_TIM_Base_Stop_IT(motor_tim[i]);
            HAL_TIM_PWM_Stop(motor_tim[i], motor_channel[i]);
            break;
        }

        /* ── 检查是否已到目标 ── */
        if (Motor[i].current_step >= Motor[i].target_step)
        {
            Motor[i].current_step = 0;
            Motor[i].target_step  = 0;
            HAL_TIM_Base_Stop_IT(motor_tim[i]);
            HAL_TIM_PWM_Stop(motor_tim[i], motor_channel[i]);
            break;
        }

        /* ── 递增步数 ── */
        Motor[i].current_step++;

        /* ── 根据曲线类型计算下一步速度 ── */
        uint16_t next_hz;

        if (Motor[i].profile_type == PROFILE_SCURVE
            && Motor[i].mode == SCurve_step)
        {
            /* ★ S 曲线: 纯定点运算, ISR 安全 ★ */
            next_hz = scurve_step_update(&Motor[i].scurve_plan,
                                         &Motor[i].scurve_state);

            if (Motor[i].scurve_state.finished)
            {
                /* S 曲线运动完成 */
                Motor[i].current_step = 0;
                Motor[i].target_step  = 0;
                HAL_TIM_Base_Stop_IT(motor_tim[i]);
                HAL_TIM_PWM_Stop(motor_tim[i], motor_channel[i]);
                break;
            }
        }
        else
        {
            /* ★ 梯形: 保留原浮点逻辑 (兼容) ★ */
            next_hz = get_step_speed(Motor[i].current_step,
                                     Motor[i].steps,
                                     Motor[i].velocity);
        }

        Motor[i].hz = next_hz;

        /* ── 每 8 步更新力矩补偿 ── */
        if ((Motor[i].current_step & 0x07) == 0)
        {
            Motor[i].comp_duty = torque_comp_get_duty(i, next_hz);
        }

        /* ── 更新定时器 ──
         * arr = TIMER_CLK_HZ / (hz + 1) - 1
         * 因为 TIMER_CLK_HZ = 1000000, hz ≤ 65535
         * arr ≥ 14 (安全, 不会溢出)
         */
        {
            uint16_t arr = (uint16_t)(TIMER_CLK_HZ / (uint32_t)(next_hz + 1));
            if (arr > 0) arr -= 1;

            /* 力矩补偿占空比 → 比较值 */
            uint8_t duty = Motor[i].comp_duty;
            if (duty < 30) duty = 50;
            if (duty > 90) duty = 75;
            uint16_t cmp = (uint16_t)((uint32_t)arr * duty / 100);

            __HAL_TIM_SET_AUTORELOAD(motor_tim[i], arr);
            __HAL_TIM_SET_COMPARE(motor_tim[i], motor_channel[i], cmp);
            __HAL_TIM_SET_COUNTER(motor_tim[i], 0);
        }

        break;
    }
}
