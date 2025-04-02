#include "tim.h"
#include "gpio.h"
#include "math.h"
#include "H_Tmc2209.h"

uint8_t current_level=0,target_level=0;

//档位0：40hz 24999 档位39：1600hz 624
int speed_levels[40]={24999,12499,8332,6249,4999,4165,3570,3124,2776,2499,2271,2082,1922,1784,1665,1561,1469,
	1387,1314,1249,1189,1135,1085,1040,999,960,924,891,861,832,805,780,756,734,713,693,674,656,640,624};

void Motor2_Speed(uint8_t level){
	if(level>MAX_SPEED_LEVEL) level=MAX_SPEED_LEVEL;	//限幅
	target_level = level;															//更新目标等级
}

//DISABLE OR ENABLE
//选择是否使能
void Motor2_Enable(FunctionalState state) {
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, (state == ENABLE) ? GPIO_PIN_RESET : GPIO_PIN_SET);
}

//GPIO_PIN_RESET OR GPIO_PIN_SET
//选择方向vcc为正
void Motor2_Direction(GPIO_PinState dir) {
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, dir);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim == &htim5) {
        // 逐步逼近目标档位
        if (current_level < target_level) current_level++;
        else if (current_level > target_level) current_level--;
        
        // 更新TIM2参数
        uint32_t new_arr = speed_levels[current_level];
        __HAL_TIM_SET_AUTORELOAD(&htim2, new_arr);      // 设置ARR
        __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, new_arr / 2); // 50%占空比
    }
}
