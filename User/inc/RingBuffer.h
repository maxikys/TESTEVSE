#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <stdint.h>
#include <stdbool.h>
#include "stm32f0xx.h"
#include "gpio_driver.h"
#include <stdlib.h>
 
 #define SizeRingBuff 8
 #define MaskRingBuff SizeRingBuff -1
 
 


/* секци€ объ€влени€ протатипов*/

/*возращает состо€ние буфера true - полон, false - не полон*/
bool RingBuffEmpty(void);

 /* помещаем данные в буфер*/
void WriteToRingBuff(uint8_t);

/* чтение из кольцевого буфера */
 uint8_t ReadFromRingBuff( void );
  
  
#endif
