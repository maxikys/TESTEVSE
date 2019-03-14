#pragma once


#include <stdint.h>
#include <stdbool.h>
#include "stm32f0xx.h"
#include "gpio_driver.h"
#include <stdlib.h>
#include "systick.h"
#include "tim.h"
#include "adc2.h"
#include "EXTI.h"
#include <stdlib.h>
#include "entertype.h"

#define SizeArrVolt 1
#define TimeOut 10000
#define SizeOfTwoStageArrayVoltage 50 


void MeasureProccesing(void);
