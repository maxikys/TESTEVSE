#pragma once


#include <stdint.h>
#include <stdbool.h>
#include "stm32f0xx.h"
#include "gpio_driver.h"
#include <stdlib.h>
#include "systick.h"
#include "adc2.h"
#include "tim.h"
#include "entertype.h"

typedef enum
{
  RiseEdge,
  FadeEdge,
  BothEdge
} t_ModeTriger;

typedef struct
{
  const TPin* Pin;
  IRQn_Type IRQ;
  t_ModeTriger ModeTriger;
  uint8_t LevelOfPririty;
}t_EXTI;


void EXTI_set( IRQn_Type IRQ, t_State state );
void EXTI_Init(t_EXTI EXTI_Pin );
void SortM(uint16_t* num, uint16_t size);
