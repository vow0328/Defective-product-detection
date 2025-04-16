#include "tim.h"
#include "gpio.h"
#include "math.h"
#include "H_Tmc2209.h"

// 1号电机预计与当前值
uint8_t current1_level = 0, target1_level = 0;
uint32_t step1 = 0;
// 2号电机预计与当前值
uint8_t current2_level = 0, target2_level = 0;
uint32_t step2 = 0;
// 3号电机预计与当前值
uint8_t current3_level = 0, target3_level = 0;
uint32_t step3 = 0;

// 档位0：40hz 24999 档位39：1600hz 624
int speed_levels[40] = {24999, 12499, 8332, 6249, 4999, 4165, 3570, 3124, 2776, 2499, 2271, 2082, 1922, 1784, 1665, 1561, 1469,
                        1387, 1314, 1249, 1189, 1135, 1085, 1040, 999, 960, 924, 891, 861, 832, 805, 780, 756, 734, 713, 693, 674, 656, 640, 624};
void Motor_Init()
{
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
    HAL_TIM_Base_Start_IT(&htim4);
}

void Motor1_SetSpeed(uint8_t en, GPIO_PinState dir, uint8_t level)
{
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, (en == ENABLE) ? GPIO_PIN_SET : GPIO_PIN_RESET); // 1使能 0失能
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);

    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, dir); // 电机方向 1正转0反转

    level = (level > MAX_SPEED_LEVEL) ? MAX_SPEED_LEVEL : level; // 限幅
    target1_level = level;                                       // 更新目标等级
}
void Motor2_SetSpeed(uint8_t en, GPIO_PinState dir, uint8_t level)
{
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, (en == ENABLE) ? GPIO_PIN_RESET : GPIO_PIN_SET); // 1使能 0失能
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_RESET);

    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, dir); // 电机方向 1正转0反转

    level = (level > MAX_SPEED_LEVEL) ? MAX_SPEED_LEVEL : level; // 限幅
    target2_level = level;                                       // 更新目标等级
}
void Motor3_SetSpeed(uint8_t en, GPIO_PinState dir, uint8_t level)
{
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, (en == ENABLE) ? GPIO_PIN_RESET : GPIO_PIN_SET); // 1使能 0失能
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);

    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, dir); // 电机方向 1正转0反转

    level = (level > MAX_SPEED_LEVEL) ? MAX_SPEED_LEVEL : level; // 限幅
    target3_level = level;                                       // 更新目标等级
}

// 开启tim4定时中断用于调整arr与ccr
// 开启tim1,2,3更新中断用于计算脉冲数
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim == &htim4)
    {
        // 逐步逼近目标档位
        if (current1_level < target1_level)
            current1_level++;
        else if (current1_level > target1_level)
            current1_level--;

        // 逐步逼近目标档位
        if (current2_level < target2_level)
            current2_level++;
        else if (current2_level > target2_level)
            current2_level--;

        // 逐步逼近目标档位
        if (current3_level < target3_level)
            current3_level++;
        else if (current3_level > target3_level)
            current3_level--;

        // 更新TIM2参数
        uint32_t new_arr1 = speed_levels[current1_level];
        uint32_t new_arr2 = speed_levels[current2_level];
        uint32_t new_arr3 = speed_levels[current3_level];

        __HAL_TIM_SET_AUTORELOAD(&htim1, new_arr1);                 // 设置ARR
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, new_arr1 / 2); // 50%占空比

        __HAL_TIM_SET_AUTORELOAD(&htim2, new_arr2);                 // 设置ARR
        __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, new_arr2 / 2); // 50%占空比

        __HAL_TIM_SET_AUTORELOAD(&htim3, new_arr3);                 // 设置ARR
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, new_arr3 / 2); // 50%占空比
    }
    else if (htim == &htim1)
    {
        // TIM1每个脉冲周期计数
        step1++;
    }
    else if (htim == &htim2)
    {
        // TIM2每个脉冲周期计数
        step2++;
    }
    else if (htim == &htim3)
    {
        // TIM3每个脉冲周期计数
        step3++;
    }
}
