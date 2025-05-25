#include "include.h"
void led_control(uint8_t num, GPIO_PinState state)
{
    switch (num)
    {
    case 1:
        HAL_GPIO_WritePin(JDOUT1_GPIO_Port, JDOUT1_Pin,state);
        break;
    case 2:
        HAL_GPIO_WritePin(JDOUT2_GPIO_Port, JDOUT2_Pin,state);
        break;
    case 3:
        HAL_GPIO_WritePin(JDOUT3_GPIO_Port, JDOUT3_Pin,state);
        break;
    case 4:
        HAL_GPIO_WritePin(JDOUT4_GPIO_Port, JDOUT4_Pin,state);
    default:
        break;
    }
}
void led_init(){
    HAL_GPIO_WritePin(JDOUT1_GPIO_Port, JDOUT1_Pin,SET);
    HAL_GPIO_WritePin(JDOUT2_GPIO_Port, JDOUT2_Pin,SET);
    HAL_GPIO_WritePin(JDOUT3_GPIO_Port, JDOUT3_Pin,SET);
    HAL_GPIO_WritePin(JDOUT4_GPIO_Port, JDOUT4_Pin,SET);
}