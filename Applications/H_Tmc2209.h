#ifndef __H_TMC2209_H__
#define __H_TMC2209_H__

#define motor2 GPIOB
#define motor3 GPIOC
#define EN2 GPIO_PIN_13
#define EN3 GPIO_PIN_2
#define DIR2 GPIO_PIN_15
#define DIR3 GPIO_PIN_5
#define STEP2 TIM2CH2
#define STEP3 TIM8CH1

#define MAX_SPEED_LEVEL 39

	void Motor2_Speed(uint8_t level);
	void Motor2_Direction(GPIO_PinState dir);
	void Motor2_Enable(FunctionalState state);

#endif
