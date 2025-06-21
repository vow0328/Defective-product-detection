#include "math.h"


MotionPhase calc_trapezoid_profile(TrapezoidProfile profile)//计算梯形运动规划
{
    MotionPhase phase = {0};

    // 加速段步数 = (v_max^2 - v_start^2) / (2a) 9
    phase.accel_steps = (int)((profile.v_max * profile.v_max - profile.v_start * profile.v_start) / (2 * profile.acc));
    phase.decel_steps = phase.accel_steps; // 对称减速
    int sum_steps = phase.accel_steps + phase.decel_steps;

    if (sum_steps > profile.total_steps)
    {
        // 达不到最大速度，只能部分加速再减速（即三角形加减速）
        phase.accel_steps = profile.total_steps / 2;
        phase.decel_steps = profile.total_steps - phase.accel_steps;
        phase.cruise_steps = 0;
    }
    else
    {
        phase.cruise_steps = profile.total_steps - sum_steps;
    }

    return phase;
}


uint16_t get_step_speed(int step_idx, MotionPhase phase, TrapezoidProfile profile)// 根据当前步数计算对应速度
{
    if (step_idx < phase.accel_steps)
    {
        // 加速段：v = sqrt(v0^2 + 2as)
        return (uint16_t)sqrtf(profile.v_start * profile.v_start + 2 * profile.acc * step_idx);
    }
    else if (step_idx < phase.accel_steps + phase.cruise_steps)
    {
        // 匀速段
        return profile.v_max;
    }
    else
    {
        // 减速段：从最大速度减速
        int s = step_idx - (phase.accel_steps + phase.cruise_steps);
        return (uint16_t)sqrtf(profile.v_max * profile.v_max - 2 * profile.acc * s);
    }
}
