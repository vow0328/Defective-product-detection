#include "include.h"
void led_control(uint8_t num, GPIO_PinState state)
{
    switch (num)
    {
    case 1:
        HAL_GPIO_WritePin(MOUT1_GPIO_Port, MOUT1_Pin, state);
        break;
    case 2:
        HAL_GPIO_WritePin(MOUT2_GPIO_Port, MOUT2_Pin, state);
        break;
    case 3:
        HAL_GPIO_WritePin(MOUT3_GPIO_Port, MOUT3_Pin, state);
        break;
    case 4:
        HAL_GPIO_WritePin(MOUT4_GPIO_Port, MOUT4_Pin, state);
        break;
    case 5:
        HAL_GPIO_WritePin(MOUT5_GPIO_Port, MOUT5_Pin, state);
        break;
    case 6:
        HAL_GPIO_WritePin(MOUT6_GPIO_Port, MOUT6_Pin, state);
        break;
    case 7:
        HAL_GPIO_WritePin(MOUT7_GPIO_Port, MOUT7_Pin, state);
        break;
    case 8:
        HAL_GPIO_WritePin(MOUT8_GPIO_Port, MOUT8_Pin, state);
        break;
    default:
        break;
    }
}
void led_init()
{
    HAL_GPIO_WritePin(MOUT1_GPIO_Port, MOUT1_Pin, RESET);
    HAL_GPIO_WritePin(MOUT2_GPIO_Port, MOUT2_Pin, RESET);
    HAL_GPIO_WritePin(MOUT3_GPIO_Port, MOUT3_Pin, RESET);
    HAL_GPIO_WritePin(MOUT4_GPIO_Port, MOUT4_Pin, RESET);
    HAL_GPIO_WritePin(MOUT5_GPIO_Port, MOUT5_Pin, RESET);
    HAL_GPIO_WritePin(MOUT6_GPIO_Port, MOUT6_Pin, RESET);
    HAL_GPIO_WritePin(MOUT7_GPIO_Port, MOUT7_Pin, RESET);
    HAL_GPIO_WritePin(MOUT8_GPIO_Port, MOUT8_Pin, RESET);
}