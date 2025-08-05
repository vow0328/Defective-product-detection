#include "include.h"

MotorStruct Motor[10] = {0};

void stepper_init(MotorStruct *motor, uint16_t v_start, uint16_t v_max, uint16_t acc, uint16_t steps) // 加速度算法初始化
{
    motor->velocity.v_start = v_start;
    motor->velocity.v_max = v_max;
    motor->velocity.acc = acc;
    motor->velocity.total_steps = motor->target_step = steps;
    motor->steps = calc_trapezoid_profile(motor->velocity);
    motor->current_step = 0;
}

void Motor_Set(uint8_t num, uint8_t mode, GPIO_PinState dir, uint16_t hz, uint16_t vstart, uint16_t vmax, uint16_t vacc) // 模式1定速 模式2定步 模式3停止
{
    Motor[num].en = ENABLE;
    switch (mode)
    {
    case Constant_speed: // 定速模式
        Motor[num].mode = Constant_speed;
        Motor[num].hz = hz;
        break;
    case Constant_step: // 定步模式
        Motor[num].mode = Constant_step;
        stepper_init(&Motor[num], vstart, vmax, vacc, hz);
        // Motor[num].hz = get_step_speed(Motor[num].current_step, Motor[num].steps, Motor[num].velocity);
        Motor[num].hz = 800;
        break;
    case STOP_mode: // 停止模式
        Motor[num].mode = STOP_mode;
        Motor[num].en = 0;
        Motor[num].hz = 0;
        HAL_TIM_Base_Stop_IT(motor_tim[num]); // 关闭pwm波和定时器
        HAL_TIM_PWM_Stop(motor_tim[num], motor_channel[num]);
        break;
    default:
        break;
    }
    Motor[num].dir = dir;
    Motor[num].arr = (TIMER_CLK_HZ / Motor[num].hz) - 1; // 更新定时器
    Motor_SetSpeed(num);
}

void Motor_SetSpeed(uint8_t num)
{
    if (num == 0 || num > MOTOR_COUNT)
        return;

    // 设置使能和方向
    HAL_GPIO_WritePin(en_ports[num], en_pins[num], (Motor[num].en == ENABLE) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(dir_port[num], dir_pins[num], Motor[num].dir);

    // 设置定时器参数
    __HAL_TIM_SET_AUTORELOAD(motor_tim[num], Motor[num].arr);
    __HAL_TIM_SET_COMPARE(motor_tim[num], motor_channel[num], Motor[num].arr / 2);
    __HAL_TIM_SET_COUNTER(motor_tim[num], 0);

    // 启动 PWM 和中断
    HAL_TIM_PWM_Start(motor_tim[num], motor_channel[num]);
    HAL_TIM_Base_Start_IT(motor_tim[num]);
}

uint16_t Motor_GetStep(uint8_t num) // 细分步数为1  脉冲大概为180度
{                                   // 电机脉冲值获取
    switch (num)
    {
    case 1:
        return Motor[1].current_step;
    case 2:
        return Motor[2].current_step;
    case 3:
        return Motor[3].current_step;
    case 4:
        return Motor[4].current_step;
    case 5:
        return Motor[5].current_step;
    case 6:
        return Motor[6].current_step;
    default:
        return false;
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    for (int i = 1; i <= 6; i++)
    {
        if (htim == motor_tim[i])
        {
            if (Motor[i].mode == Constant_step && Motor[i].target_step)
            {
                Motor[i].current_step++;
                if (Motor[i].current_step >= Motor[i].target_step)
                {
                    Motor[i].current_step = 0;
                    Motor[i].target_step = 0;
                    //HAL_GPIO_WritePin(en_ports[i], en_pins[i], GPIO_PIN_RESET);//失能引脚,pwm,定时器关闭
                    HAL_TIM_Base_Stop_IT(motor_tim[i]);
                    HAL_TIM_PWM_Stop(motor_tim[i], motor_channel[i]);
                }
                // else
                // {
                //     Motor[i].hz = get_step_speed(Motor[i].current_step, Motor[i].steps, Motor[i].velocity);//获取预计速度
                //     uint16_t arr = (TIMER_CLK_HZ / Motor[i].hz) - 1;
                //     __HAL_TIM_SET_AUTORELOAD(motor_tim[i], arr);//更新定时器
                //     __HAL_TIM_SET_COMPARE(motor_tim[i], motor_channel[i], arr / 2);
                //     __HAL_TIM_SET_COUNTER(motor_tim[i], 0);
                // }
            }
            break;
        }
    }
}
