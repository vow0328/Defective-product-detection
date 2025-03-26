#include "Step_Diver.h"

volatile uint8_t STEP1_state, STEP2_state, STEP3_state, Turn1, Turn2, Turn3, Step_flag1, Step_flag2, Step_flag3;
volatile float Tim1, Tim2, Tim3;
volatile float Turn_number1, Turn_number2, Turn_number3;

void Micorstep1_Enable(void)
{
    Turn_number1 = 0;
}

void Micorstep2_Enable(void)
{
    Turn_number2 = 0;
}

void Micorstep3_Enable(void)
{
    Turn_number3 = 0;
}

void Step1_Down(float number_of_turns)
{
    while (Step_flag1 == 0)
    {
        Turn_number1 = number_of_turns;
        if (Turn1 == 0)
        {
            /* 使能电机、设置方向：向下，DIR1IN为低电平 */
            HAL_GPIO_WritePin(EN1IN_GPIO_Port, EN1IN_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(DIR1IN_GPIO_Port, DIR1IN_Pin, GPIO_PIN_RESET);

            /* 启用TIM1中断，启动定时器 */
            __HAL_TIM_ENABLE_IT(&htim1, TIM_IT_UPDATE);
            HAL_NVIC_EnableIRQ(TIM1_UP_IRQn);
            HAL_TIM_Base_Start_IT(&htim1);
        }
        else if (Turn1 == 1)
        {
            Tim1 = 0;
            /* 关闭定时器中断 */
            __HAL_TIM_DISABLE_IT(&htim1, TIM_IT_UPDATE);
            HAL_NVIC_DisableIRQ(TIM1_UP_IRQn);
            HAL_TIM_Base_Stop_IT(&htim1);
            /* HAL_TIM_IRQHandler内部会自动清除中断标志 */
            Step_flag1 = 1;
            Turn1 = 0;
        }
    }
}

void Step1_Up(float number_of_turns)
{
    while (Step_flag1 == 0)
    {
        Turn_number1 = number_of_turns;
        if (Turn1 == 0)
        {
            HAL_GPIO_WritePin(EN1IN_GPIO_Port, EN1IN_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(DIR1IN_GPIO_Port, DIR1IN_Pin, GPIO_PIN_SET);

            __HAL_TIM_ENABLE_IT(&htim1, TIM_IT_UPDATE);
            HAL_NVIC_EnableIRQ(TIM1_UP_IRQn);
            HAL_TIM_Base_Start_IT(&htim1);
        }
        else if (Turn1 == 1)
        {
            Tim1 = 0;
            __HAL_TIM_DISABLE_IT(&htim1, TIM_IT_UPDATE);
            HAL_NVIC_DisableIRQ(TIM1_UP_IRQn);
            HAL_TIM_Base_Stop_IT(&htim1);
            Step_flag1 = 1;
            Turn1 = 0;
        }
    }
}

void Step2_Down(float number_of_turns)
{
    while (Step_flag2 == 0)
    {
        Turn_number2 = number_of_turns;
        if (Turn2 == 0)
        {
            /* 使能电机、设置方向：向下，DIR1IN为低电平 */
            HAL_GPIO_WritePin(EN2IN_GPIO_Port, EN2IN_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(DIR2IN_GPIO_Port, DIR2IN_Pin, GPIO_PIN_RESET);

            /* 启用TIM1中断，启动定时器 */
            __HAL_TIM_ENABLE_IT(&htim2, TIM_IT_UPDATE);
            HAL_NVIC_EnableIRQ(TIM2_IRQn);
            HAL_TIM_Base_Start_IT(&htim2);
        }
        else if (Turn2 == 1)
        {
            Tim2 = 0;
            /* 关闭定时器中断 */
            __HAL_TIM_DISABLE_IT(&htim2, TIM_IT_UPDATE);
            HAL_NVIC_DisableIRQ(TIM2_IRQn);
            HAL_TIM_Base_Stop_IT(&htim2);
            /* HAL_TIM_IRQHandler内部会自动清除中断标志 */
            Step_flag2 = 1;
            Turn2 = 0;
        }
    }
}

void Step2_Up(float number_of_turns)
{
    while (Step_flag2 == 0)
    {
        Turn_number2 = number_of_turns;
        if (Turn2 == 0)
        {
            HAL_GPIO_WritePin(EN2IN_GPIO_Port, EN2IN_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(DIR2IN_GPIO_Port, DIR2IN_Pin, GPIO_PIN_SET);

            __HAL_TIM_ENABLE_IT(&htim2, TIM_IT_UPDATE);
            HAL_NVIC_EnableIRQ(TIM2_IRQn);
            HAL_TIM_Base_Start_IT(&htim2);
        }
        else if (Turn2 == 1)
        {
            Tim2 = 0;
            __HAL_TIM_DISABLE_IT(&htim2, TIM_IT_UPDATE);
            HAL_NVIC_DisableIRQ(TIM2_IRQn);
            HAL_TIM_Base_Stop_IT(&htim2);
            Step_flag2 = 1;
            Turn2 = 0;
        }
    }
}

void Step3_Down(float number_of_turns)
{
    while (Step_flag3 == 0)
    {
        Turn_number3 = number_of_turns;
        if (Turn3 == 0)
        {
            /* 使能电机、设置方向：向下，DIR1IN为低电平 */
            HAL_GPIO_WritePin(EN3IN_GPIO_Port, EN3IN_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(DIR3IN_GPIO_Port, DIR3IN_Pin, GPIO_PIN_RESET);

            /* 启用TIM1中断，启动定时器 */
            __HAL_TIM_ENABLE_IT(&htim3, TIM_IT_UPDATE);
            HAL_NVIC_EnableIRQ(TIM3_IRQn);
            HAL_TIM_Base_Start_IT(&htim3);
        }
        else if (Turn3 == 1)
        {
            Tim3 = 0;
            /* 关闭定时器中断 */
            __HAL_TIM_DISABLE_IT(&htim3, TIM_IT_UPDATE);
            HAL_NVIC_DisableIRQ(TIM3_IRQn);
            HAL_TIM_Base_Stop_IT(&htim3);
            /* HAL_TIM_IRQHandler内部会自动清除中断标志 */
            Step_flag3 = 1;
            Turn3 = 0;
        }
    }
}


void Step3_Up(float number_of_turns)
{
    while (Step_flag3 == 0)
    {
        Turn_number3 = number_of_turns;
        if (Turn3 == 0)
        {
            HAL_GPIO_WritePin(EN3IN_GPIO_Port, EN3IN_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(DIR3IN_GPIO_Port, DIR3IN_Pin, GPIO_PIN_SET);

            __HAL_TIM_ENABLE_IT(&htim3, TIM_IT_UPDATE);
            HAL_NVIC_EnableIRQ(TIM3_IRQn);
            HAL_TIM_Base_Start_IT(&htim3);
        }
        else if (Turn3 == 1)
        {
            Tim3 = 0;
            __HAL_TIM_DISABLE_IT(&htim3, TIM_IT_UPDATE);
            HAL_NVIC_DisableIRQ(TIM3_IRQn);
            HAL_TIM_Base_Stop_IT(&htim3);
            Step_flag3 = 1;
            Turn3 = 0;
        }
    }
}

void SetMotor1Speed(uint32_t pulseFrequency)
{
    // 假设系统时钟72MHz，预分频固定为7200-1，则计数频率为：
    // 72,000,000/7200 = 10,000Hz
    // 则新ARR = (10000 / pulseFrequency) - 1
    uint32_t newPeriod = (10000 / pulseFrequency) - 1;
    __HAL_TIM_SET_AUTORELOAD(&htim1, newPeriod);
    __HAL_TIM_SET_COUNTER(&htim1, 0);
}

void SetMotor2Speed(uint32_t pulseFrequency)
{
    // 假设系统时钟72MHz，预分频固定为7200-1，则计数频率为：
    // 72,000,000 / 7200 = 10,000 Hz
    // 新的ARR = (10000 / pulseFrequency) - 1
    uint32_t newPeriod = (10000 / pulseFrequency) - 1;
    __HAL_TIM_SET_AUTORELOAD(&htim2, newPeriod);
    __HAL_TIM_SET_COUNTER(&htim2, 0);
}

void SetMotor3Speed(uint32_t pulseFrequency)
{
    // 假设系统时钟72MHz，预分频固定为7200-1，则计数频率为：
    // 72,000,000/7200 = 10,000Hz
    // 则新ARR = (10000 / pulseFrequency) - 1
    uint32_t newPeriod = (10000 / pulseFrequency) - 1;
    __HAL_TIM_SET_AUTORELOAD(&htim3, newPeriod);
    __HAL_TIM_SET_COUNTER(&htim3, 0);
}

void TIM1_UP_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim1);
}

void TIM2_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim2);
}

void TIM3_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim3);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM1) // 判断中断是否来自于定时器1
    {
        STEP1_state = !STEP1_state;
        HAL_GPIO_WritePin(STEP1_GPIO_Port, STEP1_Pin, (GPIO_PinState)STEP1_state);
        Tim1++; // 累加脉冲数
        /* 当累计脉冲数达到目标值时结束运动
           此处1598为每转脉冲数，依据实际电机细分设置调整 */
        if (Tim1 >= Turn_number1 * 1598)
        {
            Tim1 = 0;
            Turn1 = 1;
        }
    }
    if (htim->Instance == TIM2) // 判断中断是否来自于定时器2
    {
        STEP2_state = !STEP2_state;
        HAL_GPIO_WritePin(STEP2_GPIO_Port, STEP2_Pin, (GPIO_PinState)STEP2_state);
        Tim2++; // 累加脉冲数
        /* 当累计脉冲数达到目标值时结束运动
           此处1598为每转脉冲数，依据实际电机细分设置调整 */
        if (Tim2 >= Turn_number2 * 1598)
        {
            Tim2 = 0;
            Turn2 = 1;
        }
    }
    if (htim->Instance == TIM3) // 判断中断是否来自于定时器3
    {
        STEP3_state = !STEP3_state;
        HAL_GPIO_WritePin(STEP3_GPIO_Port, STEP3_Pin, (GPIO_PinState)STEP3_state);
        Tim3++; // 累加脉冲数
        /* 当累计脉冲数达到目标值时结束运动
           此处1598为每转脉冲数，依据实际电机细分设置调整 */
        if (Tim3 >= Turn_number3 * 1598)
        {
            Tim3 = 0;
            Turn3 = 1;
        }
    }
}
