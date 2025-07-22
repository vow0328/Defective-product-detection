#ifndef _OUTPUT_H
#define _OUTPUT_H

// LED GPIO 端口和引脚定义数组（下标从 1 开始，0 占位）
extern GPIO_TypeDef *output_ports[];
extern uint16_t output_pins[];

void OUTPUT_control(uint8_t num, GPIO_PinState state);
void OUTPUT_init(void);
#endif // !_OUTPUT_H