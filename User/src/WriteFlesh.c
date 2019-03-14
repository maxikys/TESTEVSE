#include "WriteFlesh.h"


 /* Преобразование считанных данных из флеш для удобной обработки */
  void ConvertFlashData(const uint32_t Data, Rset* RSet)
  {
   static volatile uint8_t D1, D2, D3, D4;
     
   D4 = (uint8_t)( Data        );
   D3 = (uint8_t)( Data >>  8  );
   D2 = (uint8_t)( Data >>  16 );
   D1 = (uint8_t)( Data >>  24 );
   
   RSet->Voltage = D4 * 100 + D3;
   RSet->FanValue = D2 * 10  + D1;
  }
  /* Обратное преобразование данных для записи во флеш, тетрада */
  void ConvertDataBeforWriteFlash(const uint16_t ValueVolt, const uint8_t ValueFan, uint8_t* Mass)
  {
    Mass[0] = ValueVolt/100;
    Mass[1] = ValueVolt%100;
    Mass[2] = ValueFan/10;
    Mass[3] = ValueFan%10;
  }
  void DatatoFlash( uint8_t* Set1, uint8_t* Set2, uint8_t* Set3, uint8_t* WriteMass, uint8_t SizeWriteMass )
  {
   static uint8_t i;
     for( i = 0; i < SizeWriteMass; i++ )
     {
      if( i <= 3 )            {WriteMass[i] = Set1[i];}
      if( i > 3 && i <=  7 )  {WriteMass[i] = Set2[i - 4];}
      if( i > 7 && i <= 11 )  {WriteMass[i] = Set3[i - 8];}
     }
  
  }



void UnlockFlesh(void)
{
   FLASH->KEYR = ( uint32_t )REG1;
   FLASH->KEYR = ( uint32_t )REG2;
};

//pageAddress - любой адресс пренадлежащий стираемой области памяти
void Internal_Flash_Erase(unsigned int pageAddress) {
   while (FLASH->SR & FLASH_SR_BSY);
   if (FLASH->SR & FLASH_SR_EOP) {
      FLASH->SR = FLASH_SR_EOP;
   }

   FLASH->CR |= FLASH_CR_PER;
   FLASH->AR = pageAddress;
   FLASH->CR |= FLASH_CR_STRT;
   while (!(FLASH->SR & FLASH_SR_EOP));
   FLASH->SR = FLASH_SR_EOP;
   FLASH->CR &= ~FLASH_CR_PER;
}

//data - указатель на записываемые данные
//address - адрес во flash
//count - количество записываемых байт, должно быть кратно 2
void Internal_Flash_Write(unsigned char* data, unsigned int address, unsigned int count) {
	unsigned int i;

	while (FLASH->SR & FLASH_SR_BSY);
	if (FLASH->SR & FLASH_SR_EOP) {
		FLASH->SR = FLASH_SR_EOP;
	}

	FLASH->CR |= FLASH_CR_PG;

	for (i = 0; i < count; i += 2) {
		*(volatile unsigned short*)(address + i) = (((unsigned short)data[i + 1]) << 8) + data[i];
		while (!(FLASH->SR & FLASH_SR_EOP));
		FLASH->SR = FLASH_SR_EOP;
	}

	FLASH->CR &= ~(FLASH_CR_PG);
}
/* чтение значения по адресу */
uint32_t FlashRead(uint32_t address) 
{
return (*(__IO uint32_t*) address);
}

/* Залочивание Flash */
void flash_lock(void)
{
FLASH->CR |= FLASH_CR_LOCK;
}

 /* Запись значеия data, по адресу adress, количество байт - count */
void FlashWrite(unsigned char* data, unsigned int address, unsigned int count)
{
   UnlockFlesh();
   Internal_Flash_Erase(address);
   Internal_Flash_Write(data,address,count);
   flash_lock();
}

