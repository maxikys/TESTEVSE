#include "keyboard.h"


bool flag1 = false;
extern bool flagIN;
bool generalflag = false;
uint8_t StateButton = 0;
//USB_CORE_HANDLE  USB_Device_dev ;

/* Секция определения указателей настройки и состояния клавиатуры*/

     TKeyboardParametrs       KeyboardParametrs = {_10ms,_80ms,3,3}; /* переменная  структуры для инициилизации параметров клавиатуры по умолчанию */
     TKeyStatus               KeyStatus;                          /* переменная структуры для хранения сосотояния  нажатых кнопок по умолчанию 3 кнопки одновременно*/
     eScanCode ScanCode;
/* Секция определения  */
 volatile uint8_t tempValue;
 volatile uint16_t ValuePwm = 500-1;

/* Секция определения ножек клавиатуры*/
const TPin Row0 = {GPIOA,11};								//0 строка     
const TPin Row1 = {GPIOA,12};								//1 строка
const TPin Row2 = {GPIOB,3}; 								//2 строка

const TPin Col0 = {GPIOB,4};								//0 столбец
const TPin Col1 = {GPIOB,5};								//1 столбец
const TPin Col2 = {GPIOB,6};								//2 столбец



 
/*Инициализация массива указателей для клавиатуры*/
const TPin* Row[]    = {&Row0,&Row1,&Row2};
const TPin* Column[] = {&Col0,&Col1,&Col2};

 
 /*Функция инициилизации клавиатуры*/
 void KeyboardInit(Tperiod TimeScan, Tperiod TimeDrebezg, uint8_t SRow,uint8_t SColumn)
     {  
          uint8_t i=0;
          KeyboardParametrs.period_oprosa = TimeScan;
          KeyboardParametrs.TimeDrebezg = TimeDrebezg;
          KeyboardParametrs.SizeRow = SRow;
          KeyboardParametrs.SizeColumn = SColumn;
          ConfigVirualPort(Column,SColumn,IN,_2MHz,PULLUP,PP); //конфигурирование сторбцов на вход
          ConfigVirualPort(Row,SRow,IN,_2MHz,NOPULL,PP);   //конфинурирование строк на вход
          
          KeyStatus.cntKeyPress = (uint8_t*) malloc(KeyboardParametrs.SizeColumn * KeyboardParametrs.SizeRow * sizeof(uint8_t));   /* Выделяем память для счетчиков */
          KeyStatus.FlagDetectPress = (bool*)malloc(KeyboardParametrs.SizeColumn * KeyboardParametrs.SizeRow * sizeof(bool)); /* Выделяем память для флагов нажатия кнопки*/
          KeyboardParametrs.ScanCodeBuff = (uint8_t*) malloc(KeyboardParametrs.SizeColumn * KeyboardParametrs.SizeRow * sizeof(uint8_t));/* Выделяем память для хранения тоблицы декодирования */
          KeyStatus.FlagDetectDePress = (bool*)malloc(KeyboardParametrs.SizeColumn * KeyboardParametrs.SizeRow * sizeof(bool)); /* Выделяем память для флагов отжатия конопки*/
         
          for(i = 0;i < KeyboardParametrs.SizeColumn * KeyboardParametrs.SizeRow; i++)/* обнуляем значения памяти выделенной под массив */
          {
           *(KeyStatus.cntKeyPress + i)           = 0;
           *(KeyStatus.FlagDetectPress  + i)      = false;/*в первоначальном состояни все  флаги нажатия кнопок сброшены*/
           *(KeyStatus.FlagDetectDePress  + i)    = true;/*в первоначальном состояни все  флаги отжатия кнопок сброшены*/
           *(KeyboardParametrs.ScanCodeBuff + i)  = 0x00;
            KeyStatus.flag1 = false;
            KeyStatus.CountInOneMoment = 0;
          }
           

           /*Инициализация масок для SPI*/
          
          /*Включение зеленого отключение белого*/
          *(KeyboardParametrs.ScanCodeBuff + 0) = 0x01;
          *(KeyboardParametrs.ScanCodeBuff + 1) = 0x02;
          *(KeyboardParametrs.ScanCodeBuff + 2) = 0x03;
          *(KeyboardParametrs.ScanCodeBuff + 3) = 0x04;
          *(KeyboardParametrs.ScanCodeBuff + 4) = 0x05;
          *(KeyboardParametrs.ScanCodeBuff + 5) = 0x06;
          *(KeyboardParametrs.ScanCodeBuff + 6) = 0x07;
          *(KeyboardParametrs.ScanCodeBuff + 7) = 0x08;
          *(KeyboardParametrs.ScanCodeBuff + 8) = 0x09;
     }


     
     
//  /*Функция проецирует в требуемый выходной формат согласно ТЗ*/  
 uint8_t DecodingScanCode(void)  
  {
     uint8_t i = 0, Temp = 0;
     
     for(i = 0; i < 8; i++)
     {
        if(KeyStatus.FlagDetectPress[i]) Temp = (Temp | (1 << i)) & 0x17; /* 0x17 маска для защиты от изменения битов которые необходимо модифицировать*/
     }
     
     if(KeyStatus.FlagDetectPress[3]){Temp|= (1<<6); Temp&= ~(1 << 3);}
     
     if(KeyStatus.FlagDetectPress[6]){Temp|= (1<<3); Temp&= ~(1 << 6);}
     
     if(KeyStatus.FlagDetectPress[5]){Temp|= (1<<7); Temp&= ~(1 << 5);}
     if(KeyStatus.FlagDetectPress[7]){Temp|= (1<<5); Temp&= ~(1 << 7);}
     //StateButton = Temp
     return Temp;
  }
    
/*Функция  сканирования клавиатуры */
 void KeyboardScan(void )
    {       
    static uint8_t i = 0,k = 0;

       volatile uint16_t del = 0;
        for(i = 0; i < KeyboardParametrs.SizeRow; i++)
        {
             
          PinModeVirtualPort(Row,KeyboardParametrs.SizeRow,IN);/*настраиваем весь виртуальный порт на вход*/   
          PinMode(Row[i],OUT);/*настраиваем i-ый вывод виртуального порта на выход*/
          output_low(Row[i]); /*устанавливаем на i-ом выводе виртуального порта  "0" */
          for(del = 0; del < 1000; del++){}
          for (k = 0; k < KeyboardParametrs.SizeColumn;k++)
           {
                if( !Pin_Value(Column[k])  )
                {   
                    if(KeyStatus.cntKeyPress[ k + i * KeyboardParametrs.SizeColumn ] < KeyboardParametrs.TimeDrebezg/KeyboardParametrs.period_oprosa )/*если счетчик меньше, то инкрементируем*/
                    {
                     KeyStatus.cntKeyPress[k + i * KeyboardParametrs.SizeColumn] ++; 
                    }
                    else /* иначе обрабатываем нажатие кнопки */
                    {
                       if(KeyStatus.CountInOneMoment < KeyboardParametrs.SizeRow * KeyboardParametrs.SizeColumn)
                       {
                        KeyStatus.CountInOneMoment++;
                          if(KeyStatus.CountInOneMoment <= 1)
                          {
                            KeyStatus.FlagDetectPress [k + i * KeyboardParametrs.SizeColumn] = true;
                          }
                       }
                    }
                }
                     
                else
                {
                     if(KeyStatus.cntKeyPress[k + i * KeyboardParametrs.SizeColumn] < KeyboardParametrs.TimeDrebezg/KeyboardParametrs.period_oprosa )
                     {
                      KeyStatus.cntKeyPress[k + i * KeyboardParametrs.SizeColumn] = 0; /* если небыло подряд 8 событий то обнуляем соответствующий счектчик */
                      KeyStatus.FlagDetectPress [k + i * KeyboardParametrs.SizeColumn] = false;
                      
                     }
                     
                     /* если кнопка отжата и при этом ранее было зафиксировано нажатие, то... */
                     if( KeyStatus.FlagDetectPress [k + i * KeyboardParametrs.SizeColumn] == true )
                     {
                      KeyStatus.FlagDetectPress [k + i * KeyboardParametrs.SizeColumn] = false;
                      KeyStatus.FlagDetectDePress [k + i * KeyboardParametrs.SizeColumn] = true;/*состояние после которого можно передовать "0" */
                      KeyStatus.CountInOneMoment = 0;
                      //KeyStatus.TempCount = 0;
                     }
                }
           }
               
        }
         //DecodingScanCode();/*отправляем данные на декодировку*/
          
       
    }

  
  

  

   


