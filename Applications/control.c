#include "include.h"
SerialPacket Command; 
void Control_task(void)
{
    uint8_t Motor, motion, dir, ouput_num;
    uint16_t pulses;
    uint16_t vstart = 200, vmax = 1600, vacc = 100;
    while (1)
    {
        xQueueReceive(serial3PacketQueue, &Command, portMAX_DELAY);
        switch (Command.data[0])
        {
        case 0x01:                                                     // 控制电机
            Motor = Command.data[1];                                   // 选择几号电机
            motion = Command.data[2];                                  // 运动方式01:定速 02:定步 03:停止
            dir = Command.data[3];                                     // 方向01:正转 00:反转
            pulses = (Command.data[4] << 8) | Command.data[5];         // 脉冲数
            Motor_Set(Motor, motion, dir, pulses, vstart, vmax, vacc); // 根据输入参数设置电机运动
            break;
        case 0x02: // 输出控制
            ouput_num = Command.data[1];
            for (uint8_t i = 1; i <= 8; i++)
            {
                OUTPUT_control(i, (ouput_num & (1 << (i - 1)))); // 八位输出控制
            }
            break;
        default: // 错误数据不处理
            break;
        }
        osDelay(1);
    }
}
