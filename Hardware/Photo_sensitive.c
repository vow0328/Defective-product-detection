#include "include.h"

uint8_t read_Photo_sensor(uint8_t num)
{

    switch (num)
    {
    case 1:
        return HAL_GPIO_ReadPin(INPUT1_GPIO_Port, INPUT1_Pin);
        break;
    case 2:
        return HAL_GPIO_ReadPin(INPUT2_GPIO_Port, INPUT2_Pin);
        break;
    case 3:
        return HAL_GPIO_ReadPin(INPUT3_GPIO_Port, INPUT3_Pin);
        break;
    case 4:
        return HAL_GPIO_ReadPin(INPUT4_GPIO_Port, INPUT4_Pin);
        break;
    case 5:
        return HAL_GPIO_ReadPin(INPUT5_GPIO_Port, INPUT5_Pin);
        break;
    case 6:
        return HAL_GPIO_ReadPin(INPUT6_GPIO_Port, INPUT6_Pin);
        break;
    case 7:
        return HAL_GPIO_ReadPin(INPUT7_GPIO_Port, INPUT7_Pin);
        break;
    case 8:
        return HAL_GPIO_ReadPin(INPUT8_GPIO_Port, INPUT8_Pin);
        break;
    default:
        break;
    }
}
uint8_t send_Photo_sensor(void)
{
    uint16_t Input_num = 0x00;

    for (uint8_t i = 8; i > 0; i--)
    {
        Input_num = (Input_num << 1) | read_Photo_sensor(i);
    }
    Serial3_SendByte(0xFF);   
    Serial3_SendByte(Input_num); // 发送八位输入的引脚状态
    Serial3_SendByte(0xFE);      
}
