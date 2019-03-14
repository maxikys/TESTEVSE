#include "EXTI.h"
#include <math.h>


/* Extern difinition */
  
extern t_Status CurentState;
extern t_Timer Tim15;
extern const TPin C8;
extern const TPin C6;
  
/* Difinition of varables */
uint16_t ADC_Result[CountofChannel] = {0};
uint16_t Index = 0;
uint16_t MassV0[SizeArray]= {0};
uint16_t SortMass[SizeArray];
bool flagEnableSort = false;
bool flagEndSort = true;
uint16_t HightValueCh1 = 0;
uint16_t LowValueCh1 = 0;
uint16_t MidlValueCh1 = 0;
/*Varable of Smart type */


/* Define for EXTI */

/* тригер для захвата пилот тона */
const TPin Trig_PT  = {GPIOC, 0};
t_EXTI EXTI_Trig_PT ={&Trig_PT, EXTI0_1_IRQn, BothEdge,5};

const TPin Trig_Valtage = {GPIOC, 3};
t_EXTI EXTI_Trig_Valtage ={&Trig_Valtage, EXTI2_3_IRQn, FadeEdge,0};



/* прерывание для начала старта измерения напряжения по , триггер подключен к фазе А */
void EXTI2_3_IRQHandler(void)
{
  if( EXTI->PR & 1 << (EXTI_Trig_Valtage.Pin->Pin ) )
  {
    ADC_SelectChannel(AN1,On);
    ADC_SelectChannel(AN0,Off);
    /*----------------------------------*/
    if( (TIM15->SR & TIM_SR_UIF) != 0 )
    {
      TIM15->SR &= ~ TIM_SR_UIF;
    }
    NVIC_EnableIRQ(TIM15_IRQn);                                                       // Запрещаем прерывание
    TIM15->CR1  |= TIM_CR1_CEN;                                                       // включаем таймер
    TIM15->DIER |= TIM_DIER_UIE;                                                      // разрешаем прерывание от таймера
    /*----------------------------------*/
    TIM15->ARR = TimeBeforMeasurePhaseA - 1;
    TIM15->CNT = 0;
    EXTI->PR |= 1 << (EXTI_Trig_Valtage.Pin->Pin);                                    // Сбросим флаг прерывания
    TogglePin(&C6);
    CurentState.status = PilotMeasureDone;                                            //!!!!!!!!!!!!!!!!!!!!
  }

}


// Внешнее прерывание пилот тона, в функции изменять структуру с инициализацией
/*----------------------------------------------------------------------------------*/
void EXTI0_1_IRQHandler(void)                               
{
   uint16_t k = 0;
   static uint32_t Temp = 0;
   static uint32_t t;
   static uint8_t i = 0;

  if( (EXTI->PR & 0x0001) != 0 )                                                    // если сработало прерывание на ножку "0"
  {
    ADC_SelectChannel(AN0,On);
    ADC_SelectChannel(AN1,Off);
    //EXTI->PR |= 0x01;                                                             // Сбросим флаг прерывания
     EXTI->PR |= 0x0001;
    for(t = 0; t < 25; t++){}                                                       // Задержка для устранения затягивания фронта
    ADC1->CR |= ADC_CR_ADSTART;                                                     // Включаем преобразование
    Temp = ADC1->DR;
    Temp = 0;
    for (i = 0; i < CountofChannel; i++)                                            // В цикле опрашиваем каналы
    {
      while ((ADC1->ISR & ADC_ISR_EOC) == 0 && Temp <= 10000)                       // Ожидаем флаг окончания преобразования
      {
      Temp++;                                                                       // Тайм аут
      }
      ADC_Result[i] = ADC1->DR;                                                     // Сохраняем значения в буфере
    }
    
    MassV0[Index] = ADC_Result[0];                                                  // пересчитываем значения напряжения с канала 1

    if(Index < SizeArray - 1)                                                       // проверка за выскакивание из диапазона
    {
      Index++;
    }
    if(Index >= SizeArray - 1)                                                      // Если значение равно размеру массива, то сортируем массив  и выбираем медиантное значение
    {
      Index = 0;
      if(flagEndSort == true )
      {
        for(k = 0; k < SizeArray; k++)
        {
          SortMass[k] = MassV0[k];
        }
       flagEndSort = false;
      }
      flagEnableSort = true;
      EXTI_set(EXTI_Trig_PT.IRQ, Off);                                              // запрещаем прерывание от измерителя пилот тона
      EXTI_set(EXTI_Trig_Valtage.IRQ, On);                                          // разрешаем прерывание по захвату фазы А
      CurentState.status = PilotMeasureDone;                                        // пилот тон померяли

      ADC_SelectChannel(AN1,On);
      ADC_SelectChannel(AN0,Off);
      TogglePin(&C8);
    }
  }
}

/* Сортировка методом пузырька */
/*-------------------------------------*/
void SortM(uint16_t* num, uint16_t size)
{
  uint16_t i = 0;
  uint16_t j = 0;
  uint16_t temp = 0;
  
  for (i = 0; i < size - 1; i++)
  {
    for (j = (size - 1); j > i; j--) 
    {
      if (num[j - 1] > num[j]) 
      {
        temp = num[j - 1]; 
        num[j - 1] = num[j];
        num[j] = temp;
      }
    }
  }
   HightValueCh1 = num[size/2 + size/4 - 1];                              /* берем медиантное значение из диапазона верхних значений */
   LowValueCh1 =   num[size/4 - 1];                                            /* берем медиантное значение из диапазона нижних значений */
   MidlValueCh1 =  num[size/2 - 1];

   flagEndSort = true;
}


/* Настройка внешнего прерывания */
void EXTI_Init(t_EXTI EXTI_Pin )
{
   const TPin* EXTI_Port[1] = {EXTI_Pin.Pin};
   ConfigVirualPort(EXTI_Port,1,IN,_2MHz,PULLDOWN,PP);
   RCC->APB2ENR |= RCC_APB2ENR_SYSCFGCOMPEN;                   // включаем тактирование SYSCFG
   
   if(EXTI_Pin.Pin->Port == GPIOA)
   {
     switch (EXTI_Pin.Pin->Pin)
     {
      case 0: SYSCFG->EXTICR[0] |= (uint16_t)SYSCFG_EXTICR1_EXTI0_PA; break;
      case 1: SYSCFG->EXTICR[0] |= (uint16_t)SYSCFG_EXTICR1_EXTI1_PA; break;
      case 2: SYSCFG->EXTICR[0] |= (uint16_t)SYSCFG_EXTICR1_EXTI2_PA; break;
      case 3: SYSCFG->EXTICR[0] |= (uint16_t)SYSCFG_EXTICR1_EXTI3_PA; break;
      
      case 4: SYSCFG->EXTICR[1] |= (uint16_t)SYSCFG_EXTICR2_EXTI4_PA; break;
      case 5: SYSCFG->EXTICR[1] |= (uint16_t)SYSCFG_EXTICR2_EXTI5_PA; break;
      case 6: SYSCFG->EXTICR[1] |= (uint16_t)SYSCFG_EXTICR2_EXTI6_PA; break;
      case 7: SYSCFG->EXTICR[1] |= (uint16_t)SYSCFG_EXTICR2_EXTI7_PA; break;
      
      case 8: SYSCFG->EXTICR[2] |= (uint16_t)SYSCFG_EXTICR3_EXTI8_PA; break;
      case 9: SYSCFG->EXTICR[2] |= (uint16_t)SYSCFG_EXTICR3_EXTI9_PA; break;
      case 10: SYSCFG->EXTICR[2] |= (uint16_t)SYSCFG_EXTICR3_EXTI10_PA; break;
      case 11: SYSCFG->EXTICR[2] |= (uint16_t)SYSCFG_EXTICR3_EXTI11_PA; break;
      
      case 12: SYSCFG->EXTICR[3] |= (uint16_t)SYSCFG_EXTICR4_EXTI12_PA; break;
      case 13: SYSCFG->EXTICR[3] |= (uint16_t)SYSCFG_EXTICR4_EXTI13_PA; break;
      case 14: SYSCFG->EXTICR[3] |= (uint16_t)SYSCFG_EXTICR4_EXTI14_PA; break;
      case 15: SYSCFG->EXTICR[3] |= (uint16_t)SYSCFG_EXTICR4_EXTI15_PA; break;
     }
  }
   
  if(EXTI_Pin.Pin->Port == GPIOB)
   {
     switch (EXTI_Pin.Pin->Pin)
     {
      case 0: SYSCFG->EXTICR[0] |= (uint16_t)SYSCFG_EXTICR1_EXTI0_PB; break;
      case 1: SYSCFG->EXTICR[0] |= (uint16_t)SYSCFG_EXTICR1_EXTI1_PB; break;
      case 2: SYSCFG->EXTICR[0] |= (uint16_t)SYSCFG_EXTICR1_EXTI2_PB; break;
      case 3: SYSCFG->EXTICR[0] |= (uint16_t)SYSCFG_EXTICR1_EXTI3_PB; break;
      
      case 4: SYSCFG->EXTICR[1] |= (uint16_t)SYSCFG_EXTICR2_EXTI4_PB; break;
      case 5: SYSCFG->EXTICR[1] |= (uint16_t)SYSCFG_EXTICR2_EXTI5_PB; break;
      case 6: SYSCFG->EXTICR[1] |= (uint16_t)SYSCFG_EXTICR2_EXTI6_PB; break;
      case 7: SYSCFG->EXTICR[1] |= (uint16_t)SYSCFG_EXTICR2_EXTI7_PB; break;
      
      case 8: SYSCFG->EXTICR[2] |= (uint16_t)SYSCFG_EXTICR3_EXTI8_PB; break;
      case 9: SYSCFG->EXTICR[2] |= (uint16_t)SYSCFG_EXTICR3_EXTI9_PB; break;
      case 10: SYSCFG->EXTICR[2] |= (uint16_t)SYSCFG_EXTICR3_EXTI10_PB; break;
      case 11: SYSCFG->EXTICR[2] |= (uint16_t)SYSCFG_EXTICR3_EXTI11_PB; break;
      
      case 12: SYSCFG->EXTICR[3] |= (uint16_t)SYSCFG_EXTICR4_EXTI12_PB; break;
      case 13: SYSCFG->EXTICR[3] |= (uint16_t)SYSCFG_EXTICR4_EXTI13_PB; break;
      case 14: SYSCFG->EXTICR[3] |= (uint16_t)SYSCFG_EXTICR4_EXTI14_PB; break;
      case 15: SYSCFG->EXTICR[3] |= (uint16_t)SYSCFG_EXTICR4_EXTI15_PB; break;
     }
  }
   
  if(EXTI_Pin.Pin->Port == GPIOC)
   {
     switch (EXTI_Pin.Pin->Pin)
     {
      case 0: SYSCFG->EXTICR[0] |= (uint16_t)SYSCFG_EXTICR1_EXTI0_PC; break;
      case 1: SYSCFG->EXTICR[0] |= (uint16_t)SYSCFG_EXTICR1_EXTI1_PC; break;
      case 2: SYSCFG->EXTICR[0] |= (uint16_t)SYSCFG_EXTICR1_EXTI2_PC; break;
      case 3: SYSCFG->EXTICR[0] |= (uint16_t)SYSCFG_EXTICR1_EXTI3_PC; break;
      
      case 4: SYSCFG->EXTICR[1] |= (uint16_t)SYSCFG_EXTICR2_EXTI4_PC; break;
      case 5: SYSCFG->EXTICR[1] |= (uint16_t)SYSCFG_EXTICR2_EXTI5_PC; break;
      case 6: SYSCFG->EXTICR[1] |= (uint16_t)SYSCFG_EXTICR2_EXTI6_PC; break;
      case 7: SYSCFG->EXTICR[1] |= (uint16_t)SYSCFG_EXTICR2_EXTI7_PC; break;
      
      case 8: SYSCFG->EXTICR[2] |= (uint16_t)SYSCFG_EXTICR3_EXTI8_PC; break;
      case 9: SYSCFG->EXTICR[2] |= (uint16_t)SYSCFG_EXTICR3_EXTI9_PC; break;
      case 10: SYSCFG->EXTICR[2] |= (uint16_t)SYSCFG_EXTICR3_EXTI10_PC; break;
      case 11: SYSCFG->EXTICR[2] |= (uint16_t)SYSCFG_EXTICR3_EXTI11_PC; break;
      
      case 12: SYSCFG->EXTICR[3] |= (uint16_t)SYSCFG_EXTICR4_EXTI12_PC; break;
      case 13: SYSCFG->EXTICR[3] |= (uint16_t)SYSCFG_EXTICR4_EXTI13_PC; break;
      case 14: SYSCFG->EXTICR[3] |= (uint16_t)SYSCFG_EXTICR4_EXTI14_PC; break;
      case 15: SYSCFG->EXTICR[3] |= (uint16_t)SYSCFG_EXTICR4_EXTI15_PC; break;
     }
  }
   
   if(EXTI_Pin.Pin->Port == GPIOD)
   {
     switch (EXTI_Pin.Pin->Pin)
     {
      case 0: SYSCFG->EXTICR[0] |= (uint16_t)SYSCFG_EXTICR1_EXTI0_PD; break;
      case 1: SYSCFG->EXTICR[0] |= (uint16_t)SYSCFG_EXTICR1_EXTI1_PD; break;
      case 2: SYSCFG->EXTICR[0] |= (uint16_t)SYSCFG_EXTICR1_EXTI2_PD; break;
      case 3: SYSCFG->EXTICR[0] |= (uint16_t)SYSCFG_EXTICR1_EXTI3_PD; break;
      
      case 4: SYSCFG->EXTICR[1] |= (uint16_t)SYSCFG_EXTICR2_EXTI4_PD; break;
      case 5: SYSCFG->EXTICR[1] |= (uint16_t)SYSCFG_EXTICR2_EXTI5_PD; break;
      case 6: SYSCFG->EXTICR[1] |= (uint16_t)SYSCFG_EXTICR2_EXTI6_PD; break;
      case 7: SYSCFG->EXTICR[1] |= (uint16_t)SYSCFG_EXTICR2_EXTI7_PD; break;
      
      case 8: SYSCFG->EXTICR[2] |= (uint16_t)SYSCFG_EXTICR3_EXTI8_PD; break;
      case 9: SYSCFG->EXTICR[2] |= (uint16_t)SYSCFG_EXTICR3_EXTI9_PD; break;
      case 10: SYSCFG->EXTICR[2] |= (uint16_t)SYSCFG_EXTICR3_EXTI10_PD; break;
      case 11: SYSCFG->EXTICR[2] |= (uint16_t)SYSCFG_EXTICR3_EXTI11_PD; break;
      
      case 12: SYSCFG->EXTICR[3] |= (uint16_t)SYSCFG_EXTICR4_EXTI12_PD; break;
      case 13: SYSCFG->EXTICR[3] |= (uint16_t)SYSCFG_EXTICR4_EXTI13_PD; break;
      case 14: SYSCFG->EXTICR[3] |= (uint16_t)SYSCFG_EXTICR4_EXTI14_PD; break;
      case 15: SYSCFG->EXTICR[3] |= (uint16_t)SYSCFG_EXTICR4_EXTI15_PD; break;
     }
  }

   if(EXTI_Pin.Pin->Port == GPIOE)
   {
     switch (EXTI_Pin.Pin->Pin)
     {
      case 0: SYSCFG->EXTICR[0] |= (uint16_t)SYSCFG_EXTICR1_EXTI0_PE; break;
      case 1: SYSCFG->EXTICR[0] |= (uint16_t)SYSCFG_EXTICR1_EXTI1_PE; break;
      case 2: SYSCFG->EXTICR[0] |= (uint16_t)SYSCFG_EXTICR1_EXTI2_PE; break;
      case 3: SYSCFG->EXTICR[0] |= (uint16_t)SYSCFG_EXTICR1_EXTI3_PE; break;
      
      case 4: SYSCFG->EXTICR[1] |= (uint16_t)SYSCFG_EXTICR2_EXTI4_PE; break;
      case 5: SYSCFG->EXTICR[1] |= (uint16_t)SYSCFG_EXTICR2_EXTI5_PE; break;
      case 6: SYSCFG->EXTICR[1] |= (uint16_t)SYSCFG_EXTICR2_EXTI6_PE; break;
      case 7: SYSCFG->EXTICR[1] |= (uint16_t)SYSCFG_EXTICR2_EXTI7_PE; break;
      
      case 8: SYSCFG->EXTICR[2] |= (uint16_t)SYSCFG_EXTICR3_EXTI8_PE; break;
      case 9: SYSCFG->EXTICR[2] |= (uint16_t)SYSCFG_EXTICR3_EXTI9_PE; break;
      case 10: SYSCFG->EXTICR[2] |= (uint16_t)SYSCFG_EXTICR3_EXTI10_PE; break;
      case 11: SYSCFG->EXTICR[2] |= (uint16_t)SYSCFG_EXTICR3_EXTI11_PE; break;
      
      case 12: SYSCFG->EXTICR[3] |= (uint16_t)SYSCFG_EXTICR4_EXTI12_PE; break;
      case 13: SYSCFG->EXTICR[3] |= (uint16_t)SYSCFG_EXTICR4_EXTI13_PE; break;
      case 14: SYSCFG->EXTICR[3] |= (uint16_t)SYSCFG_EXTICR4_EXTI14_PE; break;
      case 15: SYSCFG->EXTICR[3] |= (uint16_t)SYSCFG_EXTICR4_EXTI15_PE; break;
     }
  }
   
   if(EXTI_Pin.Pin->Port == GPIOF)
   {
     switch (EXTI_Pin.Pin->Pin)
     {
      case 0: SYSCFG->EXTICR[0] |= (uint16_t)SYSCFG_EXTICR1_EXTI0_PF; break;
      case 1: SYSCFG->EXTICR[0] |= (uint16_t)SYSCFG_EXTICR1_EXTI1_PF; break;
      case 2: SYSCFG->EXTICR[0] |= (uint16_t)SYSCFG_EXTICR1_EXTI2_PF; break;
      case 3: SYSCFG->EXTICR[0] |= (uint16_t)SYSCFG_EXTICR1_EXTI3_PF; break;
      
      case 4: SYSCFG->EXTICR[1] |= (uint16_t)SYSCFG_EXTICR2_EXTI4_PF; break;
      case 5: SYSCFG->EXTICR[1] |= (uint16_t)SYSCFG_EXTICR2_EXTI5_PF; break;
      case 6: SYSCFG->EXTICR[1] |= (uint16_t)SYSCFG_EXTICR2_EXTI6_PF; break;
      case 7: SYSCFG->EXTICR[1] |= (uint16_t)SYSCFG_EXTICR2_EXTI7_PF; break;
      
      case 9: SYSCFG->EXTICR[2] |= (uint16_t)SYSCFG_EXTICR3_EXTI9_PF; break;
      case 10: SYSCFG->EXTICR[2] |= (uint16_t)SYSCFG_EXTICR3_EXTI10_PF; break;
     }
  }
  
// SYSCFG->EXTICR[0] |= (uint16_t)SYSCFG_EXTICR1_EXTI0_PC;     // EXTICR[0] - регистр, SYSCFG_EXTICR1_EXTI0_PC включить мультиплексор на С0
   EXTI->IMR |= 1 << EXTI_Pin.Pin->Pin;                                 // разрешаем прерывание на пин 
  // EXTI->IMR = 0x0001;                                         
   if(EXTI_Pin.ModeTriger == RiseEdge)                                  // разрешаем прерывание по переднему фронту
   {
     EXTI->RTSR |= 1 << EXTI_Pin.Pin->Pin;                      
   }
   
   if(EXTI_Pin.ModeTriger == FadeEdge)                                  // разрешаем прерывание по заднему фронту
   {
     EXTI->FTSR |= 1 << EXTI_Pin.Pin->Pin;                      
   }
   
   if(EXTI_Pin.ModeTriger == BothEdge)                                  // разрешаем прерывание по обоим фронтам
   {
     EXTI->FTSR |= 1 << EXTI_Pin.Pin->Pin;
     EXTI->RTSR |= 1 << EXTI_Pin.Pin->Pin;
   }
   //EXTI->RTSR = 0x0001;                                               // разрешаем прерывание по переднему фронту
   //EXTI->FTSR = 0x0001;                                               // разрешаем прерывание по заднему фронту
//   NVIC_EnableIRQ(EXTI0_1_IRQn);                                      // Разрешаем общее прерывание
   EXTI_set(EXTI_Pin.IRQ, On);
   NVIC_SetPriority(EXTI_Pin.IRQ, EXTI_Pin.LevelOfPririty & 0x0f);      //  высокий приоритет
   __enable_irq();                                                      // Разрешаем прерывания глобальные
}


/* разрешение / запрет прерывания */
void EXTI_set( IRQn_Type IRQ, t_State state )
{
  if(state == On)
  {
    NVIC_EnableIRQ(IRQ);
  }
  else
  {
    NVIC_DisableIRQ(IRQ);
  }
}
