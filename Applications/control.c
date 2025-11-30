/**
 * @file    control.c
 * @brief   串口指令解析与执行
 *
 * 协议格式: 0xFF + [CMD] + [DATA...] + 0xFE
 *
 * 指令集:
 *   0x01 — 电机控制 (兼容旧协议, 5 字节数据)
 *          数据: [Motor][Mode][Dir][Pulse_H][Pulse_L]
 *
 *   0x02 — 输出控制 (1 字节数据)
 *          数据: [Output_Bitmask]
 *
 *   0x03 — S 曲线电机控制 (8 字节数据)
 *          数据: [Motor][Mode][Dir][Steps_H][Steps_L]
 *                [VStart_H][VStart_L][Jerk_H][Jerk_L]
 *
 *   0x04 — 力矩补偿配置 (4 字节数据)
 *          数据: [Axis][PointIdx][Duty][Speed_H][Speed_L]
 *
 *   0x05 — 力矩补偿使能 (2 字节数据)
 *          数据: [Axis][Enable]
 *
 *   0x10 — 读取电机状态 (1 字节数据)
 *          数据: [Motor]
 *          返回: 0xAA + [Motor] + [Mode] + [CurrentStep_H:CurrentStep_L]
 *                + [TargetStep_H:TargetStep_L] + [Speed_H:Speed_L] + [Duty] + 0x55
 */

#include "include.h"

/* 外部全局变量 */
extern MotorStruct Motor[];

/* ══════════════════════════════════════════════════════════════════
 *  Command_Control — 串口指令分发 (由 Scheduler 1000Hz 调用)
 * ══════════════════════════════════════════════════════════════════ */

void Command_Control(void)
{
    uint8_t flag = Serial3_GetRxData();
    if (flag == 0) return;  /* 无新数据 */

    switch (flag)
    {
    /* ── 0x01: 电机控制 (梯形/恒速, 兼容旧协议) ── */
    case 0x01:
        {
            uint8_t  cmd[5];
            Serial3_GetRxPacket(cmd, 5);

            uint8_t  motor  = cmd[0];                         /* 电机编号 1~6   */
            uint8_t  motion = cmd[1];                         /* 模式 1/2/3      */
            uint8_t  dir    = cmd[2];                         /* 方向 0/1        */
            uint16_t pulses = ((uint16_t)cmd[3] << 8) | cmd[4]; /* 脉冲数/速度   */

            /* 使用默认参数 (可通过 0x03 指令覆盖) */
            uint16_t vstart = 800;
            uint16_t vmax   = 3200;
            uint16_t vacc   = 2400;

            Motor_Set(motor, motion, (GPIO_PinState)dir,
                      pulses, vstart, vmax, vacc);
        }
        break;

    /* ── 0x02: 输出控制 ── */
    case 0x02:
        {
            uint8_t cmd[1];
            Serial3_GetRxPacket(cmd, 1);
            uint8_t output_mask = cmd[0];

            for (uint8_t i = 1; i <= 8; i++)
            {
                OUTPUT_control(i, (output_mask & (1 << (i - 1)))
                                  ? GPIO_PIN_SET : GPIO_PIN_RESET);
            }
        }
        break;

    /* ── 0x03: S 曲线电机控制 ── */
    case 0x03:
        {
            uint8_t cmd[8];
            Serial3_GetRxPacket(cmd, 8);

            uint8_t  motor  = cmd[0];
            uint8_t  motion = cmd[1];                         /* 4=S曲线 或 2=梯形 */
            uint8_t  dir    = cmd[2];
            uint16_t steps  = ((uint16_t)cmd[3] << 8) | cmd[4];
            uint16_t vstart = ((uint16_t)cmd[5] << 8) | cmd[6];
            uint16_t jmax   = cmd[7] * 100;                  /* jerk = cmd[7] * 100 */

            if (vstart < 100) vstart = 100;
            if (jmax < 100)   jmax   = 500;

            if (motion == SCurve_step)
            {
                /* S 曲线: vmax 从内部默认, acc 固定 2400 */
                Motor_Set(motor, SCurve_step, (GPIO_PinState)dir,
                          steps, vstart, 3200, jmax);
            }
            else
            {
                /* 降级为梯形 */
                Motor_Set(motor, Constant_step, (GPIO_PinState)dir,
                          steps, vstart, 3200, 2400);
            }
        }
        break;

    /* ── 0x04: 力矩补偿曲线配置 ── */
    case 0x04:
        {
            uint8_t cmd[5];
            Serial3_GetRxPacket(cmd, 5);

            uint8_t  axis  = cmd[0];
            uint8_t  idx   = cmd[1];
            uint8_t  duty  = cmd[2];
            uint16_t speed = ((uint16_t)cmd[3] << 8) | cmd[4];

            torque_comp_set_point(axis, idx, speed, duty);
        }
        break;

    /* ── 0x05: 力矩补偿使能/禁用 ── */
    case 0x05:
        {
            uint8_t cmd[2];
            Serial3_GetRxPacket(cmd, 2);

            uint8_t axis   = cmd[0];
            bool    enable = (cmd[1] != 0);

            torque_comp_set_enable(axis, enable);
        }
        break;

    /* ── 0x10: 读取电机状态 ── */
    case 0x10:
        {
            uint8_t cmd[1];
            Serial3_GetRxPacket(cmd, 1);
            uint8_t motor = cmd[0];

            if (motor >= 1 && motor <= MOTOR_COUNT)
            {
                Serial3_SendByte(0xAA);
                Serial3_SendByte(motor);
                Serial3_SendByte(Motor[motor].mode);

                /* 当前步数 (高8位 + 低8位) */
                Serial3_SendByte((uint8_t)(Motor[motor].current_step >> 8));
                Serial3_SendByte((uint8_t)(Motor[motor].current_step & 0xFF));

                /* 目标步数 */
                Serial3_SendByte((uint8_t)(Motor[motor].target_step >> 8));
                Serial3_SendByte((uint8_t)(Motor[motor].target_step & 0xFF));

                /* 当前速度 */
                Serial3_SendByte((uint8_t)(Motor[motor].hz >> 8));
                Serial3_SendByte((uint8_t)(Motor[motor].hz & 0xFF));

                /* 补偿占空比 */
                Serial3_SendByte(Motor[motor].comp_duty);

                Serial3_SendByte(0x55);
            }
        }
        break;

    default:
        /* 未知指令: 忽略 */
        break;
    }
}
