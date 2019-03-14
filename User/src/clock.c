// ***********************************************************
//	clock.с
// 	Информация о тактировании
//
// ***********************************************************

#include "clock.h"
#include "stm32f0xx.h"

TStructClock StructClock;
__IO uint32_t CoreValue = 8000000;

const uint32_t massMultPLL[] = {
                                    RCC_CFGR_PLLMUL2,
                                    RCC_CFGR_PLLMUL3,
                                    RCC_CFGR_PLLMUL4,
                                    RCC_CFGR_PLLMUL5,
                                    RCC_CFGR_PLLMUL6,
                                    RCC_CFGR_PLLMUL7,
                                    RCC_CFGR_PLLMUL8,
                                    RCC_CFGR_PLLMUL9,
                                    RCC_CFGR_PLLMUL10,
                                    RCC_CFGR_PLLMUL11,
                                    RCC_CFGR_PLLMUL12,
                                    RCC_CFGR_PLLMUL13,
                                    RCC_CFGR_PLLMUL14,
                                    RCC_CFGR_PLLMUL15,
                                    RCC_CFGR_PLLMUL16
                               };

/* Сброс настроек частоты */
static void ClockReset(void)
{
   /* Включить HSE */
  RCC->CR |= (uint32_t)0x00000001;

#if defined(STM32F051)  
  /* Reset SW[1:0], HPRE[3:0], PPRE[2:0], ADCPRE and MCOSEL[2:0] bits */
  RCC->CFGR &= (uint32_t)0xF8FFB80C;
#else
  /* Reset SW[1:0], HPRE[3:0], PPRE[2:0], ADCPRE, MCOSEL[2:0], MCOPRE[2:0] and PLLNODIV bits */
  RCC->CFGR &= (uint32_t)0x08FFB80C;
#endif /* STM32F051 */
  
  /* Reset HSEON, CSSON and PLLON bits */
  RCC->CR &= (uint32_t)0xFEF6FFFF;

  /* Reset HSEBYP bit */
  RCC->CR &= (uint32_t)0xFFFBFFFF;

  /* Reset PLLSRC, PLLXTPRE and PLLMUL[3:0] bits */
  RCC->CFGR &= (uint32_t)0xFFC0FFFF;

  /* Reset PREDIV1[3:0] bits */
  RCC->CFGR2 &= (uint32_t)0xFFFFFFF0;

  /* Reset USARTSW[1:0], I2CSW, CECSW and ADCSW bits */
  RCC->CFGR3 &= (uint32_t)0xFFFFFEAC;

  /* Reset HSI14 bit */
  RCC->CR2 &= (uint32_t)0xFFFFFFFE;

  /* Disable all interrupts */
  RCC->CIR = 0x00000000;
}

/* Установка преддлелителя шины AHB */
 void AHBPrescaler(TAHBPrescaler AHBPrescaler)
{
    RCC->CFGR |= (uint32_t)AHBPrescaler;
}

/* Установка преддлелителя шины APB */
void APBPrescaler(TAPBPrescaler APBPrescaler)
{
    RCC->CFGR |= (uint32_t)APBPrescaler;
}
 /* Выбор источника тактирования для PLL */
static void SelectSourceForPLL(Tsource source)
{
    if(source == HSE)
    {
        RCC->CFGR |=   1 << 16;
        RCC->CFGR &= ~(1 << 15);
    }
    if(source == HSI)
    {
     RCC->CFGR |=   1 << 15;
     RCC->CFGR &= ~(1 << 16);
    }
}
  /* Предделитель перед PLL */
 static void SelectPredivForPLL(uint8_t prediv)
 {
    RCC->CFGR2 = (prediv & 0x1F) - 1;
 }

 /* Выбор источника для настройки частоты */
 static void SelectBaseSystemSource(Tsource source)
 {
 
    if(source == HSE)
    {
     RCC->CFGR &= ~ (1 << 1);
     RCC->CFGR |=    1 << 0;
     while ((RCC->CFGR & 0x0C) != (uint32_t)0x04){};
    }
    
    if(source == HSI)
    {
    RCC->CFGR &= ~( (1 << 0)|(1 << 1) );
    while ((RCC->CFGR & 0x0C) != (uint32_t)0x00){};
    }
    
    if (source == PLL)
    {
     RCC->CFGR &= ~ (1 << 0);
     RCC->CFGR |=    1 << 1;
     while ((RCC->CFGR & 0x0C) != (uint32_t)0x08){};
    }
 }
 
 /* Выбор коэфф. умножения */
 static void SelectMultPLL( uint8_t Mult)
 {
     if(Mult > 1 && Mult <= 16 ) RCC->CFGR |= massMultPLL[(Mult) - 2];
 }
 
 /* Настройка HSI, приватная функция */
 static void InitHSI(void)
 {
     AHBPrescaler(AHBDIV1);/* предделитель шыны AHB = 1, можно менять (1,2,4...512) */
     APBPrescaler(APBDIV1);/* предделитель шыны APB = 1  можно менять (1,2,4,8,16)  */
        
      /*  Сбросим настройки касаемые PLL, это:
          - Выберем источник для PLL - HSI/2;
          - Сбросим предделитель перед PLL;
          - Сбросим умножитель PLL.
      */
        RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMULL));
        SelectPredivForPLL(1);/* Устанавливаем предделитель перед PLL (1...16), можно изменить */
        if( StructClock.Mult > 1 )  
      {
          SelectSourceForPLL(HSI);/* Выберем источник для PLL */
          SelectMultPLL (StructClock.Mult);/* Устанавливаем коэф. умножения */
          RCC->CR |= RCC_CR_PLLON; /* Включим PLL */
          while((RCC->CR & RCC_CR_PLLRDY) == 0){};/* Подождем запуска  PLL */
          SelectBaseSystemSource(PLL);
      }
      else 
      {
          /* Нужно выбрать в качестве источника сигнала HSE */
          SelectBaseSystemSource(HSI);
      }
 }
 
 /* Настройка HSE, приватная функция */
 static void InitHSE(void)
 {
     volatile uint32_t counter = 0;
        RCC->CR |= (uint32_t) RCC_CR_HSEON; /* Включаем HSE */
       /*проверка установления флага готовности HSE*/
       while( ( !(RCC->CR & RCC_CR_HSERDY ) ) && (counter != HSE_STARTUP_TIMEOUT))
        {
        counter++; 
        };
  /* Если готов HSE */
  if( (RCC->CR & RCC_CR_HSERDY)  != NoReady )
  {
        AHBPrescaler(AHBDIV1);/* предделитель шыны AHB = 1, можно менять (1,2,4...512) */
        APBPrescaler(APBDIV1);/* предделитель шыны APB = 1  можно менять (1,2,4,8,16)  */
      /*  Сбросим настройки касаемые PLL, это:
          - Выберем источник для PLL - HSI/2;
          - Сбросим предделитель перед PLL;
          - Сбросим умножитель PLL.
      */
        RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMULL)); 
        
        SelectPredivForPLL(1);/* Устанавливаем предделитель перед PLL (1...16), можно изменить */
      
      if( StructClock.Mult > 1 )  
      {
          SelectSourceForPLL(HSE);/* Выберем источник для PLL */
          SelectMultPLL (StructClock.Mult);/* Устанавливаем коэф. умножения */
          RCC->CR |= RCC_CR_PLLON; /* Включим PLL */
          while((RCC->CR & RCC_CR_PLLRDY) == 0){};/* Подождем запуска  PLL */
          SelectBaseSystemSource(PLL);
      }
      else 
      {
          /* Нужно выбрать в качестве источника сигнала HSE */
          SelectBaseSystemSource(HSE);
      }
      
  }
   else
    {
        /* вернуться к запуску HSI */
        InitHSI();
    }
    
 }    
 
 
 /*Функция инициализации структуры хранения конфигурации настройки тактовой частоты */
 static void InitClockStruct(uint32_t* ClockValue, uint32_t* CoreClock, uint8_t* Mult)
 {
  StructClock.ValueClock =  *ClockValue;
  StructClock.ValueCore  =  *CoreClock;
  StructClock.Mult       =  *Mult;
 }
 
// Инициализация
void ClockInit(Tsource source, uint32_t ClockValue, uint32_t CoreClock )
{   
    
    uint8_t temp = 0;
    temp = CoreClock/ClockValue;/* Коэф. умножения */
    InitClockStruct(&ClockValue,&CoreClock,&temp);
    ClockReset();
    /* Enable Prefetch Buffer and set Flash Latency */
    FLASH->ACR = FLASH_ACR_PRFTBE | FLASH_ACR_LATENCY;
    if(source == HSE)
    {
      InitHSE();
    }
    if(source == HSI) /* Если в качестве тактовой выбран HSI */
    {
     InitHSI();
    }
   CoreValue = (uint32_t) CoreClock;  /*раскоментировать для корректной работы*/
}

// Частота работы ядра
uint32_t clock_GetSYS(void)
{
    return CoreValue;
}

// Частота шины APB
uint32_t clock_GetAPB(void)
{
    // RCC->CFGR 10:8
    int Prescaler = 1;
    uint8_t PrescalerSelect = ((RCC->CFGR >> 8) & 0x07);
    
    if(PrescalerSelect & 0x04) Prescaler = (2 << (PrescalerSelect & 0x03));
    
    return (CoreValue / Prescaler);
}


