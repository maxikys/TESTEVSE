#include "pwm.h"
#include "gpio_driver.h"
#include "clock.h"
#include "stm32f0xx.h"

// TODO: добавить включение вывода для TIM1/TIM8!

// Разрешение: 15 бит
#define Count  1000

// Частота ШИМ: 1000 Гц
#define FREQUENCY   1000


 static uint16_t CalculationStartValue(void);
//float GammaCorrection(uint32_t TimetMaxCount, uint32_t SetValue);
// Описание канала вывода таймера
typedef struct
{
    TIM_TypeDef *       TIM;                 // Таймер
    int                 Channel;             // Канал таймера
    bool                Advanced;            // С поддержкой управления двигателем? (требует дополнительных телодвижений)
    
    uint32_t            RCCRegMask;          // Маска для управления тактированием
    volatile uint32_t * RCCReg;              // Адрес регистра управления тактированием
    
    ALTFUNC             AlternateFunction;   // Номер альтернативной функции
    TPin                Pin;                 // Вывод
    AktiveState         State;               // Полярность шима
    
} TChannel;

  typedef struct
  {
    uint16_t            StartValue;
  }TStartValue;
  
 static TStartValue StartValue;
// Таблица с настройками ШИМ (можно вписать иные конфигурации с альтернативным маппингом
static const TChannel Channels[] = {
                                    
                                    
                                    {  TIM3,  4, false,  RCC_APB1ENR_TIM3EN,  &RCC->APB1ENR, AF1, {GPIOB,  1},  AktiveLow},  // PB1  - TIM3_CH4
                                    {  TIM3,  3, false,  RCC_APB1ENR_TIM3EN,  &RCC->APB1ENR, AF1, {GPIOB,  0},  AktiveHigh},  // PB2  - TIM3_CH3
                                    
                                    
                                   };

// количество каналов
static const int ChannelCount = sizeof(Channels) / sizeof(Channels[0]);

// Инициализировать таймер под ШИМ
static void pwm_InitTimer(const TChannel * Channel)
{
    
    uint32_t Clock = clock_GetAPB(); // Частота шины
    //uint32_t Counts = (1UL << RESOLUTION);
    
    // Включить тактирование модуля
    *Channel->RCCReg |= Channel->RCCRegMask;
    
    // Разрядность: 16 бит
    Channel->TIM->ARR = Count - 1;
    
    // Предделитель:
    Channel->TIM->PSC = (Clock / FREQUENCY / Count) - 1;
    
    // Включить счёт
    Channel->TIM->CR1 |= TIM_CR1_CEN;

    if(Channel->Advanced)
    {
        // Разрешить выход
        Channel->TIM->BDTR |= TIM_BDTR_MOE;
    }
    
    StartValue.StartValue =  CalculationStartValue();/*расчет стартового значения*/
}

// Установка значения регистра сравнения
static void pwm_SetValue(const TChannel * Channel, uint16_t Value)
{
    switch(Channel->Channel)
    {
    case 1: Channel->TIM->CCR1 = Value; break;
    case 2: Channel->TIM->CCR2 = Value; break;
    case 3: Channel->TIM->CCR3 = Value; break;
    case 4: Channel->TIM->CCR4 = Value; break;
    }
}

// Инициализация вывода ШИМ
static void pwm_InitChannel(const TChannel * Channel)
{
   /*Настройка ножки МК*/
       TalkPort(&Channel->Pin,ENABLE);
       PinMode(&Channel->Pin,AF);   
       SetSpeedPin(&Channel->Pin,_50MHz);   
       PinPUPD(&Channel->Pin,NOPULL);
       PinPPorOD(&Channel->Pin,PP);
       PinAltFunc(&Channel->Pin,Channel->AlternateFunction);
    // Выключить счёт на время настройки
    Channel->TIM->CR1 &= ~TIM_CR1_CEN;
    
    switch(Channel->Channel)
    {
    case 1:
        // Режим: ШИМ1 0b110
        Channel->TIM->CCMR1 &= ~TIM_CCMR1_OC1M;  // очистим поле режима
        Channel->TIM->CCMR1 |= (TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1); // Запишем 0b110
        if(Channel->State == AktiveHigh)Channel->TIM->CCER |= TIM_CCER_CC1P;
        else Channel->TIM->CCER &= ~TIM_CCER_CC1P;
        //Channel->TIM->CCER 
        
        // Канал 1 включить
        Channel->TIM->CCER |= TIM_CCER_CC1E;
        break;
    case 2:
        // Режим: ШИМ1 0b110
        Channel->TIM->CCMR1 &= ~TIM_CCMR1_OC2M;  // очистим поле режима
        Channel->TIM->CCMR1 |= (TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1); // Запишем 0b110
        if(Channel->State == AktiveHigh)Channel->TIM->CCER |= TIM_CCER_CC2P;
        else Channel->TIM->CCER &= ~TIM_CCER_CC2P;
        // Канал 2 включить
        Channel->TIM->CCER |= TIM_CCER_CC2E;
        break;
    case 3:
        // Режим: ШИМ1 0b110
        Channel->TIM->CCMR2 &= ~TIM_CCMR2_OC3M;  // очистим поле режима
        Channel->TIM->CCMR2 |= (TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1); // Запишем 0b110
        if(Channel->State == AktiveHigh)Channel->TIM->CCER |= TIM_CCER_CC3P;
        else Channel->TIM->CCER &= ~TIM_CCER_CC3P;
        // Канал 3 включить
        Channel->TIM->CCER |= TIM_CCER_CC3E;
        break;
    case 4:
        // Режим: ШИМ1 0b110
        Channel->TIM->CCMR2 &= ~TIM_CCMR2_OC4M;  // очистим поле режима
        Channel->TIM->CCMR2 |= (TIM_CCMR2_OC4M_2 | TIM_CCMR2_OC4M_1); // Запишем 0b110
        if(Channel->State == AktiveHigh)Channel->TIM->CCER |= TIM_CCER_CC4P;
        else Channel->TIM->CCER &= ~TIM_CCER_CC4P;
        // Канал 4 включить
        Channel->TIM->CCER |= TIM_CCER_CC4E;
        break;
    }
    
    // Обнулим вывод
    pwm_SetValue(Channel, 0);
    
    // Передадим сигнал о сбросе счётчика, чтоб все внутренние регистры точно обновились.
    Channel->TIM->EGR = TIM_EGR_UG;
    
    // Включить счёт
    Channel->TIM->CR1 |= TIM_CR1_CEN;
}

// Запись о канале
static const TChannel * pwm_GetChannel(int Channel)
{
    if (Channel >= ChannelCount) return 0;
    
    return &Channels[Channel];
}

// Настроить канал
void pwm_Init(int Channel)
{
    const TChannel * C = pwm_GetChannel(Channel);
    if(!C) return;
    
    // Настроим
    pwm_InitTimer(C);
    pwm_InitChannel(C);
}

/*Гамма корректировка 3-его порядка*/
float GammaCorrection(uint32_t TimetMaxCount, uint32_t SetValue)
{
      double f = ((double)SetValue/(float)TimetMaxCount);
      return f*f*f; // gamma = 3

}
/* Функция вычисления значения при использовании Гамма коррекции запись которого даст "1" */
static uint16_t CalculationStartValue(void)
{
   uint16_t i = 0, ScaledValue = 0,PwmValue = 0;
   float level = 0;
   
   
   for (i = 0; i < Count; i++)
   {
    ScaledValue = Count/100 * i;
    level = GammaCorrection(Count, ScaledValue);
    PwmValue = (uint16_t)(level*ScaledValue);  
    if(PwmValue>=1)return i;
   }
  return 1;
}


// Задать скважность  (0-100%)
void PwmSetPersent(int Channel, uint16_t Value)
{
    // Приведём к нужному разрешению
    uint16_t ScaledValue = 0, PwmValue = 0;
    float level = 0;
   
    const TChannel * C = pwm_GetChannel(Channel);
    if(!C) return;
    
    if(Value > 100) return;
    
    ScaledValue = Count/100 * (Value + StartValue.StartValue - 1);
    if(Value + StartValue.StartValue > 100) 
    {
       pwm_SetValue(C,Count - 1);
    }
    level = GammaCorrection(Count, ScaledValue);
    PwmValue = (uint16_t)(level*ScaledValue);
    // Установим величину
    pwm_SetValue(C,PwmValue);
}



// Задать скважность (0-0xFFFF)
void pwm_Set(int Channel, uint16_t Value)
{
    // Приведём к нужному разрешению
    uint16_t ScaledValue = Value;
    
    const TChannel * C = pwm_GetChannel(Channel);
    if(!C) return;
    
    // Установим величину
    pwm_SetValue(C, ScaledValue);
}
