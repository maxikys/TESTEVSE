#ifndef PLATFORM_DELAY_H
#define PLATFORM_DELAY_H



#include "stdint.h"
#include "clock.h"
 enum delay {PlatformCyslesPerDelayLoop = 6};
 __inline void DelayLoop(uint32_t delayLoops);
 void delay_us(unsigned long us);
 void delay_ms(unsigned long ms);
#endif
