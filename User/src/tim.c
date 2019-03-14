#include "tim.h"

/* extern difinition */
extern const TPin C9;
extern t_Status CurentState;
extern const TPin C5;
/* Smart type */
t_Timer Tim15 = {TIM15, RCC_APB2ENR_TIM15EN, &RCC->APB2ENR, TIM15_IRQn, 0};     /* структура для настройки TIM15 */


/* Установка предделителя */
__INLINE uint32_t GetPrescaller( uint32_t freq)
{
  uint32_t Clock = clock_GetAPB();                                              // Частота шины
  return Clock/freq - 1;                                                        // вернем значение предделителя
}

/* Инициализация таймера */
void timerInit(t_Timer* Timer, t_State state)
{
  
  Timer->TIM->CNT = 0;
  if(state == On)
  {
    *Timer->RCCReg |= Timer->RCCRegMask;                                          // включаем тактирование таймера
    Timer->TIM->DIER |= TIM_DIER_UIE;
  }
  else
  {
    *Timer->RCCReg &= ~Timer->RCCRegMask;                                          // выключаем тактирование таймера
    Timer->TIM->DIER &= ~TIM_DIER_UIE;
  }
  NVIC_SetPriority(Timer->IRQ, Timer->LevelOfPririty);                            // приоритет прерывания
}

/* установка нового значения таймера */
__INLINE void SetCountValue(t_Timer* timer, uint16_t value, uint32_t freq)
{
  NVIC_DisableIRQ(timer->IRQ);                                                  // Запрещаем прерывание
  timer->TIM->CR1  &= ~ TIM_CR1_CEN;                                            // выключаем таймер
  timer->TIM->DIER &= ~ TIM_DIER_UIE;                                           // запрещаем прерывание от таймера
  
  timer->TIM->ARR = value - 1;                                                  // заносим значение до которого будет считать таймер
  timer->TIM->PSC = GetPrescaller(freq);                                        // частота  Мгц
  //timer->TIM->CNT = 0;
  timer->TIM->DIER |= TIM_DIER_UIE;                                             // разрешаем прерывание от таймера
  timer->TIM->CR1  |= TIM_CR1_CEN;                                              // включаем таймер
  
  NVIC_EnableIRQ(timer->IRQ);                                                   // разрешаем прерывание от таймера
}

/* комбинированный обработчик прерывания */
__INLINE void Timer_Handler(t_Timer* timer)
{
  timer->TIM->SR &= ~ TIM_SR_UIF;                                               // Сбрасываем флаг прерывания
  //SetCountValue(timer, value, freq);                                          // Записываем новое значение, через которое сработает прерывание 
}


/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/* обработка прерывания таймера, для каждого таймера свой */
void TIM15_IRQHandler(void) 
{
//   /* начало измерения фазы А */
    Timer_Handler(&Tim15);   /* сбросим прерывание */
  
    if(CurentState.status == PilotMeasureDone)
    {
      TogglePin(&C5);
      CurentState.status = Phase_A_MeasureStart;                                // выставляем флаг говорящий что сейчас будем производить измерение фазы А
      //ADC_SelectChannel(AN1,On);                                                // настраиваем на измерения напряжения
      //ADC_SelectChannel(AN0,Off);

    }
   /* начало измерения фазы B */
    if(CurentState.status == Phase_A_MeasureDone)
    {
      CurentState.status = Phase_B_MeasureStart;                                // выставляем флаг говорящий что сейчас будем производить измерение фазы B
      //ADC_SelectChannel(AN1,On);                                                // настраиваем на измерения напряжения
      TogglePin(&C5);
    }

   /* начало измерения фазы С */
    if(CurentState.status == Phase_B_MeasureDone)
    {
      CurentState.status = Phase_C_MeasureStart;                                // выставляем флаг говорящий что сейчас будем производить измерение фазы B
      //ADC_SelectChannel(AN1,On);                                                // настраиваем на измерения напряжения
      TogglePin(&C5);
    }

    
 
}

