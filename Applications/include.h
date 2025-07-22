#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_tim.h"
#include "main.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "H_Tmc2209.h"
#include "serial.h"
#include "Scheduler.h"
#include "control.h"
#include "input.h"
#include "output.h"


extern TIM_HandleTypeDef *motor_htim[];
extern uint32_t motor_channel[];
extern GPIO_TypeDef *motor_en_port[];
extern uint16_t motor_en_pin[];
extern GPIO_TypeDef *input_ports[];
extern uint16_t input_pins[];
extern GPIO_TypeDef *output_ports[];
extern uint16_t output_pins[];