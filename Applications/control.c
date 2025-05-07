#include "include.h"
uint16_t Command[4];
void Control(void)
{
    uint8_t Motor, motion, dir;
    uint16_t pulses;
    uint8_t flag = Serial3_GetRxPacket(Command, 4); // 读取串口指令
    while (!flag)
        flag = Serial3_GetRxPacket(Command, 4);
    //HAL_Delay(1000);
    Motor = Command[0];                  // 选择几号电机01:传送带 02:顶部相机
    motion = Command[1] == 0x01 ? 1 : 2; // 运动方式01:定速 02:定步
    dir = Command[2] == 0x01 ? 1 : 0;    // 方向01:正转 00:反转
    pulses = Command[3];                 // 脉冲数
    Motor_SetSpeed(Motor, motion, dir, pulses);
}
