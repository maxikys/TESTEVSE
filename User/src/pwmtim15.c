#include "pwmtim15.h"

const TPin Pb15 = {GPIOB,15};
const TPin* VPb15[] = {&Pb15};


void tim15Init(void)
{
   GPIO_InitTypeDef GPIO_Config;
   TIM_TimeBaseInitTypeDef TIM_BaseConfig;
   TIM_OCInitTypeDef TIM_OCConfig;
   
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM15, ENABLE); /* тактирование таймера */
   RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);   /* тактирование порта B */
 
   
   GPIO_Config.GPIO_Pin = GPIO_Pin_15;
   GPIO_Config.GPIO_Mode = GPIO_Mode_AF;
   GPIO_Config.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_Init(GPIOB, &GPIO_Config);
   
   /* Конфигурация таймера */
   
   
   TIM_BaseConfig.TIM_Prescaler = 79;
   TIM_BaseConfig.TIM_Period = 1000;
   TIM_BaseConfig.TIM_ClockDivision = 0;
   TIM_BaseConfig.TIM_CounterMode = TIM_CounterMode_Up;
   TIM_TimeBaseInit(TIM15, &TIM_BaseConfig);
   
   TIM_OCConfig.TIM_OCMode = TIM_OCMode_PWM1;
   TIM_OCConfig.TIM_OutputState = TIM_OutputState_Enable;
   TIM_OCConfig.TIM_Pulse = 150;
   TIM_OCConfig.TIM_OCPolarity = TIM_OCPolarity_High;
   
   TIM_OC2Init(TIM15, &TIM_OCConfig);
   
   TIM_OC1PreloadConfig(TIM15, TIM_OCPreload_Enable);
   TIM_OC2PreloadConfig(TIM15, TIM_OCPreload_Enable);
   TIM_ARRPreloadConfig(TIM15, ENABLE);
   
   TIM_Cmd(TIM15, ENABLE);
    

   
   

}
