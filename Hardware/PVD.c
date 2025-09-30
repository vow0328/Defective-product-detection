#include "pvd.h"

PWR_PVDTypeDef sConfigPVD;

/**
 * @brief  Configures the PVD resources.
 * @param  None
 * @retval None
 */
void PVD_Config(void)
{
    /*##-1- Enable Power Clock #################################################*/
    __HAL_RCC_PWR_CLK_ENABLE();

    /*##-2- Configure the NVIC for PVD #########################################*/
    HAL_NVIC_SetPriority(PVD_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(PVD_IRQn);

    sConfigPVD.PVDLevel = PWR_PVDLEVEL_0;
    sConfigPVD.Mode = PWR_PVD_MODE_IT_RISING_FALLING;
    HAL_PWR_ConfigPVD(&sConfigPVD);

    /* Enable the PVD Output */
    HAL_PWR_EnablePVD();
}

/** PVD (Programmable Votage Detector) ,即可编程电压监测器，PVD中断回调，在这个函数中添加自己需要的断电时处理的内容
 * @brief  PWR PVD interrupt callback
 * @retval None
 */
void HAL_PWR_PVDCallback(void)
{
    /* NOTE : This function Should not be modified, when the callback is needed,
              the HAL_PWR_PVDCallback could be implemented in the user file
     */
    // NVIC_SystemReset();
    HAL_PWR_EnterSTANDBYMode(); //进入待机模式
}
