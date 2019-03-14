#include "platform_delay.h"


__inline void DelayLoop(uint32_t delayLoops)
{
  do
  {
    // TODO: rewrite in assembly
    // __no_operation();
  }
  while( delayLoops-- );
}




void delay_us(unsigned long us)
{
 const unsigned long delayLoops = (unsigned long)(clock_GetSYS() / (1.0e6 * PlatformCyslesPerDelayLoop) * us);
  
  DelayLoop(delayLoops);
}

void delay_ms(unsigned long ms)
{
  delay_us(1000 * ms);
}


