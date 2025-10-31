#include "include.h"
uint8_t Command[5];
void Command_Control(void)
{
    uint8_t Motor, motion, dir;
    uint16_t pulses;
    uint16_t vstart = 200, vmax = 3200, vacc = 100;
    uint8_t flag = Serial2_GetRxData(); // 读取串口状态
    static int nx = 0, ny = 0;
    switch (flag)
    {
    case 0x01:
        Serial2_GetRxPacket(Command, 3);
        int dx = 0, dy = 0;
        dx = Command[0] - nx;
        dy = Command[2] - ny;
        nx = Command[0];
        ny = Command[2];
        // OLED_ShowNum(1, 1, nx, 2);
        // OLED_ShowNum(2, 1, ny, 2);
        if (dx < 0)
        {
            Motor_Set(3, 2, 1, 40 * (-dx), vstart, vmax, vacc); // 转15°
        }
        else if (dx > 0)
        {
            Motor_Set(3, 2, 0, 40 * (dx), vstart, vmax, vacc); // 转15°
        }
        if (dy < 0)
        {
            Motor_Set(2, 2, 0, 1200 * (-dy), vstart, vmax, vacc);
        }
        else if (dy > 0)
        {
            Motor_Set(2, 2, 1, 1200 * (dy), vstart, vmax, vacc);
        }
        Motor_Set(1, 2, 0, 0x29a, vstart, vmax, vacc); // 针管向下
        HAL_Delay(1000);
        led_control(1, 1);
        HAL_Delay(1000);
        led_control(1, 0);
        Motor_Set(1, 2, 1, 0x29a, vstart, vmax, vacc); // 针管向上
        HAL_Delay(1000);
        Serial2_SendByte(0xAC);
        break;
    // case 0x01:                                                     // 控制电机
    //     Serial2_GetRxPacket(Command, 5);                           // 读取串口下一步指令
    //     Motor = Command[0];                                        // 选择几号电机
    //     motion = Command[1];                                       // 运动方式01:定速 02:定步 03:停止
    //     dir = Command[2];                                          // 方向01:正转 00:反转
    //     pulses = (Command[3] << 8) | Command[4];                   // 脉冲数
    //     Motor_Set(Motor, motion, dir, pulses, vstart, vmax, vacc); // 根据输入参数设置电机运动
    //     break;
    // case 0x02:                           // 输出控制
    //     Serial2_GetRxPacket(Command, 1); // 读取串口下一步指令
    //     uint8_t Led_num = Command[0];
    //     for (uint8_t i = 1; i <= 2; i++)
    //     {
    //         led_control(i, (Led_num & (1 << (i - 1)))); // 八位输出控制
    //     }
    //     break;
    case 0x02:
    {
        for (int j = 0; j < 2; j++)
        {
            for (int i = 0; i < 40; i++)
            {
                Motor_Set(1, 2, 0, 0x29a, vstart, vmax, vacc); // 针管向下
                HAL_Delay(1000);
                led_control(1, 1);
                HAL_Delay(1000);
                led_control(1, 0);
                Motor_Set(1, 2, 1, 0x29a, vstart, vmax, vacc); // 针管向上
                HAL_Delay(1000);
                Motor_Set(3, 2, 1, 0x28, vstart, vmax, vacc); // 逆时针转9°
                HAL_Delay(1000);
            }
            if (j < 1)
            {
                Motor_Set(2, 2, 0, 0x500, vstart, vmax, vacc); // 向前5mm
                HAL_Delay(2000);
            }
        }
        Motor_Set(2, 2, 1, 0x1000, vstart, vmax, vacc); // 向后10mm
        break;
    }
    case 0x03:
    {
        for (int j = 0; j < 2; j++)
        {
            for (int i = 0; i < 30; i++)
            {
                Motor_Set(1, 2, 0, 0x29a, vstart, vmax, vacc); // 根据输入参数设置电机运动
                HAL_Delay(1000);
                led_control(1, 1);
                HAL_Delay(1000);
                led_control(1, 0);
                Motor_Set(1, 2, 1, 0x29a, vstart, vmax, vacc); // 根据输入参数设置电机运动
                HAL_Delay(1000);
                Motor_Set(3, 2, 1, 0x32, vstart, vmax, vacc); // 转12°
                HAL_Delay(1000);
            }
            if (j < 1)
            {
                Motor_Set(2, 2, 0, 0x500, vstart, vmax, vacc); // 向前5mm
                HAL_Delay(2000);
            }
        }
        Motor_Set(2, 2, 1, 0x1000, vstart, vmax, vacc); // 向后10mm
        break;
    }
    case 0x04:
    {
        for (int j = 0; j < 2; j++)
        {
            for (int i = 0; i < 24; i++)
            {
                Motor_Set(1, 2, 0, 0x29a, vstart, vmax, vacc); // 根据输入参数设置电机运动
                HAL_Delay(1000);
                led_control(1, 1);
                HAL_Delay(1000);
                led_control(1, 0);
                Motor_Set(1, 2, 1, 0x29a, vstart, vmax, vacc); // 根据输入参数设置电机运动
                HAL_Delay(1000);
                Motor_Set(3, 2, 1, 0x40, vstart, vmax, vacc); // 转15°
                HAL_Delay(1000);
            }
            if (j < 1)
            {
                Motor_Set(2, 2, 0, 0x500, vstart, vmax, vacc); // 向前5mm
                HAL_Delay(2000);
            }
        }
        Motor_Set(2, 2, 1, 0x1000, vstart, vmax, vacc); // 向后10mm
        break;
    }
    case 0x05:
    {
        for (int j = 0; j < 2; j++)
        {
            for (int i = 0; i < 20; i++)
            {
                Motor_Set(1, 2, 0, 0x29a, vstart, vmax, vacc); // 根据输入参数设置电机运动
                HAL_Delay(1000);
                led_control(1, 1);
                HAL_Delay(1000);
                led_control(1, 0);
                Motor_Set(1, 2, 1, 0x29a, vstart, vmax, vacc); // 根据输入参数设置电机运动
                HAL_Delay(1000);
                Motor_Set(3, 2, 1, 0x50, vstart, vmax, vacc); // 转18°
                HAL_Delay(1000);
            }
            if (j < 1)
            {
                Motor_Set(2, 2, 0, 0x500, vstart, vmax, vacc); // 向前5mm
                HAL_Delay(2000);
            }
        }
        Motor_Set(2, 2, 1, 0x1000, vstart, vmax, vacc); // 向后10mm
        break;
    }
    case 0x06:
    {
        for (int j = 0; j < 3; j++)
        {
            for (int i = 0; i < 40; i++)
            {
                Motor_Set(1, 2, 0, 0x29a, vstart, vmax, vacc); // 根据输入参数设置电机运动
                HAL_Delay(1000);
                led_control(1, 1);
                HAL_Delay(1000);
                led_control(1, 0);
                Motor_Set(2, 2, 1, 0x29a, vstart, vmax, vacc); // 根据输入参数设置电机运动
                HAL_Delay(1000);
                Motor_Set(3, 2, 1, 0x28, vstart, vmax, vacc); // 转3°
                HAL_Delay(1000);
            }
            if (j < 1)
            {
                Motor_Set(2, 2, 0, 0x400, vstart, vmax, vacc); // 向前5mm
                HAL_Delay(2000);
            }
        }
        Motor_Set(2, 2, 1, 0x1000, vstart, vmax, vacc); // 向后10mm
        break;
    }
    case 0x07:
    {
        for (int j = 0; j < 3; j++)
        {
            for (int i = 0; i < 30; i++)
            {
                Motor_Set(1, 2, 0, 0x29a, vstart, vmax, vacc); // 根据输入参数设置电机运动
                HAL_Delay(1000);
                led_control(1, 1);
                HAL_Delay(1000);
                led_control(1, 0);
                Motor_Set(1, 2, 1, 0x29a, vstart, vmax, vacc); // 根据输入参数设置电机运动
                HAL_Delay(1000);
                Motor_Set(3, 2, 1, 0x32, vstart, vmax, vacc); // 转4°
                HAL_Delay(1000);
            }
            if (j < 1)
            {
                Motor_Set(2, 2, 0, 0x400, vstart, vmax, vacc); // 向前5mm
                HAL_Delay(2000);
            }
        }
        Motor_Set(2, 2, 1, 0x1000, vstart, vmax, vacc); // 向后10mm
        break;
    }
    case 0x08:
    {
        for (int j = 0; j < 3; j++)
        {
            for (int i = 0; i < 24; i++)
            {
                Motor_Set(1, 2, 0, 0x29a, vstart, vmax, vacc); // 根据输入参数设置电机运动
                HAL_Delay(1000);
                led_control(1, 1);
                HAL_Delay(1000);
                led_control(1, 0);
                Motor_Set(1, 2, 1, 0x29a, vstart, vmax, vacc); // 根据输入参数设置电机运动
                HAL_Delay(1000);
                Motor_Set(3, 2, 1, 0x40, vstart, vmax, vacc); // 转5°
                HAL_Delay(1000);
            }
            if (j < 1)
            {
                Motor_Set(2, 2, 0, 0x400, vstart, vmax, vacc); // 向前5mm
                HAL_Delay(2000);
            }
        }
        Motor_Set(2, 2, 1, 0x1000, vstart, vmax, vacc); // 向后10mm
        break;
    }
    case 0x09:
    {
        for (int j = 0; j < 3; j++)
        {
            for (int i = 0; i < 20; i++)
            {
                Motor_Set(1, 2, 0, 0x29a, vstart, vmax, vacc); // 根据输入参数设置电机运动
                HAL_Delay(1000);
                led_control(1, 1);
                HAL_Delay(1000);
                led_control(1, 0);
                Motor_Set(1, 2, 1, 0x29a, vstart, vmax, vacc); // 根据输入参数设置电机运动
                HAL_Delay(1000);
                Motor_Set(3, 2, 1, 0x50, vstart, vmax, vacc); // 转6°
                HAL_Delay(1000);
            }
            if (j < 1)
            {
                Motor_Set(2, 2, 0, 0x400, vstart, vmax, vacc); // 向前5mm
                HAL_Delay(2000);
            }
        }
        Motor_Set(2, 2, 1, 0x1000, vstart, vmax, vacc); // 向后10mm
        break;
    }
    default: // 错误数据不处理
        break;
    }
}
