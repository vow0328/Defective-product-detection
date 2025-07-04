#include "include.h"

MotorStruct motor[10] = {0};

void Motor_Init()
{
    __HAL_TIM_CLEAR_IT(&htim1, TIM_CHANNEL_4);
    __HAL_TIM_CLEAR_IT(&htim8, TIM_CHANNEL_3);
    __HAL_TIM_CLEAR_IT(&htim2, TIM_CHANNEL_4);
    // __HAL_TIM_CLEAR_IT(&htim8, TIM_CHANNEL_1);
    // __HAL_TIM_CLEAR_IT(&htim1, TIM_CHANNEL_1); // 清除中断标志位
}

void Motor_SetSpeed(uint8_t num, uint8_t mode, GPIO_PinState dir, uint16_t hz) // 模式1定速 模式2定步 模式3停止
{
    uint8_t motor_mode = 0, en = 1;
    uint16_t target_step = 0;
    switch (mode)
    {
    case Constant_speed: // 定速模式
        motor_mode = Constant_speed;
        break;
    case Constant_step: // 定步模式
        motor_mode = Constant_step;
        target_step = hz;
        hz = 800; // 模式一速度为输入速度,模式二固定速度 1600为一秒一圈
        break;
    case STOP_mode: // 停止模式
        motor_mode = STOP_mode;
        en = 0;
        break;
    default:
        break;
    }
    switch (num)
    {
    case 1:
        motor[1].mode = motor_mode;
        motor[1].target_step = (motor[1].mode == Constant_step) ? target_step : 0;
        motor[1].current_step = 0;
        Motor1_SetSpeed(en, dir, hz);
        break;
    case 2:
        motor[2].mode = motor_mode;
        motor[2].target_step = (motor[2].mode == Constant_step) ? target_step : 0;
        motor[2].current_step = 0;
        Motor2_SetSpeed(en, dir, hz);
        break;
    case 3:
        motor[3].mode = motor_mode;
        motor[3].target_step = (motor[3].mode == Constant_step) ? target_step : 0;
        motor[3].current_step = 0;
        Motor3_SetSpeed(en, dir, hz);
    case 4:
        motor[4].mode = motor_mode;
        motor[4].target_step = (motor[4].mode == Constant_step) ? target_step : 0;
        motor[4].current_step = 0;
        Motor4_SetSpeed(en, dir, hz);
    case 5:
        motor[5].mode = motor_mode;
        motor[5].target_step = (motor[5].mode == Constant_step) ? target_step : 0;
        motor[5].current_step = 0;
        Motor5_SetSpeed(en, dir, hz);
    case 6:
        motor[6].mode = motor_mode;
        motor[6].target_step = (motor[6].mode == Constant_step) ? target_step : 0;
        motor[6].current_step = 0;
        Motor6_SetSpeed(en, dir, hz);
        break;
    default:
        break;
    }
}

void Motor1_SetSpeed(uint8_t en, GPIO_PinState dir, uint16_t hz)
{
    HAL_GPIO_WritePin(EN1OUT_GPIO_Port, EN1OUT_Pin, (en == ENABLE) ? GPIO_PIN_SET : GPIO_PIN_RESET); // 1使能 0失能
    HAL_GPIO_WritePin(DIR1OUT_GPIO_Port, DIR1OUT_Pin, dir);                                          // 电机方向 1正转0反转

    if (motor[1].hz != hz)
    {
        uint16_t arr = (TIMER_CLK_HZ / hz) - 1;
        __HAL_TIM_SET_AUTORELOAD(&htim1, arr);
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, arr / 2);
        __HAL_TIM_SET_COUNTER(&htim1, 0); // 重置计数器
        motor[1].hz = hz;                 // 更新目标等级
    }
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1); // 启动pwm模式
    HAL_TIM_Base_Start_IT(&htim1);            // 启动计步
}

void Motor2_SetSpeed(uint8_t en, GPIO_PinState dir, uint16_t hz)
{
    HAL_GPIO_WritePin(EN2OUT_GPIO_Port, EN2OUT_Pin, (en == ENABLE) ? GPIO_PIN_SET : GPIO_PIN_RESET); // 1使能 0失能
    HAL_GPIO_WritePin(DIR2OUT_GPIO_Port, DIR2OUT_Pin, dir);                                          // 电机方向 1正转0反转

    if (motor[2].hz != hz)
    {
        uint16_t arr = (TIMER_CLK_HZ / hz) - 1;
        __HAL_TIM_SET_AUTORELOAD(&htim8, arr);
        __HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_2, arr / 2);
        __HAL_TIM_SET_COUNTER(&htim8, 0); // 重置计数器
        motor[2].hz = hz;                 // 更新目标等级
    }
    HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_2); // 启动pwm模式
    HAL_TIM_Base_Start_IT(&htim8);            // 启动计步
}
void Motor3_SetSpeed(uint8_t en, GPIO_PinState dir, uint16_t hz)
{
    HAL_GPIO_WritePin(EN3OUT_GPIO_Port, EN3OUT_Pin, (en == ENABLE) ? GPIO_PIN_SET : GPIO_PIN_RESET); // 1使能 0失能
    HAL_GPIO_WritePin(DIR3OUT_GPIO_Port, DIR3OUT_Pin, dir);                                          // 电机方向 1正转0反转

    if (motor[3].hz != hz)
    {
        uint16_t arr = (TIMER_CLK_HZ / hz) - 1;
        __HAL_TIM_SET_AUTORELOAD(&htim2, arr);
        __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, arr / 2);
        __HAL_TIM_SET_COUNTER(&htim2, 0); // 重置计数器
        motor[3].hz = hz;                 // 更新目标等级
    }
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3); // 启动pwm模式
    HAL_TIM_Base_Start_IT(&htim2);            // 启动计步
}
void Motor4_SetSpeed(uint8_t en, GPIO_PinState dir, uint16_t hz)
{
    HAL_GPIO_WritePin(EN4OUT_GPIO_Port, EN4OUT_Pin, (en == ENABLE) ? GPIO_PIN_SET : GPIO_PIN_RESET); // 1使能 0失能
    HAL_GPIO_WritePin(DIR4OUT_GPIO_Port, DIR4OUT_Pin, dir);                                          // 电机方向 1正转0反转

    if (motor[4].hz != hz)
    {
        uint16_t arr = (TIMER_CLK_HZ / hz) - 1;
        __HAL_TIM_SET_AUTORELOAD(&htim3, arr);
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, arr / 2);
        __HAL_TIM_SET_COUNTER(&htim3, 0); // 重置计数器
        motor[4].hz = hz;                 // 更新目标等级
    }
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3); // 启动pwm模式
    HAL_TIM_Base_Start_IT(&htim3);            // 启动计步
}
void Motor5_SetSpeed(uint8_t en, GPIO_PinState dir, uint16_t hz)
{
    HAL_GPIO_WritePin(EN5OUT_GPIO_Port, EN5OUT_Pin, (en == ENABLE) ? GPIO_PIN_SET : GPIO_PIN_RESET); // 1使能 0失能
    HAL_GPIO_WritePin(DIR5OUT_GPIO_Port, DIR5OUT_Pin, dir);                                          // 电机方向 1正转0反转

    if (motor[5].hz != hz)
    {
        uint16_t arr = (TIMER_CLK_HZ / hz) - 1;
        __HAL_TIM_SET_AUTORELOAD(&htim5, arr);
        __HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_4, arr / 2);
        __HAL_TIM_SET_COUNTER(&htim5, 0); // 重置计数器
        motor[5].hz = hz;                 // 更新目标等级
    }
    HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_4); // 启动pwm模式
    HAL_TIM_Base_Start_IT(&htim5);            // 启动计步
}
void Motor6_SetSpeed(uint8_t en, GPIO_PinState dir, uint16_t hz)
{
    HAL_GPIO_WritePin(EN6OUT_GPIO_Port, EN6OUT_Pin, (en == ENABLE) ? GPIO_PIN_SET : GPIO_PIN_RESET); // 1使能 0失能
    HAL_GPIO_WritePin(DIR6OUT_GPIO_Port, DIR6OUT_Pin, dir);                                          // 电机方向 1正转0反转

    if (motor[6].hz != hz)
    {
        uint16_t arr = (TIMER_CLK_HZ / hz) - 1;
        __HAL_TIM_SET_AUTORELOAD(&htim4, arr);
        __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_2, arr / 2);
        __HAL_TIM_SET_COUNTER(&htim4, 0); // 重置计数器
        motor[6].hz = hz;                 // 更新目标等级
    }
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2); // 启动pwm模式
    HAL_TIM_Base_Start_IT(&htim4);            // 启动计步
}

uint16_t Motor1_GetStep() // 细分步数为1  脉冲大概为180度
{                         // 电机脉冲值获取
    return motor[1].current_step;
}
uint16_t Motor2_GetStep()
{ // 电机脉冲值获取
    return motor[2].current_step;
}
uint16_t Motor3_GetStep()
{ // 电机脉冲值获取
    return motor[3].current_step;
}
uint16_t Motor4_GetStep()
{ // 电机脉冲值获取
    return motor[4].current_step;
}
uint16_t Motor5_GetStep()
{ // 电机脉冲值获取
    return motor[5].current_step;
}
uint16_t Motor6_GetStep()
{ // 电机脉冲值获取
    return motor[6].current_step;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{

    if (htim == &htim1)
    {
        if (motor[1].mode == Constant_step && motor[1].target_step)
        {
            motor[1].current_step++;
            if (motor[1].current_step >= motor[1].target_step)
            {
                motor[1].current_step = 0;
                motor[1].target_step = 0;
                HAL_GPIO_WritePin(EN1OUT_GPIO_Port, EN1OUT_Pin, GPIO_PIN_RESET); // 失能en引脚
                HAL_TIM_Base_Stop_IT(&htim1);                                    // 关闭计步
                HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);                         // 关闭pwm输出
            }
        }
    }
    else if (htim == &htim8)
    {
        if (motor[2].mode == Constant_step && motor[2].target_step)
        {
            motor[2].current_step++;
            if (motor[2].current_step >= motor[2].target_step)
            {
                motor[2].current_step = 0;
                motor[2].target_step = 0;
                HAL_GPIO_WritePin(EN2OUT_GPIO_Port, EN2OUT_Pin, GPIO_PIN_RESET);
                HAL_TIM_Base_Stop_IT(&htim8);
                HAL_TIM_PWM_Stop(&htim8, TIM_CHANNEL_2);
            }
        }
    }
    else if (htim == &htim2)
    {
        if (motor[3].mode == Constant_step && motor[3].target_step)
        {
            motor[3].current_step++;
            if (motor[3].current_step >= motor[3].target_step)
            {
                motor[3].current_step = 0;
                motor[3].target_step = 0;
                HAL_GPIO_WritePin(EN3OUT_GPIO_Port, EN3OUT_Pin, GPIO_PIN_RESET);
                HAL_TIM_Base_Stop_IT(&htim2);
                HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_3);
            }
        }
    }
    else if (htim == &htim3)
    {
        if (motor[4].mode == Constant_step && motor[4].target_step)
        {
            motor[4].current_step++;
            if (motor[4].current_step >= motor[4].target_step)
            {
                motor[4].current_step = 0;
                motor[4].target_step = 0;
                HAL_GPIO_WritePin(EN4OUT_GPIO_Port, EN4OUT_Pin, GPIO_PIN_RESET);
                HAL_TIM_Base_Stop_IT(&htim3);
                HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_3);
            }
        }
    }
    else if (htim == &htim5)
    {
        if (motor[5].mode == Constant_step && motor[5].target_step)
        {
            motor[5].current_step++;
            if (motor[5].current_step >= motor[5].target_step)
            {
                motor[5].current_step = 0;
                motor[5].target_step = 0;
                HAL_GPIO_WritePin(EN5OUT_GPIO_Port, EN5OUT_Pin, GPIO_PIN_RESET);
                HAL_TIM_Base_Stop_IT(&htim5);
                HAL_TIM_PWM_Stop(&htim5, TIM_CHANNEL_4);
            }
        }
    }
    else if (htim == &htim4)
    {
        if (motor[6].mode == Constant_step && motor[6].target_step)
        {
            motor[6].current_step++;
            if (motor[6].current_step >= motor[6].target_step)
            {
                motor[6].current_step = 0;
                motor[6].target_step = 0;
                HAL_GPIO_WritePin(EN6OUT_GPIO_Port, EN6OUT_Pin, GPIO_PIN_RESET);
                HAL_TIM_Base_Stop_IT(&htim4);
                HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_2);
            }
        }
    }
}
