/**
 * @file    include.h
 * @brief   全局头文件 — 所有应用模块的统一入口
 *
 * 更新:
 *   - 新增 fixed_math, scurve_profile, torque_comp 模块
 *   - 修复 extern 声明与 include.c 定义一致
 */

#ifndef _INCLUDE_H
#define _INCLUDE_H

#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_tim.h"
#include "main.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* ── 应用层模块 ── */
#include "Scheduler.h"
#include "control.h"
#include "math.h"
#include "fixed_math.h"
#include "scurve_profile.h"
#include "torque_comp.h"

/* ── 硬件层模块 ── */
#include "H_Tmc2209.h"
#include "serial.h"
#include "input.h"
#include "output.h"
#include "pvd.h"

/* ── 硬件映射数组 (定义在 include.c) ── */

extern TIM_HandleTypeDef *motor_tim[];
extern uint32_t motor_channel[];
extern GPIO_TypeDef *en_ports[];
extern uint16_t en_pins[];
extern GPIO_TypeDef *dir_port[];
extern uint16_t dir_pins[];
extern GPIO_TypeDef *input_ports[];
extern uint16_t input_pins[];
extern GPIO_TypeDef *output_ports[];
extern uint16_t output_pins[];

#endif /* _INCLUDE_H */
