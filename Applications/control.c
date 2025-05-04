#include "include.h"
uint32_t Command[4];
void Control(void)
{
    static uint8_t Motor, motion, dir;
    static uint32_t pulses;
    uint8_t flag = Serial2_GetRxPacket(Command, 4); // 读取串口指令
    while (!flag)
        flag = Serial2_GetRxPacket(Command, 4);
    Motor = Command[0] == 0x01 ? 1 : 2;  // 选择几号电机01:传送带 02:顶部相机
    motion = Command[1] == 0x01 ? 1 : 2; // 运动方式01:定速 02:定步
    dir = Command[2] == 0x01 ? 1 : 0;    // 方向01:正转 00:反转
    pulses = Command[3];                 // 脉冲数
    Motor_SetSpeed(Motor, motion, dir, pulses);
}