#include "include.h"
uint16_t vstart = 200, vmax = 1600, vacc = 10;
StepperMotor motor1 = {0};
StepperMotor motor2 = {0};
StepperMotor motor3 = {0};

void stepper_init(StepperMotor *motor, uint16_t v_start, uint16_t v_max, uint16_t acc, uint16_t steps)
{
    motor->velocity.v_start = v_start;
    motor->velocity.v_max = v_max;
    motor->velocity.acc = acc;
    motor->velocity.total_steps = steps;
    motor->steps = calc_trapezoid_profile(motor->velocity);
    motor->step_index = 0;
}

void Motor_Init()
{

    __HAL_TIM_CLEAR_IT(&htim2, TIM_CHANNEL_4);
    __HAL_TIM_CLEAR_IT(&htim1, TIM_CHANNEL_1); // 清除中断标志位
    __HAL_TIM_CLEAR_IT(&htim8, TIM_CHANNEL_1);
}

void Motor_SetSpeed(uint8_t num, uint8_t mode, GPIO_PinState dir, uint16_t hz) // 模式1定速 模式2定步 模式3停止
{
    uint8_t motor_mode = 0, en = 1;
    uint16_t steps = 0;
    switch (mode)
    {
    case Constant_speed:
        motor_mode = Constant_speed;
        break;
    case Constant_step:
        motor_mode = Constant_step;
        steps = hz; // 模式一不限步数,模式二步数为输入脉冲数;
        hz = 1600;  // 模式一速度为输入速度,模式二固定速度 200为一秒一圈
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
        motor1.motor_mode = motor_mode;
        if (motor1.motor_mode == Constant_step)
        {
            stepper_init(&motor1, vstart, vmax, vacc, steps);
        }
        else if (motor1.motor_mode == Constant_speed)
        {
            motor1.velocity.v_max = hz;
        }
        Motor1_SetSpeed(en, dir);
        break;
    case 2:
        motor2.motor_mode = motor_mode;
        if (motor2.motor_mode == Constant_step)
        {
            stepper_init(&motor2, vstart, vmax, vacc, steps);
        }
        Motor2_SetSpeed(en, dir);
        break;
    case 3:
        motor3.motor_mode = motor_mode;
        if (motor3.motor_mode == Constant_step)
        {
            stepper_init(&motor3, vstart, vmax, vacc, steps);
        }
        Motor3_SetSpeed(en, dir);
        break;
    default:
        break;
    }
}

void Motor1_SetSpeed(uint8_t en, GPIO_PinState dir)
{
    HAL_GPIO_WritePin(EN1OUT_GPIO_Port, EN1OUT_Pin, (en == ENABLE) ? GPIO_PIN_SET : GPIO_PIN_RESET); // 1使能 0失能
    HAL_GPIO_WritePin(COM1OUT_GPIO_Port, COM1OUT_Pin, GPIO_PIN_RESET);                               // COM拉低
    HAL_GPIO_WritePin(DIR1OUT_GPIO_Port, DIR1OUT_Pin, dir);                                          // 电机方向 1正转0反转

    if (motor1.hz_index != motor1.velocity.v_max)
    {
        motor1.hz_index = get_step_speed(motor1.step_index, motor1.steps, motor1.velocity); // 更新当前速度
        uint16_t arr1 = (TIMER_CLK_HZ / motor1.hz_index) - 1;
        __HAL_TIM_SET_AUTORELOAD(&htim2, arr1);
        __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, arr1 / 2);
        __HAL_TIM_SET_COUNTER(&htim2, 0); // 重置计数器
    }
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4); // 启动pwm模式
    HAL_TIM_Base_Start_IT(&htim2);            // 启动计步
}

void Motor2_SetSpeed(uint8_t en, GPIO_PinState dir)
{
    HAL_GPIO_WritePin(EN2OUT_GPIO_Port, EN2OUT_Pin, (en == ENABLE) ? GPIO_PIN_SET : GPIO_PIN_RESET); // 1使能 0失能
    HAL_GPIO_WritePin(COM2OUT_GPIO_Port, COM2OUT_Pin, GPIO_PIN_RESET);                               // COM拉低
    HAL_GPIO_WritePin(DIR2OUT_GPIO_Port, DIR2OUT_Pin, dir);                                          // 电机方向 1正转0反转

    if (motor2.hz_index != motor2.velocity.v_max)
    {
        motor2.hz_index = get_step_speed(motor2.step_index, motor2.steps, motor2.velocity); // 更新当前速度
        uint16_t arr2 = (TIMER_CLK_HZ / motor2.hz_index) - 1;
        __HAL_TIM_SET_AUTORELOAD(&htim8, arr2);
        __HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, arr2 / 2);
        __HAL_TIM_SET_COUNTER(&htim8, 0); // 重置计数器
    }
    HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_1); // 启动pwm模式
    HAL_TIM_Base_Start_IT(&htim8);            // 启动计步
}
void Motor3_SetSpeed(uint8_t en, GPIO_PinState dir)
{
    HAL_GPIO_WritePin(EN3OUT_GPIO_Port, EN3OUT_Pin, (en == ENABLE) ? GPIO_PIN_SET : GPIO_PIN_RESET); // 1使能 0失能
    HAL_GPIO_WritePin(COM3OUT_GPIO_Port, COM3OUT_Pin, GPIO_PIN_RESET);                               // COM拉低
    HAL_GPIO_WritePin(DIR3OUT_GPIO_Port, DIR3OUT_Pin, dir);                                          // 电机方向 1正转0反转

    if (motor3.hz_index != motor3.velocity.v_max)
    {
        motor3.hz_index = get_step_speed(motor3.step_index, motor3.steps, motor3.velocity); // 更新当前速度
        uint16_t arr3 = (TIMER_CLK_HZ / motor3.hz_index) - 1;
        __HAL_TIM_SET_AUTORELOAD(&htim1, arr3);
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, arr3 / 2);
        __HAL_TIM_SET_COUNTER(&htim1, 0); // 重置计数器
    }
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1); // 启动pwm模式
    HAL_TIM_Base_Start_IT(&htim1);            // 启动计步
}

uint16_t Motor1_GetStep() // 细分步数为1  脉冲大概为180度
{                         // 电机脉冲值获取
    return motor1.step_index;
}
uint16_t Motor2_GetStep()
{ // 电机脉冲值获取
    return motor2.step_index;
}
uint16_t Motor3_GetStep()
{ // 电机脉冲值获取
    return motor3.step_index;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{

    if (htim == &htim2)
    {
        motor1.step_index++;
        if (motor1.hz_index != motor1.velocity.v_max)
        {
            motor1.hz_index = get_step_speed(motor1.step_index, motor1.steps, motor1.velocity);
            uint16_t arr1 = (TIMER_CLK_HZ / motor1.hz_index) - 1;
            __HAL_TIM_SET_AUTORELOAD(&htim2, arr1);
            __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, arr1 / 2);
            __HAL_TIM_SET_COUNTER(&htim2, 0); // 重置计数器
        }
        if (motor1.step_index >= motor1.velocity.total_steps && motor1.motor_mode == Constant_step)
        {
            motor1.hz_index = 0;
            motor1.velocity.total_steps = 0;
            HAL_GPIO_WritePin(EN1OUT_GPIO_Port, EN1OUT_Pin, GPIO_PIN_RESET); // 失能en引脚
            HAL_TIM_Base_Stop_IT(&htim2);                                    // 关闭计步
            HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_4);                         // 关闭pwm输出
        }
    }
    else if (htim == &htim8)
    {
        motor2.step_index++;
        if (motor2.hz_index != motor2.velocity.v_max)
        {
            motor2.hz_index = get_step_speed(motor2.step_index, motor2.steps, motor2.velocity);
            uint16_t arr2 = (TIMER_CLK_HZ / motor2.hz_index) - 1;
            __HAL_TIM_SET_AUTORELOAD(&htim8, arr2);
            __HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, arr2 / 2);
            __HAL_TIM_SET_COUNTER(&htim8, 0); // 重置计数器
        }
        if (motor2.step_index >= motor2.velocity.total_steps && motor2.motor_mode == Constant_step)
        {
            motor2.hz_index = 0;
            motor2.velocity.total_steps = 0;
            HAL_GPIO_WritePin(EN2OUT_GPIO_Port, EN2OUT_Pin, GPIO_PIN_RESET); // 失能en引脚
            HAL_TIM_Base_Stop_IT(&htim8);                                    // 关闭计步
            HAL_TIM_PWM_Stop(&htim8, TIM_CHANNEL_1);                         // 关闭pwm输出
        }
    }
    else if (htim == &htim1)
    {
        motor3.step_index++;
        if (motor3.hz_index != motor3.velocity.v_max)
        {
            motor3.hz_index = get_step_speed(motor3.step_index, motor3.steps, motor3.velocity);
            uint16_t arr3 = (TIMER_CLK_HZ / motor3.hz_index) - 1;
            __HAL_TIM_SET_AUTORELOAD(&htim1, arr3);
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, arr3 / 2);
            __HAL_TIM_SET_COUNTER(&htim1, 0); // 重置计数器
        }
        if (motor3.step_index >= motor3.velocity.total_steps && motor3.motor_mode == Constant_step)
        {
            motor3.hz_index = 0;
            motor3.velocity.total_steps = 0;
            HAL_GPIO_WritePin(EN3OUT_GPIO_Port, EN3OUT_Pin, GPIO_PIN_RESET);
            HAL_TIM_Base_Stop_IT(&htim1);
            HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
        }
    }
}
