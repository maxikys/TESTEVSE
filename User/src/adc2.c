#include "adc2.h"
#define TimeOut 10000




/* ���������� ��� */
/*--------------------------------------------------------------------------------------*/
__INLINE bool ADC_Calibrate(void) 
{
  uint32_t Cnt = 0;
  /* (1) Ensure that ADEN = 0 */
  /* (2) Clear ADEN */ 
  /* (3) Launch the calibration by setting ADCAL */
  /* (4) Wait until ADCAL=0 */
  if ((ADC1->CR & ADC_CR_ADEN) != 0) /* (1) */
  {
    ADC1->CR &= (uint32_t)(~ADC_CR_ADEN);  /* (2) */  
  }
  ADC1->CR |= ADC_CR_ADCAL; /* (3) */
  while ((ADC1->CR & ADC_CR_ADCAL) != 0 && Cnt <= TimeOut ) /* (4) */
  {
    Cnt++;
  }
   if( Cnt >= TimeOut )
  {
    return false;
  }
  return true;
}

/* ����� ��� */
/*--------------------------------------------------------------------------------------*/

__INLINE bool ADC_Start(void)
{
   uint32_t Cnt = 0;
  do{
    ADC1->CR |= ADC_CR_ADEN; /* (1) */
    Cnt++;
    }
  while ( (ADC1->ISR & ADC_ISR_ADRDY) == 0 && Cnt <= TimeOut );
  if( Cnt >= TimeOut )
  {
    return false;
  }
    return true;
}

/* ��������� ��� */
/*--------------------------------------------------------------------------------------*/

__INLINE bool ADC_Stop(void)
{
  uint32_t Cnt = 0;
  /* (1) Ensure that no conversion on going */
  /* (2) Stop any ongoing conversion */
  /* (3) Wait until ADSTP is reset by hardware i.e. conversion is stopped */
  /* (4) Disable the ADC */
  /* (5) Wait until the ADC is fully disabled */
  if ((ADC1->CR & ADC_CR_ADSTART) != 0) /* (1) */
  {
    ADC1->CR |= ADC_CR_ADSTP; /* (2) */
  }
  while ( (ADC1->CR & ADC_CR_ADSTP) != 0 && Cnt <= TimeOut ) /* (3) */
  {
     Cnt++;
  }
  if( Cnt >= TimeOut )// ������� �� ��������
  {
    return false;
  }
  ADC1->CR |= ADC_CR_ADDIS; /* (4) */
  Cnt = 0;
  while ( (ADC1->CR & ADC_CR_ADEN) != 0 && Cnt <= TimeOut ) /* (5) */
  {
    Cnt++;
  }
  if( Cnt >= TimeOut )
  {
    return false;
  }
  return true;
}


/* ���������� HSI */
/*--------------------------------------------------------------------------------------*/
__INLINE void HSI14(t_State state)
{
  if(state == On)
   {
   RCC->CR2 |= RCC_CR2_HSI14ON; /* (2) */
   }
  if(state == Off)
   {
    RCC->CR2 &= ~RCC_CR2_HSI14ON;
   }
}


/* ���������� ������������� ��� */
/*--------------------------------------------------------------------------------------*/
__INLINE void AdcTalc(t_State state)
{
  if(state == On)
  {
   RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
  }
  if(state == Off)
  {
   RCC->APB2ENR &= ~RCC_APB2ENR_ADC1EN;
  }
}


/* ����� ������ � �������� ����� ����������� ���������� */
/*--------------------------------------------------------------------------------------*/
__INLINE void ADC_SelectChannel(t_ADC_Channel AN, t_State state)
 {
   (state == On)?(ADC1->CHSELR |= (1 << AN) & 0x3ffff):(ADC1->CHSELR &= ~(1 << AN) & 0x3ffff);
 }

/* ������������� ��� */
/*--------------------------------------------------------------------------------------*/  
bool ADC_InitADC(void)
{
  uint8_t Cnt = 0;

  AdcTalc(On);                                                        // ������������
  HSI14(On);                                                          // ��������� ����������� 14 ��� ���������� RC
  if( ADC_Stop()      == true ) {Cnt++;};                             // ����� ����������� �������� ���
  if( ADC_Calibrate() == true ) {Cnt++;};                             // ����������
  if( ADC_Start()     == true ) {Cnt++;};                             // ������ ��������������
  ADC1->SMPR |= ADC_SMPR_SMP_0 | ADC_SMPR_SMP_1 | ADC_SMPR_SMP_2;     /* ����� ������� ���, 17.1 ��(��������� ���������) */
  if( Cnt == 3 ) return true;
  else return false;
}
