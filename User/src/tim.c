#include "tim.h"

/* extern difinition */
extern const TPin C9;
extern t_Status CurentState;
extern const TPin C5;
/* Smart type */
t_Timer Tim15 = {TIM15, RCC_APB2ENR_TIM15EN, &RCC->APB2ENR, TIM15_IRQn, 0};     /* ��������� ��� ��������� TIM15 */


/* ��������� ������������ */
__INLINE uint32_t GetPrescaller( uint32_t freq)
{
  uint32_t Clock = clock_GetAPB();                                              // ������� ����
  return Clock/freq - 1;                                                        // ������ �������� ������������
}

/* ������������� ������� */
void timerInit(t_Timer* Timer, t_State state)
{
  
  Timer->TIM->CNT = 0;
  if(state == On)
  {
    *Timer->RCCReg |= Timer->RCCRegMask;                                          // �������� ������������ �������
    Timer->TIM->DIER |= TIM_DIER_UIE;
  }
  else
  {
    *Timer->RCCReg &= ~Timer->RCCRegMask;                                          // ��������� ������������ �������
    Timer->TIM->DIER &= ~TIM_DIER_UIE;
  }
  NVIC_SetPriority(Timer->IRQ, Timer->LevelOfPririty);                            // ��������� ����������
}

/* ��������� ������ �������� ������� */
__INLINE void SetCountValue(t_Timer* timer, uint16_t value, uint32_t freq)
{
  NVIC_DisableIRQ(timer->IRQ);                                                  // ��������� ����������
  timer->TIM->CR1  &= ~ TIM_CR1_CEN;                                            // ��������� ������
  timer->TIM->DIER &= ~ TIM_DIER_UIE;                                           // ��������� ���������� �� �������
  
  timer->TIM->ARR = value - 1;                                                  // ������� �������� �� �������� ����� ������� ������
  timer->TIM->PSC = GetPrescaller(freq);                                        // �������  ���
  //timer->TIM->CNT = 0;
  timer->TIM->DIER |= TIM_DIER_UIE;                                             // ��������� ���������� �� �������
  timer->TIM->CR1  |= TIM_CR1_CEN;                                              // �������� ������
  
  NVIC_EnableIRQ(timer->IRQ);                                                   // ��������� ���������� �� �������
}

/* ��������������� ���������� ���������� */
__INLINE void Timer_Handler(t_Timer* timer)
{
  timer->TIM->SR &= ~ TIM_SR_UIF;                                               // ���������� ���� ����������
  //SetCountValue(timer, value, freq);                                          // ���������� ����� ��������, ����� ������� ��������� ���������� 
}


/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/* ��������� ���������� �������, ��� ������� ������� ���� */
void TIM15_IRQHandler(void) 
{
//   /* ������ ��������� ���� � */
    Timer_Handler(&Tim15);   /* ������� ���������� */
  
    if(CurentState.status == PilotMeasureDone)
    {
      TogglePin(&C5);
      CurentState.status = Phase_A_MeasureStart;                                // ���������� ���� ��������� ��� ������ ����� ����������� ��������� ���� �
      //ADC_SelectChannel(AN1,On);                                                // ����������� �� ��������� ����������
      //ADC_SelectChannel(AN0,Off);

    }
   /* ������ ��������� ���� B */
    if(CurentState.status == Phase_A_MeasureDone)
    {
      CurentState.status = Phase_B_MeasureStart;                                // ���������� ���� ��������� ��� ������ ����� ����������� ��������� ���� B
      //ADC_SelectChannel(AN1,On);                                                // ����������� �� ��������� ����������
      TogglePin(&C5);
    }

   /* ������ ��������� ���� � */
    if(CurentState.status == Phase_B_MeasureDone)
    {
      CurentState.status = Phase_C_MeasureStart;                                // ���������� ���� ��������� ��� ������ ����� ����������� ��������� ���� B
      //ADC_SelectChannel(AN1,On);                                                // ����������� �� ��������� ����������
      TogglePin(&C5);
    }

    
 
}

