/**
  ******************************************************************************
  * @file    main.c
  * @author  Ткаченко Максим
  * @version V1.0.0
  * @date    21.04.16
  * @brief   
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/ 
#include  "main.h"



/* External difinitions */
extern bool flagEnableSort;
extern bool flagEndSort;
extern uint16_t SortMass[SizeArray];
extern t_EXTI EXTI_Trig_PT;
extern t_EXTI EXTI_Trig_Valtage;
extern t_Timer Tim15;
extern uint16_t LowValueCh1;
extern uint16_t HightValueCh1;
extern uint16_t MidlValueCh1;

/* Definitions of varable */

const TPin MCO = {GPIOA, 8};

const TPin A0 = {GPIOA,0};
const TPin A1 = {GPIOA,1};
const TPin A4 = {GPIOA,4};
const TPin B0 = {GPIOB,0};

const TPin A13 = {GPIOA,13};
const TPin A14 = {GPIOA,14};

const TPin C9 = {GPIOC,9}; /* период изм. напряжения */
const TPin C8 = {GPIOC,8}; /* период изм. пилот тона */
const TPin C6 = {GPIOC,6}; /* триг. напр. */
const TPin C5 = {GPIOC,5}; /* прерывание по сработке таймера15. */
/*-------------------------------------------------------------*/
t_Status CurentState;
const TPin* SimpleAnalogInput[]  = {&A0, &A1, &A4, &B0};
const TPin* ExtAnalogInput[]     = {&A13, &A14};
const TPin* TestPin[]            = {&C5, &C6, &C8, &C9};
const TPin* MCOPin[]             = {&MCO};






void Func1(void)
{
  EXTI_set(EXTI_Trig_PT.IRQ, On);
}



int main(void)
{

  
   ClockInit(HSI,8000000,48000000);
   SystickInit(1000);                                                         // Инициилизация Systick
   ConfigVirualPort(SimpleAnalogInput,4,AN,_2MHz,NOPULL,PP);                  /* Настраиваем нажки на считывание инфы с аналоговых входов */
   ConfigVirualPort(TestPin, 4, OUT,_2MHz,NOPULL,PP);
   ConfigVirualPort(MCOPin, 1, AF,_50MHz,NOPULL,PP);
   PinAltFunc(&MCO,AF0);
   
   if( ADC_InitADC() == true ) CurentState.status = PilotMeasureStart;        // Настраиваем АЦП.
   //ADC_SelectChannel(AN0,On);                                                 // включаем сразу измерение пилот тона
   //ADC_SelectChannel(AN1,Off);
   EXTI_Init(EXTI_Trig_PT);                                                   // инициализация внешнего прерывания для пилот тона
   EXTI_Init(EXTI_Trig_Valtage);                                              // инициализация внешнего прерывания для измерения напряжения
   EXTI_set(EXTI_Trig_Valtage.IRQ, Off);                                      // запрещаем прерывание по захвату фазы А
   pwm_Init(0);
   pwm_Set(0, 100);                                                           /* Тестовый шим для отладки */
   //SystickAddFunction(1,&Func1);                                            /* Переод.ф. */
   timerInit(&Tim15, On);                                                     // включаем тактирование таймера
   SetCountValue( &Tim15, TimeBeforMeasurePhaseA, ValueCountTim15 );
   output_low(&C9);
   output_low(&C8);
   output_low(&C6);
   output_low(&C5);

   RCC->CFGR |=  RCC_CFGR_MCO_PLL;
   RCC->CFGR |=RCC_CFGR_MCO_PRE_4;
   while (1)
   {
     MeasureProccesing();                                                 // мериловка
     SystickScanEvent();                                                  // Функция сканирования событий Systick

   }
}

