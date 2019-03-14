// ***********************************************************
//	clock.h
// 	Информация о тактировании
//
// ***********************************************************

#include <stdint.h>
#include <stdbool.h>
#include "stm32f0xx.h"


#ifndef _CLOCK_H
#define _CLOCK_H

typedef enum { HSEoff = 0, HSEon = 1, HSEext = 2} TconfHSE;
typedef enum { HSI = 0, HSE = 1, PLL } Tsource;
typedef enum { PLLx1 = 0,PLLx2, PLLx3, PLLx4, PLLx5, PLLx6, PLLx7, PLLx8, PLLx9, PLLx10, PLLx11, PLLx12, PLLx13, PLLx14, PLLx15, PLLx16} TPLL;
typedef enum { BeforeDiv1 = 2, BeforeDiv2 = 3 } TBeforeDiv;
typedef enum { NoReady = 0, Ready = 1} TStatus;



typedef enum {
                AHBDIV1 = 0,
                AHBDIV2 = 0x00000080,
                AHBDIV4 = 0x00000090,
                AHBDIV8 = 0x000000A0,
                AHBDIV16 = 0x000000B0,
                AHBDIV64 = 0x000000C0,
                AHBDIV128 = 0x000000D0,
                AHBDIV256 = 0x000000E0,
                AHBDIV512 = 0x000000F0,
             }TAHBPrescaler;

typedef enum {
                APBDIV1 = 0,
                APBDIV2 = 0x00000400,
                APBDIV4 = 0x00000500,
                APBDIV8 = 0x00000600,
                APBDIV16 = 0x00000700,
             }TAPBPrescaler;             


typedef struct
{
   uint32_t ValueClock;
   uint32_t ValueCore;
   uint8_t  Mult;
    
}TStructClock;


    // Инициализация
    void ClockInit(Tsource source, uint32_t ClockValue, uint32_t CoreClock );
    
    // Частота работы ядра
    uint32_t clock_GetSYS(void);

    // Частота шины APB1
    uint32_t clock_GetAPB(void);

    // Частота шины APB2
    uint32_t clock_GetAPB2(void);

#endif
