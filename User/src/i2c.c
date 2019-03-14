#include "i2c.h"

 const TPin     SCL =       {GPIOA,11};
 const TPin     SDA =       {GPIOA,12};
 const TPin*    VI2C[] =    {&SCL,&SDA};

const uint32_t timewoit = 5000;

/*
Выполняет первичную настройку шины.
Настраивает порты, альтернативные функции.
Настраивает тайминги для работы шины.
*/
void I2CInit (void)
{
    uint16_t count = 0;
	// Тактирование GPIO не включаю, т.к. оно уже включено в начале программы
	RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;		// Включаю тактирование I2C
//	// Настройка ног PB6, PB7
//	GPIOB->MODER |= GPIO_MODER_MODER6_1 | GPIO_MODER_MODER7_1;			// Режим альтернативной функции
//	GPIOB->OTYPER |= GPIO_OTYPER_OT_6 | GPIO_OTYPER_OT_7;				// Открытый коллектор
//	GPIOB->OSPEEDR |= GPIO_OSPEEDR_OSPEEDR6 | GPIO_OSPEEDR_OSPEEDR7;	// Максимальная скорость
//	// Выбор альтернативной функции
//	GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_1);	// I2C1_SCL
//	GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_1);	// I2C1_SDA
    
    ConfigVirualPort(VI2C,2,AF,_50MHz,NOPULL,OD);
    PinAltFunc(&SCL,AF1);
    PinAltFunc(&SDA,AF1);
	
	I2C_BUS->CR1 &= ~I2C_CR1_PE;			// Отключаю I2C
    count = 0;
	while ((I2C_BUS->CR1 & I2C_CR1_PE) && (count != timewoit )) {count++;};	// Жду пока отключится
	
	// Частота тактирования модуля I2C = 48 МГц
	// Частота шины I2C = 100 kHz
	// Настраиваю тайминги
	I2C_BUS->TIMINGR = 0x10805E89; /* Считаю утилитой: 48Мгц - частота APB, скорость 100 кБит, аналоговый фильтр - вкл. */
	
	
	I2C_BUS->CR1 |= I2C_CR1_PE;					// Включаю I2C
    count = 0;
	while (((I2C_BUS->CR1 & I2C_CR1_PE) == 0) && (count != timewoit )) {count++;};	// Жду пока включится
}


/*
Это служебная функция, использовать её не нужно.
Устанавливает направление данных - приём или передача.
Задаёт объём пересылаемых данных.
Задаёт адрес ведомого устройства.
Выдаёт старт на шину.
Параметры:
Direction - направление (0-отправка, 1-приём)
Adress - адрес ведомого устройства
Size - размер данных (от 1 до 255 байт)
*/
void I2C_Start_Direction_Adress_Size (I2C_Direction Direction, uint8_t Adress, uint8_t Size)
{
     uint16_t count = 0;
	//I2C_BUS->CR2 &= ~I2C_CR2_AUTOEND;				// Выдавать стоп вручную
	//I2C_BUS->CR2 &= ~I2C_CR2_RELOAD;				// Не использовать режим перезагрузки
	if (Direction) I2C_BUS->CR2 |= I2C_CR2_RD_WRN;	// Режим приёма
	else I2C_BUS->CR2 &= ~I2C_CR2_RD_WRN;			// Режим передачи
	I2C_BUS->CR2 &= ~I2C_CR2_NBYTES;				// Очистить размер данных
	I2C_BUS->CR2 |= Size<<I2C_OFFSET_CR2_NBYTES;	// Установить размер данных
	I2C_BUS->CR2 &= ~I2C_CR2_SADD;	// Очистить адрес ведомого устройства
	I2C_BUS->CR2 |= Adress;			// Установить адрес ведомого устройства
	I2C_BUS->CR2 |= I2C_CR2_START;					// Выдать старт на шину
    count = 0;
	while (((I2C_BUS->ISR & I2C_ISR_BUSY) == 0) && (count != timewoit)) {count++;};	// Ожидать выдачу старта
}


/*
Это служебная функция, использовать её не нужно.
Выдаёт стоп на шину.
Очищает флаги.
Проверяет наличие ошибок, очищает флаги ошибок.
*/
void I2C_Stop (void)
{  
    uint16_t count = 0;
	I2C_BUS->CR2 |= I2C_CR2_STOP;				// Выдать стоп на шину
	while ((I2C_BUS->ISR & I2C_ISR_BUSY) && (count != timewoit)) {count++;};		// Ожидать выдачу стопа
	// Очищаю флаги - необходимо для дальнейшей работы шины
	I2C_BUS->ICR |= I2C_ICR_STOPCF;		// STOP флаг
	I2C_BUS->ICR |= I2C_ICR_NACKCF;		// NACK флаг
	// Если есть ошибки на шине - очищаю флаги
	if (I2C_BUS->ISR & (I2C_ISR_ARLO | I2C_ISR_BERR))
	{
		I2C_BUS->ICR |= I2C_ICR_ARLOCF;
		I2C_BUS->ICR |= I2C_ICR_BERRCF;
	}
}


/*
Выполняет транзакцию записи Size байт в регистр Register по адресу Adress.
Параметры:
Adress - адрес ведомого устройства
Register - регистр, в который хотим передать данные
Data - указывает откуда брать данные для передачи
Size - сколько байт хотим передать (от 1 до 254)
Возвращает:
1 - если данные успешно переданы
0 - если произошла ошибка
*/
uint8_t I2C_Write_Transaction (uint8_t Adress, uint8_t Register, uint8_t *Data, uint8_t Size)
{
	uint8_t Count=0;	// Счётчик успешно переданных байт
    uint16_t count = 0;
	// Старт
	I2C_Start_Direction_Adress_Size (I2C_Transmitter, Adress, 1+Size);
	// Сейчас либо I2C запросит первый байт для отправки,
	// Либо взлетит NACK-флаг, говорящий о том, что микросхема не отвечает.
	// Если взлетит NACK-флаг, отправку прекращаем.
	while ((((I2C_BUS->ISR & I2C_ISR_TXIS)==0) && ((I2C_BUS->ISR & I2C_ISR_NACKF)==0)) && (I2C_BUS->ISR & I2C_ISR_BUSY) && (count != timewoit)) {count++;};
	if (I2C_BUS->ISR & I2C_ISR_TXIS) I2C_BUS->TXDR=Register;	// Отправляю адрес регистра
	
	// Отправляем байты до тех пор, пока не взлетит TC-флаг.
	// Если взлетит NACK-флаг, отправку прекращаем.
    
	while ((((I2C_BUS->ISR & I2C_ISR_TC) == 0) && ((I2C_BUS->ISR & I2C_ISR_NACKF)==0)) && (I2C_BUS->ISR & I2C_ISR_BUSY))
	{
		if (I2C_BUS->ISR & I2C_ISR_TXIS) I2C_BUS->TXDR=*(Data+Count++);	// Отправляю данные
	}
	I2C_Stop();
	if (Count == Size) return 1; return 0;
}


/*
Выполняет транзакцию чтения Size байт из регистра Register по адресу Adress.
Параметры:
Adress - адрес ведомого устройства
Register - регистр, из которого хотим принять данные
Data - указывает куда складывать принятые данные
Size - сколько байт хотим принять (от 1 до 255)
Возвращает:
1 - если данные успешно приняты
0 - если произошла ошибка
*/
uint8_t I2C_Read_Transaction (uint8_t Adress, uint8_t Register, uint8_t *Data, uint8_t Size)
{
	uint8_t Count=0;	// Счётчик успешно принятых байт
	// Старт
	I2C_Start_Direction_Adress_Size (I2C_Transmitter, Adress, 1);
	// Сейчас либо I2C запросит первый байт для отправки,
	// Либо взлетит NACK-флаг, говорящий о том, что микросхема не отвечает.
	// Если взлетит NACK-флаг, отправку прекращаем.
	while ((((I2C_BUS->ISR & I2C_ISR_TC)==0) && ((I2C_BUS->ISR & I2C_ISR_NACKF)==0)) && (I2C_BUS->ISR & I2C_ISR_BUSY))
	{
		if (I2C_BUS->ISR & I2C_ISR_TXIS) I2C_BUS->TXDR = Register;	// Отправляю адрес регистра
	}
	// Повторный старт
	I2C_Start_Direction_Adress_Size (I2C_Receiver, Adress, Size);
	// Принимаем байты до тех пор, пока не взлетит TC-флаг.
	// Если взлетит NACK-флаг, приём прекращаем.
	while ((((I2C_BUS->ISR & I2C_ISR_TC)==0) && ((I2C_BUS->ISR & I2C_ISR_NACKF)==0)) && (I2C_BUS->ISR & I2C_ISR_BUSY))
	{
		if (I2C_BUS->ISR & I2C_ISR_RXNE) *(Data+Count++) = I2C_BUS->RXDR;	// Принимаю данные
	}
	I2C_Stop();
	if (Count == Size) return 1; return 0;
}

