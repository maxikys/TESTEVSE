#pragma once


#include <stdint.h>
#include <stdbool.h>
#include "stm32f0xx.h"
#include "gpio_driver.h"
#include <stdlib.h>
#include "systick.h"
#include "clock.h"
#include "adc2.h"
#include "entertype.h"

#define ValueCountTim15        (1000000)
#define TimeBeforMeasurePhaseA (4800)
#define TimeBeforMeasurePhaseB (6666)
#define TimeBeforMeasurePhaseC (6666)


typedef struct
{
    TIM_TypeDef *       TIM;                 // Таймер
    uint32_t            RCCRegMask;          // Маска для управления тактированием
    volatile uint32_t * RCCReg;              // Адрес регистра управления тактированием
    IRQn_Type IRQ;
    uint8_t LevelOfPririty;
} t_Timer;


void SetCountValue(t_Timer* timer, uint16_t value, uint32_t freq);
void timerInit(t_Timer* Timer, t_State state);
