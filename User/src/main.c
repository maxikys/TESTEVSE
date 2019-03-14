/**
  ******************************************************************************
  * @file    main.c
  * @author  �������� ������
  * @version V1.0.0
  * @date    21.04.16
  * @brief   
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/ 
#include  "main.h"
#define SizeArrVolt 1
#define TimeOut 10000
#define SizeOfTwoStageArrayVoltage 50 


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

const TPin C9 = {GPIOC,9}; /* ������ ���. ���������� */
const TPin C8 = {GPIOC,8}; /* ������ ���. ����� ���� */
const TPin C6 = {GPIOC,6}; /* ����. ����. */
const TPin C5 = {GPIOC,5}; /* ���������� �� �������� �������15. */
/*-------------------------------------------------------------*/
t_Status CurentState;
const TPin* SimpleAnalogInput[]  = {&A0, &A1, &A4, &B0};
const TPin* ExtAnalogInput[]     = {&A13, &A14};
const TPin* TestPin[]            = {&C5, &C6, &C8, &C9};
const TPin* MCOPin[]             = {&MCO};

uint16_t TempArrPhaseA[SizeArrVolt] = {0};
uint16_t TempArrPhaseB[SizeArrVolt] = {0};
uint16_t TempArrPhaseC[SizeArrVolt] = {0};

uint16_t ValuePhaseA;
uint16_t ValuePhaseB;
uint16_t ValuePhaseC;

uint16_t ArrPhaseA[SizeOfTwoStageArrayVoltage] = {0};
uint16_t ArrPhaseB[SizeOfTwoStageArrayVoltage] = {0};
uint16_t ArrPhaseC[SizeOfTwoStageArrayVoltage] = {0};

uint16_t PilotTonH = 0;
uint16_t PilotTonL = 0;



void Func1(void)
{
  EXTI_set(EXTI_Trig_PT.IRQ, On);
}



int main(void)
{
   uint8_t i = 0;
   uint32_t TimeCnt = 0;
   uint8_t k = 0;
   static uint32_t Temp = 0;
   static uint8_t iA, iB, iC;
  
   ClockInit(HSI,8000000,48000000);
   SystickInit(1000);                                                         // ������������� Systick
   ConfigVirualPort(SimpleAnalogInput,4,AN,_2MHz,NOPULL,PP);                  /* ����������� ����� �� ���������� ���� � ���������� ������ */
   ConfigVirualPort(TestPin, 3, OUT,_2MHz,NOPULL,PP);
   ConfigVirualPort(MCOPin, 1, AF,_50MHz,NOPULL,PP);
   PinAltFunc(&MCO,AF0);
   
   if( ADC_InitADC() == true ) CurentState.status = PilotMeasureStart;        // ����������� ���.
   //ADC_SelectChannel(AN0,On);                                                 // �������� ����� ��������� ����� ����
   //ADC_SelectChannel(AN1,Off);
   EXTI_Init(EXTI_Trig_PT);                                                   // ������������� �������� ���������� ��� ����� ����
   EXTI_Init(EXTI_Trig_Valtage);                                              // ������������� �������� ���������� ��� ��������� ����������
   EXTI_set(EXTI_Trig_Valtage.IRQ, Off);                                      // ��������� ���������� �� ������� ���� �
   pwm_Init(0);
   pwm_Set(0, 100);                                                           /* �������� ��� ��� ������� */
   //SystickAddFunction(1,&Func1);                                            /* ������.�. */
   timerInit(&Tim15, On);                                                     // �������� ������������ �������
   SetCountValue( &Tim15, TimeBeforMeasurePhaseA, ValueCountTim15 );
   output_low(&C9);
   output_low(&C8);
   output_low(&C6);
   output_low(&C5);

   RCC->CFGR |=  RCC_CFGR_MCO_PLL;
   RCC->CFGR |=RCC_CFGR_MCO_PRE_4;
   while (1)
   {
      //SystickScanEvent();                                                  // ������� ������������ ������� Systick
                                                                             /* ��������� ���������� ������� */
      if(flagEnableSort == true)
      {
         flagEnableSort = false;
         SortM(SortMass, SizeArray);
         PilotTonH = HightValueCh1;
         PilotTonL = LowValueCh1;
      }
      
      
/* ������ ���� � */
      if(CurentState.status == Phase_A_MeasureStart)                          // ���� �������� ������ ���� �, ��...
      {
         Temp = ADC1->DR;                                                     // ����� ���������� ������
         Temp = 0;
         TimeCnt = 0;
        
         ADC1->CR |= ADC_CR_ADSTART;                                          // �������� ��������������
         for( i = 0; i < SizeArrVolt; i++ )
         {
           while ((ADC1->ISR & ADC_ISR_EOC) == 0 && TimeCnt <= TimeOut)       // ������� ���� ��������� ��������������
           {
            TimeCnt++;                                                        // ���� ���
           }
          TempArrPhaseA[i] = ADC1->DR;                                        // ��������� �������� � ������
          TimeCnt = 0;
         }
         for(k = 0; k < SizeArrVolt; k++ )
         {
          Temp = Temp + TempArrPhaseA[k];
         }
/*------------------����������--------------------------*/
         if( iA < SizeOfTwoStageArrayVoltage )
         {
             ArrPhaseA[iA++] = Temp;
         }
         else
         {
           SortM(ArrPhaseA, SizeOfTwoStageArrayVoltage);
           ValuePhaseA = ( MidlValueCh1 * 3300 ) / (( 1 << 12 ) - 1);
           iA = 0;
         }
/*------------------------------------------------------*/
         CurentState.status = Phase_A_MeasureDone;
         TogglePin(&C9);
         TIM15->ARR = TimeBeforMeasurePhaseB - 1;                             // ������� ������ �����
      }
      
      
/* ������ ���� B */
      if(CurentState.status == Phase_B_MeasureStart)
      {
        Temp = 0;
        TimeCnt = 0;
        ADC1->CR |= ADC_CR_ADSTART;                                           // �������� ��������������
        for( i = 0; i < SizeArrVolt; i++ )
        {
          while ((ADC1->ISR & ADC_ISR_EOC) == 0 && TimeCnt <= TimeOut)        // ������� ���� ��������� ��������������
          {
           TimeCnt++;                                                         // ���� ���
          }
         TempArrPhaseB[i] = ADC1->DR;                                         // ��������� �������� � ������
         TimeCnt = 0;
        }
        for(k = 0; k < SizeArrVolt; k++ )
        {
          Temp = Temp + TempArrPhaseB[k];
        }
/*------------------����������--------------------------*/
         if( iB < SizeOfTwoStageArrayVoltage )
         {
             ArrPhaseB[iB++] = Temp;
         }
         else
         {
           SortM(ArrPhaseB, SizeOfTwoStageArrayVoltage);
           ValuePhaseB = ( MidlValueCh1 * 3300 ) / (( 1 << 12 ) - 1);
           iB = 0;
         }
/*------------------------------------------------------*/
        CurentState.status = Phase_B_MeasureDone;
        TogglePin(&C9);
        TIM15->ARR = TimeBeforMeasurePhaseC - 1;                             // ������� ������ �����
      }
      
      
      
/* ������ ���� C */
      if(CurentState.status == Phase_C_MeasureStart)
      {
        Temp = 0;
        TimeCnt = 0;
        ADC1->CR |= ADC_CR_ADSTART;                                         // �������� ��������������
        for( i = 0; i < SizeArrVolt; i++ )
        {
          while ((ADC1->ISR & ADC_ISR_EOC) == 0 && TimeCnt <= TimeOut)      // ������� ���� ��������� ��������������
          {
           TimeCnt++;                                                       // ���� ���
          }
         TempArrPhaseC[i] = ADC1->DR;                                       // ��������� �������� � ������
         TimeCnt = 0;
        }
        for(k = 0; k < SizeArrVolt; k++ )
        {
          Temp = Temp + TempArrPhaseC[k];
        }
/*------------------����������--------------------------*/
         if( iC < SizeOfTwoStageArrayVoltage )
         {
             ArrPhaseC[iC++] = Temp;
         }
         else
         {
           SortM(ArrPhaseC, SizeOfTwoStageArrayVoltage);
           ValuePhaseC = ( MidlValueCh1 * 3300 ) / (( 1 << 12 ) - 1);
           iC = 0;
         }
/*------------------------------------------------------*/
        CurentState.status = Phase_C_MeasureDone;
        TogglePin(&C9);
        
        
        /*-----------------------------*/
        NVIC_DisableIRQ(TIM15_IRQn);                                        // ��������� ����������
        TIM15->CR1  &= ~ TIM_CR1_CEN;                                       // ��������� ������
        TIM15->DIER &= ~ TIM_DIER_UIE;                                      // ��������� ���������� �� �������
        
        EXTI_set(EXTI_Trig_PT.IRQ, On);                                      // ��������� ���������� �� ���������� ����� ����
        EXTI_set(EXTI_Trig_Valtage.IRQ, Off);                                // ��������� ���������� �� ������� ���� �
      }

   }
}

