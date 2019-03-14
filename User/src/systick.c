// ***********************************************************
//	systick.с
// 	Системный таймер
//
// ***********************************************************

#include "systick.h"
#include "clock.h"
#include <stm32f0xx.h>
#include <string.h>

// Количество обработчиков максимальное
#define TIMER_HANDLERS    15

typedef struct 
{
	void  (* Handler)(void);
	uint16_t Countdown;
	uint16_t Reload;
    
    // Запущено
    bool     Run;
    // Сработало
    bool     Fired;
} THandlers;

// Функции, кои будут вызываться
static THandlers Handlers[TIMER_HANDLERS];
static uint32_t  TimerFrequency;
static int       TimerCount;

void SysTick_Handler(void)
{
    int i;
    
	for(i = 0; i < TIMER_HANDLERS; i++)
	{
		if(Handlers[i].Run)
		{
			if(--Handlers[i].Countdown == 0)
			{
				Handlers[i].Countdown = Handlers[i].Reload;
		
				// Запишем, что сработало
				Handlers[i].Fired = true;
			}
		}
	}
}

// Запуск таймера с заданной частотой
void SystickInit(uint32_t Frequency)
{
   __enable_irq();//разрешаем глобальные прерывания 
    if(!Frequency) return;
    
    // Сбросим параметры
    TimerCount = 0;
    TimerFrequency = Frequency;
    memset(&Handlers[0], 0, sizeof(Handlers));
    
    // Настройка частоты
    SysTick_Config(clock_GetSYS() / Frequency);
    
    // Разрешение прерывания
    NVIC_EnableIRQ(SysTick_IRQn);
}


// Добавить функцию в список вызова, Handler будет вызываться с заданной частотой
    void SystickAddFunction(uint16_t Frequency, void * Handler)
    {
    int i;

    for(i = 0; i < TIMER_HANDLERS; i++)
    {
        // Найдём пустой слот
        if(!Handlers[i].Handler)
        {
            // Обработчик, частота опроса
            Handlers[i].Run       =  true;
            Handlers[i].Fired     =  false;
            Handlers[i].Handler   = (void (*)(void))Handler;
            Handlers[i].Reload    = (TimerFrequency / Frequency);
            Handlers[i].Countdown =  Handlers[i].Reload;
            
            TimerCount++;
            
            return;
        }
    }

while(1) {} // Нет пустых
}

// Изменить частоту таймера
    void SystickChangeFrequency(void * Handler, uint16_t Frequency)
    {
    int i;

    for(i = 0; i < TIMER_HANDLERS; i++)
    {
        if(Handlers[i].Handler == Handler)
        {
            Handlers[i].Reload = (TimerFrequency / Frequency);
            Handlers[i].Countdown = Handlers[i].Reload;
            break;
        }
    }
    }

// Включить таймер
void SystickResume(void * Handler)
{
    int i;
	
	for(i = 0; i < TIMER_HANDLERS; i++)
	{
		if(Handlers[i].Handler == (void(*)(void))Handler)
		{
            Handlers[i].Run = true;
            break;
        }
    }
}

// Остановить таймер
void SystickStop(void * Handler)
{
    int i;
	
	for(i = 0; i < TIMER_HANDLERS; i++)
	{
		if(Handlers[i].Handler == Handler)
		{
            Handlers[i].Run = false;
            Handlers[i].Fired =  false;
            break;
        }
    }
}

void SystickScanEvent(void)
{
    int i;
	
	for(i = 0; i < TIMER_HANDLERS; i++)
	{
        // Если сработало - вызовем.
        if(Handlers[i].Fired)
        {
            Handlers[i].Fired = false;
            Handlers[i].Handler();
        }
    }
}
