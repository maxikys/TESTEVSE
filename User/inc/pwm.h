#ifndef PWM_H
#define PWM_H

#include <stdint.h>
#include <stdbool.h>
#include "stm32f0xx.h"
#include "gpio_driver.h"
#include <stdlib.h>

static uint16_t CalculationStartValue(void);

// Список каналов
    typedef enum
    {
        TIM1_CH1,  // PA8
        TIM1_CH2,  // PA9
        TIM1_CH3,  // PA10
        TIM1_CH4,  // PA11

        TIM2_CH1,  // PA0
        TIM2_CH2,  // PA1
        TIM2_CH3,  // PA2
        TIM2_CH4,  // PA3

        TIM3_CH1,  // PA6
        TIM3_CH2,  // PA7
        TIM3_CH3,  // PB0
        TIM3_CH4,  // PB1

        TIM14_CH1,      // PA7
        TIM14_CH1_B1,   // PB1
        
        TIM17_CH1,      //PA7
        TIM17_CH1_B9,   //PB9
        
    } TPwmChannel;

    
    typedef enum
    {
      AktiveHigh,
      AktiveLow
    }AktiveState;
    
    // Инициализация PWM
    void pwm_Init(int Channel);

    // Задать скважность
    void pwm_Set(int Channel, uint16_t Value);

    void PwmSetPersent(int Channel, uint16_t Value);


#endif

