
#ifndef _USART_H
#define _USART_H


#include <stdint.h>
#include <stdbool.h>
#include "stm32f0xx.h"
#include "gpio_driver.h"
#include <stdlib.h>

#include "stm32f0xx_rcc.h"
#include "stm32f0xx_gpio.h"
#include "stm32f0xx_usart.h"
#include "stm32f0xx_misc.h"
void UsartInit(void);
void put_char(uint8_t c);
uint8_t get_char(void);

#endif
