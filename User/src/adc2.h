#pragma once


#include <stdint.h>
#include <stdbool.h>
#include "stm32f0xx.h"
#include "gpio_driver.h"
#include <stdlib.h>
#include "systick.h"

#define SizeArray 20
#define CountofChannel 1



typedef enum{ Off, On}t_State;

typedef enum 
{
  AN0, AN1, AN2, AN3, AN4, AN5, AN6, AN7, AN8, AN9, AN10, AN11, AN12, AN13, AN14, AN15, AN16, AN17, AN18
}t_ADC_Channel;


bool ADC_Stop(void);
bool ADC_InitADC(void);
void ADC_SelectChannel(t_ADC_Channel AN, t_State state);
bool ADC_Start(void);
