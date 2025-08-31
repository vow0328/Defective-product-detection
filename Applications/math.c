#include "math.h"

MotionStep calc_trapezoid_profile(TrapezoidVelocity profile) // 计算梯形运动规划
{
    MotionStep phase = {0};

    // 加速段步数 = (v_max^2 - v_start^2) / (2a)
    phase.accel_steps = (int)((profile.v_max * profile.v_max - profile.v_start * profile.v_start) / (2 * profile.acc));

    // 减速段步数 = (v_max^2 - 0) / (2a) （确保减速到0）
    phase.decel_steps = (int)((profile.v_max * profile.v_max) / (2 * profile.acc));

    int sum_steps = phase.accel_steps + phase.decel_steps;

    if (sum_steps > profile.total_steps)
    {
        // 达不到最大速度，只能部分加速再减速（即三角形加减速）
        // 重新计算可达到的最大速度
        // 根据公式推导：total_steps = (v_max^2 - v_start^2)/(2*acc) + v_max^2/(2*acc)
        // 解得：v_max = sqrt((2*acc*total_steps + v_start^2) / 2)
        float max_vel_sq = (2.0f * profile.acc * profile.total_steps + profile.v_start * profile.v_start) / 2.0f;
        float new_max_vel = sqrtf(max_vel_sq);

        // 用新的最大速度重新计算加速和减速步数
        phase.accel_steps = (int)((new_max_vel * new_max_vel - profile.v_start * profile.v_start) / (2 * profile.acc));
        phase.decel_steps = (int)((new_max_vel * new_max_vel) / (2 * profile.acc));
        phase.cruise_steps = 0;
    }
    else
    {
        phase.cruise_steps = profile.total_steps - sum_steps;
    }

    return phase;
}

uint16_t get_step_speed(int step_idx, MotionStep phase, TrapezoidVelocity profile) // 根据当前步数计算对应速度
{
    if (step_idx <= phase.accel_steps)
    {
        // 加速段
        return (uint16_t)sqrtf(profile.v_start * profile.v_start + 2 * profile.acc * step_idx);
    }
    else if (step_idx <= phase.accel_steps + phase.cruise_steps)
    {
        // 匀速段
        return profile.v_max;
    }
    else if (step_idx <= phase.accel_steps + phase.cruise_steps + phase.decel_steps)
    {
        // 减速段
        int s = step_idx - (phase.accel_steps + phase.cruise_steps);
        return (uint16_t)sqrtf(profile.v_max * profile.v_max - 2 * profile.acc * s);
    }
    else
    {
        // 超出范围，默认停止
        return 0;
    }
}
