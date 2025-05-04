#include "tim.h"
#include "gpio.h"
#include "math.h"
#include "H_Tmc2209.h"

// 直接以脉冲频率（Hz）控制
volatile uint32_t current1_hz = 0, target1_hz = 0;
volatile uint32_t current2_hz = 0, target2_hz = 0;
volatile uint32_t current3_hz = 0, target3_hz = 0;

// 1号电机预计与当前值
// uint8_t current1_level = 0, target1_level = 0;
uint32_t current1_step = 0, target1_step = 0;
// 2号电机预计与当前值
// uint8_t current2_level = 0, target2_level = 0;
uint32_t current2_step = 0, target2_step = 0;
// 3号电机预计与当前值
// uint8_t current3_level = 0, target3_level = 0;
uint32_t current3_step = 0, target3_step = 0;

// // 档位0：40hz 24999 档位39：1600hz 624
// int speed_levels[40] = {24999, 12499, 8332, 6249, 4999, 4165, 3570, 3124, 2776, 2499, 2271, 2082, 1922, 1784, 1665, 1561, 1469,
//                         1387, 1314, 1249, 1189, 1135, 1085, 1040, 999, 960, 924, 891, 861, 832, 805, 780, 756, 734, 713, 693, 674, 656, 640, 624};
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

void Motor_SetSpeed(uint8_t num, uint8_t mode, GPIO_PinState dir, uint32_t hz) // 模式1定速 模式2定步
{
    hz = (mode == 1) ? hz : 624;            // 模式1速度为输入速度,模式二固定速度
    int target_step = (mode == 1) ? 0 : hz; // 模式1不限步数,模式二步数为输入脉冲数
    switch (num)
    {
    case 1:
        Motor1_SetSpeed(ENABLE, dir, hz);
        target1_step = (target_step) ? target_step : 0;
        break;
    case 2:
        Motor2_SetSpeed(ENABLE, dir, hz);
        target2_step = (target_step) ? target_step : 0;
        break;
    case 3:
        Motor3_SetSpeed(ENABLE, dir, hz);
        target3_step = (target_step) ? target_step : 0;
        break;
    default:
        break;
    }
}

void Motor1_SetSpeed(uint8_t en, GPIO_PinState dir, uint32_t hz)
{
    HAL_GPIO_WritePin(EN1OUT_GPIO_Port, EN1OUT_Pin, (en == ENABLE) ? GPIO_PIN_SET : GPIO_PIN_RESET); // 1使能 0失能
    HAL_GPIO_WritePin(COM1OUT_GPIO_Port, COM1OUT_Pin, GPIO_PIN_RESET);                               // COM拉低

    HAL_GPIO_WritePin(DIR1OUT_GPIO_Port, DIR1OUT_Pin, dir); // 电机方向 1正转0反转

    target1_hz = hz; // 更新目标等级
    current1_step = 0;
}
void Motor2_SetSpeed(uint8_t en, GPIO_PinState dir, uint32_t hz)
{
    HAL_GPIO_WritePin(EN2OUT_GPIO_Port, EN2OUT_Pin, (en == ENABLE) ? GPIO_PIN_SET : GPIO_PIN_RESET); // 1使能 0失能
    HAL_GPIO_WritePin(COM2OUT_GPIO_Port, COM2OUT_Pin, GPIO_PIN_RESET);                               // COM拉低

    HAL_GPIO_WritePin(DIR2OUT_GPIO_Port, DIR2OUT_Pin, dir); // 电机方向 1正转0反转

    target2_hz = hz; // 更新目标等级
    current2_step = 0;
}
void Motor3_SetSpeed(uint8_t en, GPIO_PinState dir, uint32_t hz)
{
    HAL_GPIO_WritePin(EN3OUT_GPIO_Port, EN3OUT_Pin, (en == ENABLE) ? GPIO_PIN_SET : GPIO_PIN_RESET); // 1使能 0失能
    HAL_GPIO_WritePin(COM3OUT_GPIO_Port, COM3OUT_Pin, GPIO_PIN_RESET);                               // COM拉低

    HAL_GPIO_WritePin(DIR3OUT_GPIO_Port, DIR3OUT_Pin, dir); // 电机方向 1正转0反转

    target3_hz = hz; // 更新目标等级
    current3_step = 0;
}

uint32_t Motor1_GetStep() // 细分步数为1是110个脉冲大概为180度
{                         // 电机脉冲值获取
    return current1_step;
}
uint32_t Motor2_GetStep()
{ // 电机脉冲值获取
    return current2_step;
}
uint32_t Motor3_GetStep()
{ // 电机脉冲值获取
    return current3_step;
}

// 在 HAL_TIM_PeriodElapsedCallback 函数外部定义 ramp 函数
static void ramp(volatile uint32_t *cur, volatile uint32_t *tgt)
{
    if (*cur < *tgt)
    {
        uint32_t diff = *tgt - *cur;
        *cur += (diff < ACCEL_HZ_PER_MS) ? diff : ACCEL_HZ_PER_MS;
    }
    else if (*cur > *tgt)
    {
        uint32_t diff = *cur - *tgt;
        *cur -= (diff < ACCEL_HZ_PER_MS) ? diff : ACCEL_HZ_PER_MS;
    }
}

// 开启tim4定时中断用于调整arr与ccr
// 开启tim1,2,3更新中断用于计算脉冲数
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim == &htim6)
    {
        // 三个电机统一使用梯形加速曲线
        ramp((volatile uint32_t *)&current1_hz, (volatile uint32_t *)&target1_hz);
        ramp((volatile uint32_t *)&current2_hz, (volatile uint32_t *)&target2_hz);
        ramp((volatile uint32_t *)&current3_hz, (volatile uint32_t *)&target3_hz);

        // 计算 ARR 和 CCR
        if (current1_hz)
        {
            uint32_t arr1 = current1_hz;
            __HAL_TIM_SET_AUTORELOAD(&htim2, arr1);
            __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, arr1 / 2);
        }
        if (current2_hz)
        {
            uint32_t arr2 = current2_hz;
            __HAL_TIM_SET_AUTORELOAD(&htim8, arr2);
            __HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_4, arr2 / 2);
        }
        if (current3_hz)
        {
            uint32_t arr3 = current3_hz;
            __HAL_TIM_SET_AUTORELOAD(&htim1, arr3);
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, arr3 / 2);
        }
    }
    else if (htim == &htim2)
    {
        // TIM1每个脉冲周期计数
        current1_step++;
        if (current1_step >= target1_step && target1_step != 0)
        {
            current1_hz = 0;
            Motor1_SetSpeed(0, 1, 0);
        }
    }
    else if (htim == &htim8)
    {
        // TIM2每个脉冲周期计数
        current2_step++;
        if (current2_step >= target2_step && target2_step != 0)
        {
            current2_hz = 0;
            Motor2_SetSpeed(0, 1, 0);
        }
    }
    else if (htim == &htim1)
    {
        // TIM8每个脉冲周期计数
        current3_step++;
        if (current3_step >= target3_step && target3_step != 0)
        {
            current3_hz = 0;
            Motor3_SetSpeed(0, 1, 0);
        }
    }
}
