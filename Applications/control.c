#include "include.h"
uint8_t Command[5];
void Control(void)
{
    uint8_t Motor, motion, dir;
    uint16_t pulses;
    uint8_t flag = Serial3_GetRxData(); // 读取串口状态
    switch (flag)
    {
    case 0x01:                                      // 控制电机
        Serial3_GetRxPacket(Command, 5);            // 读取串口下一步指令
        Motor = Command[0];                         // 选择几号电机01:传送带 02:顶部相机
        motion = Command[1];        // 运动方式01:定速 02:定步 03:停止
        dir = Command[2] == 0x01 ? 1 : 0;           // 方向01:正转 00:反转
        pulses = (Command[3] << 8) | Command[4];    // 脉冲数
        Motor_SetSpeed(Motor, motion, dir, pulses); // 根据输入参数设置电机运动
        break;
    case 0x02:                           // 四路输出控制
        Serial3_GetRxPacket(Command, 1); // 读取串口下一步指令
        uint8_t Led_num = Command[0];
        for (uint8_t i = 0; i < 4; i++)
        {
            led_control(i, (Led_num & (1 << i)));
        }
        break;
    default: // 错误数据不处理
        break;
    }
}
