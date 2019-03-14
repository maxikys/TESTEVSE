
#ifndef USART_INI_H
#define USART_INI_H

#include "stm32f0xx.h"
#include "stm32f0xx_rcc.h"
#include "stm32f0xx_gpio.h"
#include "stm32f0xx_usart.h"
void USART_ini(void);
void USART1_IRQHandler(void);
void put_char(uint8_t c);
void put_string(unsigned char *s);
void put_int(int32_t data);
uint8_t get_char(void);
#endif
