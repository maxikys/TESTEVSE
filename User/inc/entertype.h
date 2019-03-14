#pragma once


#include <stdint.h>
#include <stdbool.h>
#include "stm32f0xx.h"
#include "gpio_driver.h"
#include <stdlib.h>

typedef enum 
{
  PilotMeasureStart,
  PilotMeasureDone,
  Phase_A_MeasureStart,
  Phase_A_MeasureDone,
  Phase_B_MeasureStart,
  Phase_B_MeasureDone,
  Phase_C_MeasureStart,
  Phase_C_MeasureDone
} t_routine;

typedef struct
{
  t_routine status;
  
}t_Status;
