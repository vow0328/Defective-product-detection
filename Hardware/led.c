#include "include.h"
void led_control(uint8_t num, GPIO_PinState state)
{
    switch (num)
    {
    case 1:
        HAL_GPIO_WritePin(JDOUT1_GPIO_Port, JDOUT1_Pin, state);
        break;
    case 2:
        HAL_GPIO_WritePin(JDOUT2_GPIO_Port, JDOUT2_Pin, state);
        break;
    default:
        break;
    }
}
void led_init()
{
    HAL_GPIO_WritePin(JDOUT1_GPIO_Port, JDOUT1_Pin, RESET);
    HAL_GPIO_WritePin(JDOUT2_GPIO_Port, JDOUT2_Pin, RESET);

}