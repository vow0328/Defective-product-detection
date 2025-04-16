#include "main.h"
void led()
{
    HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_12);
    HAL_Delay(200);
}