#include "include.h"

//  电机模式
uint8_t motor1_mode = 0, motor2_mode = 0, motor3_mode = 0;

// 直接以脉冲频率（Hz）控制
uint16_t current1_hz = 0, target1_hz = 0;
uint16_t current2_hz = 0, target2_hz = 0;
uint16_t current3_hz = 0, target3_hz = 0;

// 1号电机预计与当前值
uint16_t current1_step = 0, target1_step = 0;
// 2号电机预计与当前值
uint16_t current2_step = 0, target2_step = 0;
// 3号电机预计与当前值
uint16_t current3_step = 0, target3_step = 0;

// 电机减速预留步数
uint16_t motor1_decel_step = 0, motor2_decel_step = 0, motor3_decel_step = 0;

void Motor_Init()
{
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1); // 启动pwm模式
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);
    HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_1);
    HAL_TIM_Base_Start_IT(&htim1); // 定时器启动
    HAL_TIM_Base_Start_IT(&htim2);
    HAL_TIM_Base_Start_IT(&htim8);
    HAL_TIM_Base_Start_IT(&htim6);
    __HAL_TIM_CLEAR_IT(&htim1, TIM_CHANNEL_1); // 清除中断标志位
    __HAL_TIM_CLEAR_IT(&htim2, TIM_CHANNEL_4);
    __HAL_TIM_CLEAR_IT(&htim8, TIM_CHANNEL_1);
}

void Motor_SetSpeed(uint8_t num, uint8_t mode, GPIO_PinState dir, uint16_t hz) // 模式1定速 模式2定步 模式3停止
{
    uint8_t motor_mode = 0, en = 1;
    uint16_t target_step = 0, decel_step = 0;
    switch (mode)
    {
    case Constant_speed:
        motor_mode = Constant_speed;
        break;
    case Constant_step:
        motor_mode = Constant_step;
        target_step = hz + Step_compensation; // 模式一不限步数,模式二步数为输入脉冲数;
        hz = 3200;                            // 模式一速度为输入速度,模式二固定速度 200为一秒一圈
        decel_step = (hz / ACCEL_HZ) + ((hz % ACCEL_HZ != 0) ? 1 : 0);
        break;
    case STOP_mode:
        motor_mode = STOP_mode;
        en = 0;
        break;
    default:
        break;
    }
    switch (num)
    {
    case 1:
        motor1_mode = motor_mode;
        target1_step = (motor1_mode == Constant_step) ? target_step : 0;
        motor1_decel_step = (motor1_mode == Constant_step) ? decel_step : 0;
        current1_step = 0;
        Motor1_SetSpeed(en, dir, hz);
        break;
    case 2:
        motor2_mode = motor_mode;
        target2_step = (motor2_mode == Constant_step) ? target_step : 0;
        motor2_decel_step = (motor2_mode == Constant_step) ? decel_step : 0;
        current2_step = 0;
        Motor2_SetSpeed(en, dir, hz);
        break;
    case 3:
        motor3_mode = motor_mode;
        target3_step = (motor3_mode == Constant_step) ? target_step : 0;
        motor3_decel_step = (motor3_mode == Constant_step) ? decel_step : 0;
        current3_step = 0;
        Motor3_SetSpeed(en, dir, hz);
        break;
    default:
        break;
    }
}

void Motor1_SetSpeed(uint8_t en, GPIO_PinState dir, uint16_t hz)
{
    HAL_GPIO_WritePin(EN1OUT_GPIO_Port, EN1OUT_Pin, (en == ENABLE) ? GPIO_PIN_SET : GPIO_PIN_RESET); // 1使能 0失能
    HAL_GPIO_WritePin(COM1OUT_GPIO_Port, COM1OUT_Pin, GPIO_PIN_RESET);                               // COM拉低

    HAL_GPIO_WritePin(DIR1OUT_GPIO_Port, DIR1OUT_Pin, dir); // 电机方向 1正转0反转
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);               // 启动pwm模式
    target1_hz = hz;                                        // 更新目标等级
}
void Motor2_SetSpeed(uint8_t en, GPIO_PinState dir, uint16_t hz)
{
    HAL_GPIO_WritePin(EN2OUT_GPIO_Port, EN2OUT_Pin, (en == ENABLE) ? GPIO_PIN_SET : GPIO_PIN_RESET); // 1使能 0失能
    HAL_GPIO_WritePin(COM2OUT_GPIO_Port, COM2OUT_Pin, GPIO_PIN_RESET);                               // COM拉低

    HAL_GPIO_WritePin(DIR2OUT_GPIO_Port, DIR2OUT_Pin, dir); // 电机方向 1正转0反转
    HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_1);               // 启动pwm模式
    target2_hz = hz;                                        // 更新目标等级
}
void Motor3_SetSpeed(uint8_t en, GPIO_PinState dir, uint16_t hz)
{
    HAL_GPIO_WritePin(EN3OUT_GPIO_Port, EN3OUT_Pin, (en == ENABLE) ? GPIO_PIN_SET : GPIO_PIN_RESET); // 1使能 0失能
    HAL_GPIO_WritePin(COM3OUT_GPIO_Port, COM3OUT_Pin, GPIO_PIN_RESET);                               // COM拉低

    HAL_GPIO_WritePin(DIR3OUT_GPIO_Port, DIR3OUT_Pin, dir); // 电机方向 1正转0反转
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);               // 启动pwm模式
    target3_hz = hz;                                        // 更新目标等级
}

uint16_t Motor1_GetStep() // 细分步数为1  脉冲大概为180度
{                         // 电机脉冲值获取
    return current1_step;
}
uint16_t Motor2_GetStep()
{ // 电机脉冲值获取
    return current2_step;
}
uint16_t Motor3_GetStep()
{ // 电机脉冲值获取
    return current3_step;
}

// 在 HAL_TIM_PeriodElapsedCallback 函数外部定义 ramp 函数
static void ramp(volatile uint16_t *cur, volatile uint16_t *tgt)
{
    if (*cur < *tgt)
    {
        uint16_t diff = *tgt - *cur;
        *cur += (diff < ACCEL_HZ) ? diff : ACCEL_HZ;
    }
    else if (*cur > *tgt)
    {
        uint16_t diff = *cur - *tgt;
        *cur -= (diff < ACCEL_HZ) ? diff : ACCEL_HZ;
    }
}

// 开启tim4定时中断用于调整arr与ccr
// 开启tim1,2,3更新中断用于计算脉冲数
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim == &htim6)
    {
        // 三个电机统一使用梯形加速曲线
        ramp((volatile uint16_t *)&current1_hz, (volatile uint16_t *)&target1_hz);
        ramp((volatile uint16_t *)&current2_hz, (volatile uint16_t *)&target2_hz);
        ramp((volatile uint16_t *)&current3_hz, (volatile uint16_t *)&target3_hz);

        // 计算 ARR 和 CCR
        if (current1_hz)
        {
            uint16_t arr1 = (TIMER_CLK_HZ / current1_hz) - 1;
            __HAL_TIM_SET_AUTORELOAD(&htim2, arr1);
            __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, arr1 / 2);
        }
        if (current2_hz)
        {
            uint16_t arr2 = (TIMER_CLK_HZ / current2_hz) - 1;
            __HAL_TIM_SET_AUTORELOAD(&htim8, arr2);
            __HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, arr2 / 2);
        }
        if (current3_hz)
        {
            uint16_t arr3 = (TIMER_CLK_HZ / current3_hz) - 1;
            __HAL_TIM_SET_AUTORELOAD(&htim1, arr3);
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, arr3 / 2);
        }
    }
    else if (htim == &htim2)
    {
        if (motor1_mode == Constant_step && target1_step)
        {
            current1_step++;
            if (current1_step >= target1_step)
            {
                current1_step = 0;
                target1_step = 0;
                Motor1_SetSpeed(0, 0, 0);
                HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_4);
            }
            if (current1_step + motor1_decel_step >= target1_step && target1_hz != 0)
            {
                target1_hz = 0;
            }
        }
    }
    else if (htim == &htim8)
    {
        if (motor2_mode == Constant_step && target2_step)
        {
            current2_step++;
            if (current2_step >= target2_step)
            {
                current2_step = 0;
                target2_step = 0;
                current2_hz = 0;
                target2_hz = 0;
                Motor2_SetSpeed(0, 0, 0);
                HAL_TIM_PWM_Stop(&htim8, TIM_CHANNEL_1);
            }
            if (current2_step + motor2_decel_step >= target2_step && target2_hz != 0)
            {
                target2_hz = 0;
            }
        }
    }
    else if (htim == &htim1)
    {
        if (motor3_mode == Constant_step && target3_step)
        {
            current3_step++;
            if (current3_step >= target3_step)
            {
                current3_step = 0;
                target3_step = 0;
                current3_hz = 0;
                target3_hz = 0;
                Motor3_SetSpeed(0, 0, 0);
                HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
            }
            if (current3_step + motor3_decel_step >= target3_step && target3_hz != 0)
            {
                target3_hz = 0;
            }
        }
    }
}
