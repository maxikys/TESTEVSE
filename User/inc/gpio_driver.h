#ifndef _GPIO_DRIVER
#define _GPIO_DRIVER

#include <stdint.h>
#include <stdbool.h>
#include "stm32f0xx.h"
#include "stm32f0xx_rcc.h"
#include "stm32f0xx_gpio.h"

//    typedef enum { PA, PB, PC, PD, PE, PF, PG, PH, PI } TPort;

    typedef enum {_2MHz = 0x02,_10MHz = 0x01,_50MHz = 0x03} SPEED;// скорость
    typedef enum {IN = 0,OUT = 1,AF = 2,AN = 3} MODE;             // тип ножки: ВХОД,ВЫХОД,АЛЬТЕРНАТИВНАЯ ФУНКЦИЯ, АНАЛОГОВЫЙ ВХОД/ВЫХОД
    typedef enum {NOPULL = 0,PULLUP = 1,  PULLDOWN = 2}PULLUPDOWN; // подтяжка
    typedef enum {PP,OD}PP_OD;// ПУШПУЛ, ОТКРЫТОЫЙ КОЛЕКТОР
    typedef enum {AF0,AF1,AF2,AF3,AF4,AF5,AF6,AF7,AF8,AF9,AF10,AF11,AF12,AF13,AF14,AF15}ALTFUNC;// АЛЬТЕРНАТИВНЫЕ ФУНКЦИИ
  
	
	
	
	
    typedef struct
    {
        GPIO_TypeDef *   Port;
        uint8_t Pin;
    } TPin;
    
    /* Установка значения на ножке, в зависимости от маски */
    void SetPinValue(const TPin * PtrStructPin, uint16_t Value);
    
    // инициализация
    void TalkPort (const TPin *, FunctionalState);
    
    // настройка режима OUT, IN, AF, AN
    void PinMode(const TPin * PtrStructPin,MODE mode);

    // цифровой вход
    void gpio_DigitalInput(const TPin * Pin);

    // установить высокий уровень 
    void output_hight(const TPin * Pin);

    // установить низкий уровень
    void output_low(const TPin * Pin);
    
    // инверсия текущего состояния вывода
    void TogglePin(const TPin * PtrStructPin);

    // произвольное значение на ножке
    bool gpio_Value(const TPin * Pin);

    // выход с открытым коллектором Open Drain ()
    void PinOpenDrain(const TPin * PtrStructPopt);
	
	// выход пушпул
	void PinPushPull(const TPin * PtrStructPopt);
    
    //подтяжка вверх
    void PinPullUp(const TPin * PtrStructPin);
    
     //подтяжка вниз
    void PinPullDown(const TPin * PtrStructPin);
    
     //без подтяжки 
    void PinNoPUPD(const TPin * PtrStructPin);
    
    // Установка скорости порта
    void SetSpeedPin(const TPin *,SPEED);
       
    // назначить альтернотивную функцию (0-15)
     void PinAltFunc(const TPin * PtrStructPin, ALTFUNC altfunc);
     
     // Функция записи значения в виртуальный порт
     
    void WriteToVirtualPort(const TPin* VPort[], uint8_t size,uint16_t Value);
     
     //ножка либо пушпул либо открытый коллектор
     void PinPPorOD(const TPin * PtrStructPopt, PP_OD pp_od);
     
     // ножка либо подтфнута ВВЕРХ, либо ВНИЗ, либо БЕЗПОДТЯЖКИ
     void PinPUPD(const TPin * PtrStructPin, PULLUPDOWN pulluppd);
     
     // конфигирирование виртульного порта через массив
     // в данной фунции тактирование включается по тем портам,из которых состоит виртуальная функция
    void ConfigVirualPort(const TPin* VPort[], uint8_t size, MODE mode,SPEED speed,PULLUPDOWN PUPD,PP_OD pp_od);
     
     // чтение состояния на ножке
     bool Pin_Value(const TPin * PtrStructPin);

     // Настроить режим виртуального порта
     void PinModeVirtualPort(const TPin* VPort[], uint8_t size, MODE mode);
    
    /* конфигурирование портов заранее не известных использовать в структурной инициализации*/
    void ConfigVirualPortDinamic(TPin* VPort[], uint8_t size, MODE mode,SPEED speed,PULLUPDOWN PUPD,PP_OD pp_od);

#endif


