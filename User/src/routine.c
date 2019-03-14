#include "routine.h"

extern bool flagEnableSort;
extern bool flagEndSort;

extern bool flagEnableSort;
extern bool flagEndSort;
extern uint16_t SortMass[SizeArray];
extern t_EXTI EXTI_Trig_PT;
extern t_EXTI EXTI_Trig_Valtage;
extern t_Timer Tim15;
extern uint16_t LowValueCh1;
extern uint16_t HightValueCh1;
extern uint16_t MidlValueCh1;
extern t_Status CurentState;
extern const TPin C9;

uint16_t PilotTonH = 0;
uint16_t PilotTonL = 0;



uint16_t TempArrPhaseA[SizeArrVolt] = {0};
uint16_t TempArrPhaseB[SizeArrVolt] = {0};
uint16_t TempArrPhaseC[SizeArrVolt] = {0};

uint16_t ValuePhaseA;
uint16_t ValuePhaseB;
uint16_t ValuePhaseC;

uint16_t ArrPhaseA[SizeOfTwoStageArrayVoltage] = {0};
uint16_t ArrPhaseB[SizeOfTwoStageArrayVoltage] = {0};
uint16_t ArrPhaseC[SizeOfTwoStageArrayVoltage] = {0};



void MeasureProccesing(void)
{
   uint8_t i = 0;
   uint32_t TimeCnt = 0;
   uint8_t k = 0;
   static uint32_t Temp = 0;
   static uint8_t iA, iB, iC;
      
                                                                             /* Запускаем сортировку массива */
      if(flagEnableSort == true)
      {
         flagEnableSort = false;
         SortM(SortMass, SizeArray);
         PilotTonH = ( HightValueCh1 * 3300 ) / (( 1 << 12 ) - 1);
         PilotTonL = ( LowValueCh1 * 3300   ) / (( 1 << 12 ) - 1);
      }
      
      
/* Меряем фазу А */
      if(CurentState.status == Phase_A_MeasureStart)                          // если начинаем мерять фазу А, то...
      {
         Temp = ADC1->DR;                                                     // сброс паразитных данных
         Temp = 0;
         TimeCnt = 0;
        
         ADC1->CR |= ADC_CR_ADSTART;                                          // включаем преобразование
         for( i = 0; i < SizeArrVolt; i++ )
         {
           while ((ADC1->ISR & ADC_ISR_EOC) == 0 && TimeCnt <= TimeOut)       // Ожидаем флаг окончания преобразования
           {
            TimeCnt++;                                                        // Тайм аут
           }
          TempArrPhaseA[i] = ADC1->DR;                                        // Сохраняем значения в буфере
          TimeCnt = 0;
         }
         for(k = 0; k < SizeArrVolt; k++ )
         {
          Temp = Temp + TempArrPhaseA[k];
         }
/*------------------Накопление--------------------------*/
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
         TIM15->ARR = TimeBeforMeasurePhaseB - 1;                             // изменим период счета
      }
      
      
/* Меряем фазу B */
      if(CurentState.status == Phase_B_MeasureStart)
      {
        Temp = ADC1->DR;                                                     // сброс паразитных данных
        
        Temp = 0;
        TimeCnt = 0;
        ADC1->CR |= ADC_CR_ADSTART;                                           // включаем преобразование
        for( i = 0; i < SizeArrVolt; i++ )
        {
          while ((ADC1->ISR & ADC_ISR_EOC) == 0 && TimeCnt <= TimeOut)        // Ожидаем флаг окончания преобразования
          {
           TimeCnt++;                                                         // Тайм аут
          }
         TempArrPhaseB[i] = ADC1->DR;                                         // Сохраняем значения в буфере
         TimeCnt = 0;
        }
        for(k = 0; k < SizeArrVolt; k++ )
        {
          Temp = Temp + TempArrPhaseB[k];
        }
/*------------------Накопление--------------------------*/
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
        TIM15->ARR = TimeBeforMeasurePhaseC - 1;                             // изменим период счета
      }
      
      
      
/* Меряем фазу C */
      if(CurentState.status == Phase_C_MeasureStart)
      {
        Temp = ADC1->DR;                                                     // сброс паразитных данных
        Temp = 0;
        TimeCnt = 0;
        ADC1->CR |= ADC_CR_ADSTART;                                         // включаем преобразование
        for( i = 0; i < SizeArrVolt; i++ )
        {
          while ((ADC1->ISR & ADC_ISR_EOC) == 0 && TimeCnt <= TimeOut)      // Ожидаем флаг окончания преобразования
          {
           TimeCnt++;                                                       // Тайм аут
          }
         TempArrPhaseC[i] = ADC1->DR;                                       // Сохраняем значения в буфере
         TimeCnt = 0;
        }
        for(k = 0; k < SizeArrVolt; k++ )
        {
          Temp = Temp + TempArrPhaseC[k];
        }
/*------------------Накопление--------------------------*/
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
        NVIC_DisableIRQ(TIM15_IRQn);                                        // Запрещаем прерывание
        TIM15->CR1  &= ~ TIM_CR1_CEN;                                       // выключаем таймер
        TIM15->DIER &= ~ TIM_DIER_UIE;                                      // запрещаем прерывание от таймера
        
        EXTI_set(EXTI_Trig_PT.IRQ, On);                                      // запрещаем прерывание от измерителя пилот тона
        EXTI_set(EXTI_Trig_Valtage.IRQ, Off);                                // разрешаем прерывание по захвату фазы А
      }

}
