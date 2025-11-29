/**
 * @file    Scheduler.c
 * @brief   基于时间片轮询的多任务调度器
 *
 * 特性:
 *   - 8 个任务槽位, 频率覆盖 1000Hz ~ 2Hz
 *   - 基于 HAL_GetTick() 的协同式轮询
 *   - 支持动态添加任务
 *
 * 任务分配:
 *   1000Hz: 串口指令处理 (Command_Control)
 *   500Hz:  电机速度规划更新
 *   200Hz:  力矩补偿更新
 *   100Hz:  状态监控
 *   50Hz:   (预留)
 *   20Hz:   (预留)
 *   10Hz:   输入传感器数据发送
 *   2Hz:    心跳/调试输出
 */

#include "include.h"

/* ══════════════════════════════════════════════════════════════════
 *  外部函数声明
 * ══════════════════════════════════════════════════════════════════ */

extern MotorStruct Motor[];

/* ══════════════════════════════════════════════════════════════════
 *  任务函数实现
 * ══════════════════════════════════════════════════════════════════ */

/**
 * Loop_1000Hz — 1ms 周期 (最高优先级)
 * 串口指令处理: 解析 UART 数据包并执行运动指令
 */
static void Loop_1000Hz(void)
{
    Command_Control();
}

/**
 * Loop_500Hz — 2ms 周期
 * S 曲线速度规划: 对正在运行的 S 曲线轴进行速度预处理
 * 将下一次速度值预先计算好, 减少 ISR 内的计算量
 */
static void Loop_500Hz(void)
{
    // S 曲线速度预计算 (可选优化: 提前计算若干步的速度并缓冲)
    // 目前 ISR 中已足够快, 此处留空作为扩展点
}

/**
 * Loop_200Hz — 5ms 周期
 * 力矩补偿批量更新: 检查各轴速度并更新占空比补偿
 */
static void Loop_200Hz(void)
{
    for (int i = 1; i <= MOTOR_COUNT; i++)
    {
        if (Motor[i].en == ENABLE
            && (Motor[i].mode == Constant_step || Motor[i].mode == SCurve_step))
        {
            Motor[i].comp_duty = torque_comp_get_duty(i, Motor[i].hz);
        }
    }
}

/**
 * Loop_100Hz — 10ms 周期
 * 系统状态监控: 检查丢步、超温、过流等异常
 */
static void Loop_100Hz(void)
{
    // 状态监控预留
    // 可在此检测电机是否在规定时间内完成运动
    // 配合传感器反馈实现丢步检测
}

/**
 * Loop_50Hz — 20ms 周期 (预留)
 */
static void Loop_50Hz(void)
{
}

/**
 * Loop_20Hz — 50ms 周期 (预留)
 */
static void Loop_20Hz(void)
{
}

/**
 * Loop_10Hz — 100ms 周期
 * 发送输入传感器状态到上位机
 */
static void Loop_10Hz(void)
{
    Send_INPUT();
}

/**
 * Loop_2Hz — 500ms 周期
 * 心跳/调试信息输出
 */
static void Loop_2Hz(void)
{
    // 调试输出预留
    // Serial3_SendByte(0xaa);
}

/* ══════════════════════════════════════════════════════════════════
 *  任务表定义
 * ══════════════════════════════════════════════════════════════════ */

static sched_task_t sched_tasks[] =
{
    {Loop_1000Hz, 1000, 0, 0},
    {Loop_500Hz,   500, 0, 0},
    {Loop_200Hz,   200, 0, 0},
    {Loop_100Hz,   100, 0, 0},
    {Loop_50Hz,     50, 0, 0},
    {Loop_20Hz,     20, 0, 0},
    {Loop_10Hz,     10, 0, 0},
    {Loop_2Hz,       2, 0, 0},
};

#define TASK_NUM (sizeof(sched_tasks) / sizeof(sched_task_t))

/* ══════════════════════════════════════════════════════════════════
 *  Scheduler_Setup — 初始化任务调度器
 *  计算每个任务的间隔 tick 数
 * ══════════════════════════════════════════════════════════════════ */

void Scheduler_Setup(void)
{
    for (uint8_t index = 0; index < TASK_NUM; index++)
    {
        sched_tasks[index].interval_ticks = TICK_PER_SECOND
                                          / sched_tasks[index].rate_hz;
        if (sched_tasks[index].interval_ticks < 1)
        {
            sched_tasks[index].interval_ticks = 1;
        }
    }
}

/* ══════════════════════════════════════════════════════════════════
 *  Scheduler_Run — 运行调度器 (在 main while(1) 中循环调用)
 * ══════════════════════════════════════════════════════════════════ */

void Scheduler_Run(void)
{
    for (uint8_t index = 0; index < TASK_NUM; index++)
    {
        uint32_t tnow = HAL_GetTick();

        if (tnow - sched_tasks[index].last_run >= sched_tasks[index].interval_ticks)
        {
            sched_tasks[index].last_run = tnow;
            sched_tasks[index].task_func();
        }
    }
}

/******************* (C) COPYRIGHT 2014 ANO TECH *****END OF FILE************/
