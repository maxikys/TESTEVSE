#ifndef KEYSINGLE_H
#define KEYSINGLE_H

#include <stdint.h>
#include <stdbool.h>
#include "stm32f0xx.h"
#include "gpio_driver.h"
#include <stdlib.h>
#include "RingBuffer.h"
#include "define.h"
//#include  "usbd_custom_hid_core.h"
//#include  "usbd_usr.h"
#include "systick.h"
#include "keyboard.h"
#include "semiseg.h"     
#include "pwm.h"
#include "ADC.h"
#include "WriteFlesh.h"
#include "main.h"
 
 typedef struct 
     {    
          uint8_t *cntKeyPress; /* указатель на массив в котором хранятся счетчики сканирования при обнаружении флага нажатия кнопки*/
          bool *FlagDetectPress; /* указатель на массив, хранящий флаги нажатия клавиатуры*/
          bool *FlagDetectDePress;
          bool Flag1;
          uint8_t CountInOneMoment;
          
      }TKeySingleStatus;
 
void KeySingleInit(Tperiod TimeScan, Tperiod TimeDrebezg, uint8_t CountKey,TActiveState ActiveState);      
void ScanSingleKey(void);
void DecodeSingleKey( void);
void SetBuzzer(uint8_t value);
void ConvVoltage(uint16_t Value);      
#endif
