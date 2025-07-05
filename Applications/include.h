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
#include "Photo_sensitive.h"
#include "led.h"


#define MOTOR_NUM 7  // 电机编号从 1~6，索引0不使用

extern TIM_HandleTypeDef* motor_htim[MOTOR_NUM];
extern uint32_t motor_channel[MOTOR_NUM];
extern GPIO_TypeDef* motor_en_port[MOTOR_NUM];
extern uint16_t motor_en_pin[MOTOR_NUM];
