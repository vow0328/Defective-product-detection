/**
 * @file    torque_comp.h
 * @brief   前馈式力矩补偿策略
 *
 * 原理:
 *   开环步进电机在高速段因反电动势增大导致绕组电流下降，
 *   转矩衰减，容易丢步。通过预标定的速度-力矩补偿曲线，
 *   在高速段增大 PWM 占空比以补偿转矩损失。
 *
 * 补偿方式:
 *   - 低速段 (v < v_threshold_low):  占空比 = 50% (基准)
 *   - 中速段 (v_threshold_low ~ v_threshold_high): 线性递增
 *   - 高速段 (v > v_threshold_high): 占空比 = 最大值 (默认 75%)
 *
 * 标定:
 *   每轴独立标定, 参数可通过串口指令读写。
 *   支持 3 点标定: (速度, 补偿%) 对定义补偿曲线。
 */

#ifndef _TORQUE_COMP_H
#define _TORQUE_COMP_H

#include <stdint.h>
#include <stdbool.h>

/* ── 每轴补偿参数 ──────────────────────────────────────────────── */

#define TORQUE_COMP_POINTS 4  /* 每轴补偿点数 (含 0 和 max) */

typedef struct
{
    uint16_t speed;     /* 速度点 (steps/s)                          */
    uint8_t  duty_pct;  /* 对应占空比百分比 (50~85)                  */
} TorqueCompPoint;

typedef struct
{
    bool     enabled;                        /* 是否启用补偿          */
    uint8_t  base_duty;                      /* 基准占空比 % (默认 50)*/
    uint8_t  max_duty;                       /* 最大占空比 % (默认 75)*/
    TorqueCompPoint points[TORQUE_COMP_POINTS]; /* 补偿曲线点         */
} TorqueCompConfig;

/* ── 全局补偿配置 (6 轴) ────────────────────────────────────────── */

#define TORQUE_AXES 6
extern TorqueCompConfig torque_config[TORQUE_AXES + 1];  /* 索引 1~6 */

/* ── API ────────────────────────────────────────────────────────── */

/**
 * torque_comp_init — 初始化力矩补偿
 * 设置默认补偿参数 (所有轴相同)
 */
void torque_comp_init(void);

/**
 * torque_comp_get_duty — 根据当前速度获取补偿后占空比
 *
 * @param  axis  轴号 (1~6)
 * @param  speed 当前速度 (steps/s)
 * @return       补偿后占空比百分比 (50~85)
 *
 * 线性插值查找补偿曲线
 */
uint8_t torque_comp_get_duty(uint8_t axis, uint16_t speed);

/**
 * torque_comp_set_point — 设置补偿曲线点
 *
 * @param  axis   轴号 (1~6)
 * @param  index  点索引 (0~3)
 * @param  speed  速度值
 * @param  duty   占空比百分比
 */
void torque_comp_set_point(uint8_t axis, uint8_t index,
                           uint16_t speed, uint8_t duty);

/**
 * torque_comp_set_enable — 启用/禁用某轴补偿
 */
void torque_comp_set_enable(uint8_t axis, bool enable);

/**
 * torque_comp_calibrate — 简易自动标定
 *
 * 通过逐步增加速度并检测丢步来建立补偿曲线。
 * 需要外部反馈 (如传感器检测实际位置) 或人工观察。
 *
 * @param  axis       轴号
 * @param  v_start    起始标定速度
 * @param  v_end      结束标定速度
 * @param  v_step     速度步进
 * @return            0=成功, 非0=错误码
 */
int torque_comp_calibrate(uint8_t axis, uint16_t v_start,
                          uint16_t v_end, uint16_t v_step);

#endif /* _TORQUE_COMP_H */
