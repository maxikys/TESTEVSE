#ifndef MAIN_H
#define MAIN_H

#include <stdbool.h>
#include "stm32f0xx.h"
#include "gpio_driver.h"
#include "clock.h"
#include "systick.h"
#include "keyboard.h"
#include "Spi.h"
#include "Pwm.h"
#include "platform_delay.h"
#include "hd44780.h"
#include "WDT.h"
#include "ADC.h"
#include "can.h"
#include "i2c.h"
#include "semiseg.h"
#include "KeySingle.h"
#include "pwmtim15.h"
#include "WriteFlesh.h"
#include "usart.h"
#include "crc_calc.h"
#include "tim.h"
#include "adc2.h"
#include "EXTI.h"
#include <stdlib.h>
#include "entertype.h"
#include "routine.h"



void SortValue(Rset* Mass[]);

#endif
