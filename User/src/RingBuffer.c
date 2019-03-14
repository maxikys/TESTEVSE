#include "RingBuffer.h"



 uint8_t IndexIN = 0, IndexOUT = 0;
 uint8_t RingBuff[SizeRingBuff];

/* функция проверки состояния  наполнености кольцевого буфера возращает true если в буфере нету данных, false - буфере есть данные*/
  bool RingBuffEmpty(void)
  {
   if(IndexIN != IndexOUT) return false;/*в буфере есть данные*/
   else return true;       
  }
 /*запись данных в кольцевой буфер*/ 
 void WriteToRingBuff( uint8_t Value)
  {
   RingBuff[IndexIN++] = Value; /*помещаем в кольцевой буфер сканкоды согласно нажатой конопки*/
   IndexIN &= MaskRingBuff; /* если индекс записи вышел за пределы кольцевого буфера, то помещаем его в начало*/
  };
 /*Возвращяет размер присутствующих данных в буфере*/
  uint8_t SizeWriteDataToRingBuff(void)
  {
    return ( IndexIN - IndexOUT) & MaskRingBuff;
  }
 /* чтение из кольцевого буфера*/
 uint8_t ReadFromRingBuff( void )
  {
       volatile uint8_t Value;
       Value = RingBuff[IndexOUT++];
       IndexOUT &= MaskRingBuff;
       return Value;
  }
