
#ifndef I2C_H	// Блокирую повторное включение этого модуля
#define I2C_H

//#include "main.h"
#include "stm32f0xx.h"
#include "gpio_driver.h"
#include "clock.h"

#define I2C_BUS I2C1


// Вспомогательные переменные
typedef enum _I2C_Direction {I2C_Transmitter=0, I2C_Receiver=1} I2C_Direction;
/*
Битовые смещения для настройки разных регистров
Чтобы не использовать в коде магические числа, и код был читаемым
Например:
I2C->CR2 |= 1<<I2C_OFFSET_CR2_NBYTES;	// Здесь понятно, что передаётся 1 байт
I2C->CR2 |= 1<<16;						// Здесь возникает путаница, то ли 1 байт, то ли 16
*/
#define I2C_OFFSET_TIMINGR_SCLL		0
#define I2C_OFFSET_TIMINGR_SCLH		8
#define I2C_OFFSET_TIMINGR_SDADEL	16
#define I2C_OFFSET_TIMINGR_SCLDEL	20
#define I2C_OFFSET_TIMINGR_PRESC	28
#define I2C_OFFSET_CR2_NBYTES		16


////////////////////////////////////////////////
//    Секция прототипов локальных функций     //
////////////////////////////////////////////////
void I2CInit (void);
void I2C_Start (void);
void I2C_Stop (void);
uint8_t I2C_Write_Transaction (uint8_t Adress, uint8_t Register, uint8_t *Data, uint8_t Size);
uint8_t I2C_Read_Transaction (uint8_t Adress, uint8_t Register, uint8_t *Data, uint8_t Size);


#endif



