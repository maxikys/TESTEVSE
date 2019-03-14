#ifndef ADC_H
#define ADC_H

#include <stdint.h>
#include <stdbool.h>
#include "stm32f0xx.h"
#include "gpio_driver.h"
#include "semiseg.h"
#include "KeySingle.h"

extern TSemisegment Semiseg;

void ADC_Config(void);
uint16_t GetAdcValue(void);
uint16_t ADCConversion(void);
#endif
