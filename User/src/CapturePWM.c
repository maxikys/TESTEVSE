#include "CapturePWM.h"

volatile uint16_t IC2Value = 0;
volatile uint16_t IC1Value = 0;

    

    RCC_ClocksTypeDef RCC_Clocks;
    NVIC_InitTypeDef NVIC_InitStructure;
    TIM_TimeBaseInitTypeDef timer_base;
    TIM_ICInitTypeDef  TIM_ICInitStructure;
    
 extern uint16_t Correct_W_from_EP;/*Значение яркости белого*/
 extern uint16_t Correct_G_from_EP;/*Значение яркости зеленого*/
 
 extern uint16_t Correct_W_from_EP_withoutPWM;/*Значение яркости белого   без ШИМа*/
 extern uint16_t Correct_G_from_EP_withoutPWM;/*Значение яркости зеленого без ШИМа*/

 float tempW,tempG;

 extern uint8_t count_impuls_PWM;

 extern uint16_t TimPeriod;

 const TPin PwmInPin ={GPIOB,5}; /*Вход захвата ШИМ*/

 const TPin* CapturePin[] = {&PwmInPin};

/* Прерывание по обработке захвата ШИМ */

 void TIM3_IRQHandler(void)
 {
   if (TIM_GetITStatus(TIM3, TIM_IT_CC2) != RESET)
  {
    static uint8_t i = 0, period = 10;
    uint8_t k = 0;
    static uint16_t temp1[10], temp2[10];
    static uint16_t IC2 = 0,IC1 = 0;
      
  TIM_ClearITPendingBit(TIM3, TIM_IT_CC2);
  NVIC_DisableIRQ(TIM3_IRQn);
  
    
    temp2[i] = TIM_GetCapture2(TIM3);
    temp1[i] = TIM_GetCapture1(TIM3);
    i++;
    if( i == period)/* усреднение значений */
    {
     for(k = 0; k < period; k++)
        {
        IC2 = IC2 + temp2[i];
        IC1 = IC1 + temp1[i];   
        }   
     i = 0;
    IC2Value = IC2/period;
    IC1Value = IC1/period;
    }
    
    
  IC2Value = TIM_GetCapture2(TIM3);
  IC1Value = TIM_GetCapture1(TIM3);
      
  count_impuls_PWM = 0;
   
    if (TIM_GetFlagStatus(TIM3, TIM_FLAG_CC2OF) != RESET)
    {
      TIM_ClearFlag(TIM3, TIM_FLAG_CC2OF);
      /* обработка пропущеного события*/
    }

    NVIC_EnableIRQ(TIM3_IRQn);
  }
 }

/*процедура гаммакорекции по модулю 4 */
 float GammaCorrect(uint16_t value2, uint16_t value1)
    {
    double result = (double)1.0 - ((double)value1/(float)value2);
	return result*result*result*result; // gamma = 4
    }
    
/* запись значения ШИМ в HCT595 */
 void set_pwm_width_norm(int channel, int pwm_period, float duty_cycle)
    {
	int pwm_pulses = 1000 - (pwm_period)*(float)duty_cycle;
	switch (channel){
		case 3: {if(pwm_pulses <= 1000)TIM_SetCompare3(TIM2, pwm_pulses); break;}
		case 4: {if(pwm_pulses <= 1000)TIM_SetCompare4(TIM2, pwm_pulses); break;}
	}
}

  /*Проверка значения шим*/
 void CorrectBrightness(void)
 {
    static uint16_t temp_IC2Value,temp_IC1Value;
     
     if(IC2Value != temp_IC2Value || IC1Value != temp_IC1Value) /*если новое значение отличается от старого, то применяем корретировку.*/
     {
     if(count_impuls_PWM < 5)
     {
    tempW = GammaCorrect(IC2Value,IC1Value);
    set_pwm_width_norm(3, Correct_W_from_EP * 10, tempW);
    
    tempG = GammaCorrect(IC2Value,IC1Value);
    set_pwm_width_norm(4, Correct_G_from_EP * 10, tempG);
     }
     temp_IC2Value = IC2Value;
     temp_IC1Value = IC1Value;
    }
 }
 
 /* Конфигурирование таймера на захват ШИМ */
void TIM_Config(void)
{
    RCC_GetClocksFreq(&RCC_Clocks);
    ConfigVirualPort(CapturePin,1,AF,_50MHz,PULLUP,PP);
    PinAltFunc(CapturePin[0],AF1);

  /* TIM3 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);/*тактирование таймера 1*/

   /* Предделитель для того чтобы не вылетал за пределы счета, по умолчанию считает с частотой процессора*/
  TIM_TimeBaseStructInit(&timer_base);
  timer_base.TIM_Prescaler = 48 - 1;
  TIM_TimeBaseInit(TIM3, &timer_base);

  /* Enable the TIM3 global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
  TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
  TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
  TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
  TIM_ICInitStructure.TIM_ICFilter = 0x2;

  TIM_PWMIConfig(TIM3, &TIM_ICInitStructure);

  /* Select the TIM2 Input Trigger: TI2FP2 */
  TIM_SelectInputTrigger(TIM3, TIM_TS_TI2FP2);

  /* Select the slave Mode: Reset Mode */
  TIM_SelectSlaveMode(TIM3, TIM_SlaveMode_Reset);
  TIM_SelectMasterSlaveMode(TIM3,TIM_MasterSlaveMode_Enable);

  /* TIM enable counter */
  TIM_Cmd(TIM3, ENABLE);

  /* Enable the CC2 Interrupt Request */
  TIM_ITConfig(TIM3, TIM_IT_CC2, ENABLE);
}
