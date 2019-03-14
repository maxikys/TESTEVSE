#ifndef WRITEFLESH_H
#define WRITEFLESH_H

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
 
#define REG1 (uint32_t) 0x45670123;
#define REG2 (uint32_t) 0xCDEF89AB;

typedef struct
{
   uint16_t Voltage;
   uint8_t FanValue;
   uint8_t Index;
   bool Flag;
}Rset;


void FlashWrite(unsigned char* data, unsigned int address, unsigned int count);
uint32_t FlashRead(uint32_t address);
void ConvertFlashData(const uint32_t Data, Rset* RSet);
void ConvertDataBeforWriteFlash(const uint16_t ValueVolt, const uint8_t ValueFan, uint8_t* Mass);
void DatatoFlash( uint8_t* Set1, uint8_t* Set2, uint8_t* Set3, uint8_t* WriteMass, uint8_t SizeWriteMass );
#endif
