#include "include.h"

#define OUTPUT_SENSOR_COUNT 8

void OUTPUT_control(uint8_t num, GPIO_PinState state)
{

    HAL_GPIO_WritePin(output_ports[num], output_pins[num], state);
}
void OUTPUT_init()
{
    for (size_t i = 1; i <= OUTPUT_SENSOR_COUNT; i++)
    {
        HAL_GPIO_WritePin(output_ports[i], output_pins[i], RESET);
    }
}