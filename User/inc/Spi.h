#ifndef SPI_H
#define SPI_H

#include <stdint.h>
#include <stdbool.h>
#include "stm32f0xx.h"
#include "gpio_driver.h"
#include <stdlib.h>
#include "keyboard.h"
#include "define.h"

typedef enum
{
   LedOff,
   LedOn

}TLedState;

typedef struct
 {
    uint8_t Mask;
    uint8_t NumDDMask;
    void (* Handler)(void);/*Указатель на функцию, которая будет вызываться  по нажатию на кнопку */
    
 }TMaskLed;

void SPI1_ini(void);
void Write_SPI(void);
void CheckPressButton(void);
void DirectLed( const TMaskLed* Mass[], uint8_t NumberKey, TLedState State );
//void DirectLed(uint8_t NumberKey, TLedState State );
//void SPI1_DMA1_ini(void);
//void DMA1_Channel2_3_IRQHandler(void);


#endif
