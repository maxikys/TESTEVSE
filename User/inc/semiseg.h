#ifndef SEMISEG
#define SEMISEG

#include "stm32f0xx.h"
#include "stm32f0xx_gpio.h"
#include "stm32f0xx_can.h"
#include "gpio_driver.h"
#include "clock.h"
#include <stdlib.h>
#include <string.h>
#include "platform_delay.h"

typedef enum 
{
   CommonCatode,/* общий катод */
   CommonAnode /* общий анод */
   
}TypeSemisegment;

 /* Структура хранящая массив информации и флаг необходимости вывода точки */
 typedef struct
{
   uint8_t* Data;
   bool* EnableDrop;
}TData;
/* Структура инициализации семисегментника */
typedef struct
{
   TypeSemisegment Type;
   uint8_t SizeDigit;
   uint8_t SizeSeg;
   TData Data;
   TData Temp;
   char * TempData;
   uint8_t Brightness;
   bool EndString;

}TSemisegment;



 void InitSemiseg(TypeSemisegment Type, uint8_t SizeDigit, uint8_t SizeSeg);
 void WriteDatatoSemisegment(void);
 void ShiftString(void);
 static void ShiftSTR(char* str);
 void PrintShiftString(void);
 void ShiftStr(char* str);
 void WriteString(char* str);
#endif
