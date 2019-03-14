#ifndef WDT_H
#define WDT_H

#include <stdint.h>
#include <stdbool.h>
#include "stm32f0xx.h"
#include "gpio_driver.h"
#include <stdlib.h>
#include "systick.h"
void WDTStart(unsigned periodMSec);
void WDTDisable(void);
void WDTReset(void);

    
 
#endif
