#pragma once


#include <stdint.h>
#include <stdbool.h>
#include "stm32f0xx.h"
#include "gpio_driver.h"
#include <stdlib.h>
#include "systick.h"

// 6 channels (including inttemp & vrefint)
#define NUMBER_OF_ADC_CHANNELS (4)

extern volatile uint32_t Tms; // time counter for 1-second Vdd measurement

typedef enum{
    ESW_RELEASED,
    ESW_HALL,
    ESW_BUTTON,
    ESW_ERROR
} ESW_status;

void        adc_setup(void);
uint16_t    getADCval(int nch);
int32_t     getTemp(void);
uint32_t    getVdd(void);
uint32_t    getVmot(void);
uint32_t    getImot(void);
ESW_status  eswStatus(int motnum, int eswnum);

