#ifndef _LED_H
#define _LED_H

void led_control(uint8_t num,GPIO_PinState  state);
void led_init(void);
#endif // !_LED_H