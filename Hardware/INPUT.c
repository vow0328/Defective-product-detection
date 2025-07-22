#include "include.h"

uint8_t Read_INPUT(uint8_t num)
{

    return HAL_GPIO_ReadPin(input_ports[num], input_pins[num]);
}
uint8_t Input_Task(void)
{
    uint16_t Input_num = 0x00;

    for (uint8_t i = 8; i > 0; i--)
    {
        Input_num = (Input_num << 1) | Read_INPUT(i);
    }
    Serial3_SendByte(0xFF);
    Serial3_SendByte(Input_num); // 发送八位输入的引脚状态
    Serial3_SendByte(0xFE);
    osDelay(100);
}
