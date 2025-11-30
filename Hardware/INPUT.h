#ifndef __INPUT_H
#define __INPUT_H


extern GPIO_TypeDef *input_ports[];
extern uint16_t input_pins[];

uint8_t Read_INPUT(uint8_t num);
uint8_t Send_INPUT(void);
#endif
