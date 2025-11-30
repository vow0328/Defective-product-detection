#include "include.h"

uint8_t Read_INPUT(uint8_t num)
{

    return HAL_GPIO_ReadPin(input_ports[num], input_pins[num]);
}
uint8_t Send_INPUT(void)
{
    uint16_t Input_num = 0x00;

    for (uint8_t i = 8; i > 0; i--)
    {
        Input_num = (Input_num << 1) | Read_INPUT(i);//十六进制数据按位或
    }
    Serial3_SendByte(0xFF);//数据包头校验
    Serial3_SendByte(Input_num); // 发送八位输入的引脚状态
    Serial3_SendByte(0xFE);//包尾校验
}
