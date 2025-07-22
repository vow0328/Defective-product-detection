#include "include.h"

#define MOTOR_COUNT 6
#define INPUT_SENSOR_COUNT 8
#define OUTPUT_SENSOR_COUNT 8

// GPIO 端口和引脚定义数组（下标从 1 开始，0 占位）
GPIO_TypeDef *en_ports[MOTOR_COUNT + 1] = {
    NULL,
    EN1OUT_GPIO_Port,
    EN2OUT_GPIO_Port,
    EN3OUT_GPIO_Port,
    EN4OUT_GPIO_Port,
    EN5OUT_GPIO_Port,
    EN6OUT_GPIO_Port,
};

uint16_t en_pins[MOTOR_COUNT + 1] = {
    0,
    EN1OUT_Pin,
    EN2OUT_Pin,
    EN3OUT_Pin,
    EN4OUT_Pin,
    EN5OUT_Pin,
    EN6OUT_Pin,
};

GPIO_TypeDef *dir_port[MOTOR_COUNT + 1] = {
    NULL,
    DIR1OUT_GPIO_Port,
    DIR2OUT_GPIO_Port,
    DIR3OUT_GPIO_Port,
    DIR4OUT_GPIO_Port,
    DIR5OUT_GPIO_Port,
    DIR6OUT_GPIO_Port,
};

uint16_t dir_pins[MOTOR_COUNT + 1] = {
    0,
    DIR1OUT_Pin,
    DIR2OUT_Pin,
    DIR3OUT_Pin,
    DIR4OUT_Pin,
    DIR5OUT_Pin,
    DIR6OUT_Pin,
};

// 定时器和通道定义数组（根据你当前使用的定时器顺序）
TIM_HandleTypeDef *motor_tim[MOTOR_COUNT + 1] = {
    NULL,
    &htim1,
    &htim8,
    &htim2,
    &htim3,
    &htim5,
    &htim4,
};

uint32_t motor_channel[MOTOR_COUNT + 1] = {
    0,
    TIM_CHANNEL_1,
    TIM_CHANNEL_2,
    TIM_CHANNEL_3,
    TIM_CHANNEL_3,
    TIM_CHANNEL_4,
    TIM_CHANNEL_2,
};
// INPUT 引脚端口定义数组（下标从 1 开始，0 占位）

GPIO_TypeDef *input_ports[INPUT_SENSOR_COUNT + 1] = {
    NULL, // 索引0不使用
    INPUT1_GPIO_Port,
    INPUT2_GPIO_Port,
    INPUT3_GPIO_Port,
    INPUT4_GPIO_Port,
    INPUT5_GPIO_Port,
    INPUT6_GPIO_Port,
    INPUT7_GPIO_Port,
    INPUT8_GPIO_Port,
};

uint16_t input_pins[INPUT_SENSOR_COUNT + 1] = {
    0, // 索引0不使用
    INPUT1_Pin,
    INPUT2_Pin,
    INPUT3_Pin,
    INPUT4_Pin,
    INPUT5_Pin,
    INPUT6_Pin,
    INPUT7_Pin,
    INPUT8_Pin,
};

// OUTPUT 端口定义数组（下标从 1 开始，0 占位）
GPIO_TypeDef *output_ports[OUTPUT_SENSOR_COUNT + 1] = {
    NULL, // 索引0不使用
    OUTPUT1_GPIO_Port,
    OUTPUT2_GPIO_Port,
    OUTPUT3_GPIO_Port,
    OUTPUT4_GPIO_Port,
    OUTPUT5_GPIO_Port,
    OUTPUT6_GPIO_Port,
    OUTPUT7_GPIO_Port,
    OUTPUT8_GPIO_Port,
};

uint16_t output_pins[OUTPUT_SENSOR_COUNT + 1] = {
    0, // 索引0不使用
    OUTPUT1_Pin,
    OUTPUT2_Pin,
    OUTPUT3_Pin,
    OUTPUT4_Pin,
    OUTPUT5_Pin,
    OUTPUT6_Pin,
    OUTPUT7_Pin,
    OUTPUT8_Pin,
};