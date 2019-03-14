#ifndef CAPTUREPWM_H
#define CAPTUREPWM_H

//#include "LED_ini.h"
//#include "USART_ini.h"
//#include "NVIC_ini.h"
#include <stdbool.h>
#include "stm32f0xx.h"
#include "gpio_driver.h"
#include "keyboard.h"
#include "Shim_ini.h"
#include "clock.h"
#include "systick.h"
#include "define.h"
#include "Spi.h"


void TIM_Config(void);
void CorrectBrightness(void);
float GammaCorrect(uint16_t value2, uint16_t value1);
void set_pwm_width_norm(int channel, int pwm_period, float duty_cycle);
#endif
