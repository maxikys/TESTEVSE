// ***********************************************************
//	systick.h
// 	Системный таймер
//
// ***********************************************************

#include <stdint.h>
#include <stdbool.h>

#ifndef _SYSTICK_H
#define _SYSTICK_H

    // Запуск таймера с заданной частотой
    void SystickInit(uint32_t Frequency);
    
	// Добавить функцию в список вызова, Handler будет вызываться с заданной частотой
	// Аргументы: 2
	//  Frequency - частота вызова (при увеличении снижается точность)
	//  Handler - функция, которая будет вызываться
	// Результат: нет
	void SystickAddFunction(uint16_t Frequency, void * Handler);
	
    // Остановить таймер
    void SystickStop(void * Handler);
	
    // Включить таймер
    void SystickResume(void * Handler);

    // Изменить частоту таймера
    void SystickChangeFrequency(void * Handler, uint16_t Frequency);
    
    // Главный цикл. В нём вызываются обработчики таймера
   void SystickScanEvent(void);

#endif
