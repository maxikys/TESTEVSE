#include "KeySingle.h"

      TKeySingleParametrs               KeySingleParametrs;
      TKeySingleStatus                  KeySingleStatus;
      extern TKeyStatus                 KeyStatus;
      extern TKeyboardParametrs         KeyboardParametrs;
      extern bool                       generalflag;
      uint8_t Send_Buffer[SizeMass_Send_Buffer];
      
      bool FB                          = false;
      bool FM                          = false;
      bool FR                          = false;
      bool FD                          = false;
      bool FEnterSet                   = false;
      extern TSemisegment              Semiseg;
      uint8_t cntKeyR                  = 0;
      uint8_t cntKeyB                  = 0;
      uint8_t cntKeyM                  = 0;
      
      uint8_t cntSettings              = 0;
      
      uint8_t SpeedFan                 = 80;
      uint8_t Temp                     = 0;
      uint16_t DelModeSet              = 0;
      
      extern Rset RSet1;
      extern Rset RSet2;
      extern Rset RSet3;
      extern Rset* R[];
      extern Rset* RMass[];
      extern uint8_t NumPor;
      
      uint8_t WSet1[4]= {0,0,0,0};
      uint8_t WSet2[4]= {0,0,0,0};
      uint8_t WSet3[4]= {0,0,0,0};
      uint8_t WriteDatatoFlash[12]     = {0,0,0,0,0,0,0,0,0,0,0,0};
      
      const uint32_t StartAdress = 0x08003BF0;/* Стартовый адрес для сохранения настроек */
      
      /* Секция определения ножек индивидуальных кнопок*/
      const TPin SB1                   = {GPIOB,2};								//Кнопка "Больше"
      const TPin SB2                   = {GPIOB,12};								//Кнопка "Режим"
      const TPin SB3                   = {GPIOB,10}; 								//Кнопка "Меньше"
      
      
      /*Инициализация массива указателей для индивидуальных кнопок*/
      const TPin* SingleKey[] = {&SB1,&SB2,&SB3};
  
 
   
   void KeySingleInit(Tperiod TimeScan, Tperiod TimeDrebezg, uint8_t CountKey,TActiveState ActiveState)
   {
      uint8_t i;
      KeySingleParametrs.period_oprosa    =    TimeScan;
      KeySingleParametrs.TimeDrebezg      =    TimeDrebezg;
      KeySingleParametrs.CountKey         =    CountKey;
      KeySingleParametrs.ScanCodeBuff     =   (uint8_t*) malloc(KeySingleParametrs.CountKey * sizeof(uint8_t));/* Выделяем память для хранения тоблицы декодирования */
      KeySingleStatus.cntKeyPress         =   (uint8_t*) malloc(KeySingleParametrs.CountKey * sizeof(uint8_t));   /* Выделяем память для счетчиков */
      KeySingleStatus.FlagDetectPress     =   (bool*)malloc(KeySingleParametrs.CountKey * sizeof(bool));      /* Выделяем память для флагов нажатия кнопки*/
      KeySingleStatus.FlagDetectDePress   =   (bool*)malloc(KeySingleParametrs.CountKey * sizeof(bool));      /* Выделяем память для флагов нажатия кнопки*/
      KeySingleParametrs.ActiveState      =    ActiveState;
      /*Если активный уровень "+", то подтяжка вниз*/
      if( KeySingleParametrs.ActiveState == ActivHight)
      {
       ConfigVirualPort(SingleKey,CountKey,IN,_2MHz,PULLDOWN,PP); //конфигурирование кнопок на вход, с подтяжкой вниз
      }
      /*Если активный уровень "-", то подтяжка вверх*/
      if( KeySingleParametrs.ActiveState == ActivLow)/*Если активный уровень, то подтяжка вниз*/
      {
       ConfigVirualPort(SingleKey,CountKey,IN,_2MHz,PULLUP,PP); //конфигурирование кнопок на вход, с подтяжкой вверх
      }
      
      /*Инициализируем начальными значениями*/
      for(i = 0; i < KeySingleParametrs.CountKey; i++)
      {
         KeySingleParametrs.ScanCodeBuff[i] = 0;
         KeySingleStatus.cntKeyPress[i] = 0;
         KeySingleStatus.FlagDetectPress[i]   = false;
         KeySingleStatus.FlagDetectDePress[i] = false;
      }
      
       /*Инициализация сканкодов*/
      
      KeySingleParametrs.ScanCodeBuff[0] = Key_T;
      KeySingleParametrs.ScanCodeBuff[1] = Key_F5;
      KeySingleParametrs.ScanCodeBuff[2] = Key_R;
      KeySingleParametrs.ScanCodeBuff[3] = Key_L;
      KeySingleParametrs.ScanCodeBuff[4] = Key_A;

   }
   
   
   void ConvVoltage(uint16_t Value)
{
   Semiseg.TempData[0] = (  Value / 1000);
   Semiseg.TempData[1] = ( (Value % 1000) / 100 );
   Semiseg.TempData[2] = ( (Value % 1000) % 100 / 10 );
   Semiseg.TempData[3] = ( (Value % 1000) % 100 % 10 / 1 );

   memset(Semiseg.Data.EnableDrop,false,Semiseg.SizeDigit);
   
   if(Value >= 1000 && Value < 10000)
   {
      memcpy(Semiseg.Data.Data,Semiseg.TempData,Semiseg.SizeDigit);
      Semiseg.Data.EnableDrop[1] = true;
   }
   else if(Value >= 100 && Value < 1000)
   {
      Semiseg.TempData[0] = 255;
      memcpy(Semiseg.Data.Data,Semiseg.TempData,Semiseg.SizeDigit);
      Semiseg.Data.EnableDrop[1] = true;
   }
   else if(Value >= 10 && Value < 100)
   {
      Semiseg.TempData[0] = 255;
      memcpy(Semiseg.Data.Data,Semiseg.TempData,Semiseg.SizeDigit);
      Semiseg.Data.EnableDrop[1] = true;
   }
 
}
/* процедура для бузера */
   void SetBuzzer(uint8_t value)
 {
    Temp = value;
 }
 

/* Основная функция обработки кнопок */ 
void DecodeSingleKey( void)
   {
      uint8_t i = 0;
      static uint8_t count0   = 0, count1    = 0;
      static uint8_t cntp1    = 0, cntp2     = 0,  cntp3 = 0;  /* счетчики для отображения надписи "ПОР.Х" */
      static uint8_t cntf1    = 0, cntf2     = 0,  cntf3 = 0;  /* счетчики для отображения надписи "FПP.Х" */
      static uint8_t cntOK    = 0;                             /* счетчик для отображения надписи "ЗАП." */
      static uint16_t Cnt = 0, Cnt1 = 0, Cnt2 = 0;                             /* счетчик для обновления периода обновления АЦП */
      
      
       KeySingleStatus.CountInOneMoment = 0;
       
       
      
       for(i = 0; i < KeySingleParametrs.CountKey; i++ )
      {
         if(KeySingleStatus.FlagDetectPress[i] == true) KeySingleStatus.CountInOneMoment++;/*cчитаем сколько на данный момент зажато клавиш*/
      }
      
      
       /* кнопка Режим */
      if(KeySingleStatus.FlagDetectPress[0] == true && KeySingleStatus.CountInOneMoment == 1 && FR == false )
      {
       FR = true;
       if(!FEnterSet)cntKeyR++;
       else cntSettings++;/* если вошли врежим программирования, то увеличиваем этот счетчик */
       SetBuzzer(10);
      }
      
      /* Защита от продолжительноого нажатия кнопки "Режим" */
      if(KeySingleStatus.FlagDetectDePress[0] == true && KeySingleStatus.CountInOneMoment == 1 && FR == true )
      {
         FR = false;
         KeySingleStatus.FlagDetectDePress [0] = false;
      }
      if( cntKeyR == 0 && !FEnterSet )/* Вывод надписи "НАПР." */
      {
         if(count0 == 0)       WriteString("   Н");
         if(count0 == 20)      WriteString("  НA");
         if(count0 == 40)      WriteString(" НАП");
         if(count0 == 60)      WriteString("НАПР.");
         if(count0 == 80)      WriteString("НАПР.");
         if(count0 == 100)     WriteString("    ");
         if(count0 == 120)     WriteString("НАПР.");
         if(count0 == 140)     WriteString("    ");
         if(count0 == 160)     WriteString("НАПР.");
         
         if( count0 <= 180 )  count0++;
         if( count0 == 181 )
          { 
            if( NumPor == 0 )/* если порог не сработал */
            {
            Cnt2 = 0;
            Cnt1 ++;
             if(Cnt1 == 50)/* Период получения данных, влияет на скорость обновления информации */
             {
               Cnt1 = 0;
               GetAdcValue();/* преобразование данных от АЦП */
             }
            }
            else/* если сработал порог */
            {
               Cnt2 ++;
               Cnt1 = 0;
               if(Cnt2 == 100)
               {
                  if( NumPor == 1 ){WriteString("ПОР.1");}
                  if( NumPor == 2 )WriteString("ПОР.2");
                  if( NumPor == 3 )WriteString("ПОР.3");
                  //SetBuzzer(25);
               }
               if(Cnt2 == 200)GetAdcValue();/* преобразование данных от АЦП */  
 
               if(Cnt2 == 300)Cnt2 = 0;
            }
             
          }
      }
      
      if(cntKeyR == 1 && !FEnterSet)/* Вывод надписи "FAn. " */
      {
         
         if(count1 == 0)       WriteString("   F");
         if(count1 == 20)      WriteString("  FA");
         if(count1 == 40)      WriteString(" FAn.");
         if(count1 == 60)      WriteString("FAn. ");
         if(count1 == 80)      WriteString("FAn. ");
         if(count1 == 100)     WriteString("    ");
         if(count1 == 120)     WriteString("FAn. ");
         if(count1 == 140)     WriteString("    ");
         if(count1 == 160)     WriteString("FAn. ");
          
         if( count1 <= 180 )  count1++;
         
         if(count1 == 181) /* Вывод скорости вентилятора */
         { 
           Semiseg.Data.EnableDrop[2] = false;// гасим точку, ставшуюся от "FAn."
           Semiseg.Data.Data[0] = 'F';
           Semiseg.TempData[1] = SpeedFan / 100;
           if(Semiseg.TempData[1] < 1)Semiseg.Data.Data[1] = 255;
           else Semiseg.Data.Data[1] = Semiseg.TempData[1];
           Semiseg.Data.Data[2] = (SpeedFan % 100) / 10;
           Semiseg.Data.Data[3] = (SpeedFan % 100) % 10 / 1;
            
         }
      }
      if(cntKeyR == 2 && !FEnterSet)//Сбрасываем счетчики
      {
         cntKeyR = 0;
         count0 = 0;
         count1 = 0;
      }
      

      
      /* кнопка "Больше" */
      if(KeySingleStatus.FlagDetectPress[2] == true && KeySingleStatus.CountInOneMoment == 1 && FB == false )
      {
       FB = true;
         SetBuzzer(10);
         if( cntKeyR == 0 && !FEnterSet ) /* увеличение яркости семисегментника */
         {
         if(Semiseg.Brightness < 3 )Semiseg.Brightness ++; 
         }
         
         if( cntKeyR == 1 && !FEnterSet )/* увеличение скорости вентилятора */
         {
           if(SpeedFan <= 95) SpeedFan = SpeedFan + 5;
           pwm_Set(2,SpeedFan * 10 - 1 );
         }
/*-----------------------*/
         /* Первый пресет */
         if( cntSettings == 1 && FEnterSet )/* увеличиваем "ПОР.1" */
         {
          if(RSet1.Voltage < 2400)RSet1.Voltage = RSet1.Voltage + 5;
         }
         if( cntSettings == 2 && FEnterSet )/* увеличиваем "FПP.1" */
         {
          if(RSet1.FanValue < 100)RSet1.FanValue = RSet1.FanValue + 5;
         }
/*-----------------------*/
         /* Второй пресет */
         if( cntSettings == 3 && FEnterSet )/* увеличиваем "ПОР.2" */
         {
          if(RSet2.Voltage < 2400)RSet2.Voltage = RSet2.Voltage + 5;
         }
         if( cntSettings == 4 && FEnterSet )/* увеличиваем "FПP.2" */
         {
          if(RSet2.FanValue < 100)RSet2.FanValue = RSet2.FanValue + 5;
         }
/*-----------------------*/
         /* Третий пресет */
         if( cntSettings == 5 && FEnterSet )/* увеличиваем "ПОР.3" */
         {
          if(RSet3.Voltage < 2400)RSet3.Voltage = RSet3.Voltage + 5;
         }
         if( cntSettings == 6 && FEnterSet )/* увеличиваем "FПP.32" */
         {
          if(RSet3.FanValue < 100)RSet3.FanValue = RSet3.FanValue + 5;
         }
         

      }
      /* Защита от продолжительноого нажатия кнопки "Больше" */
      if(KeySingleStatus.FlagDetectDePress[2] == true && KeySingleStatus.CountInOneMoment == 1 && FB == true )
      {
         FB = false;
         KeySingleStatus.FlagDetectDePress [2] = false;
      }
      
     /* кнопка "Меньше" */ 
      if(KeySingleStatus.FlagDetectPress[1] == true && KeySingleStatus.CountInOneMoment == 1 && FM == false )
      {
         FM = true;
         SetBuzzer(10);
         if( cntKeyR == 0 && !FEnterSet ) /* уменьшение яркости семисегментника */
         {
            if(Semiseg.Brightness > 1 )Semiseg.Brightness --; 
         }
         
       if(cntKeyR == 1 && !FEnterSet)/* уменьшение скорости вентилятора */
         {
           if(SpeedFan >= 10) SpeedFan = SpeedFan - 5;
           pwm_Set(2,SpeedFan * 10 - 1 );
         }
/*-----------------------*/
         /* Первый пресет */
       if( cntSettings == 1 && FEnterSet )/* уменьшаем "ПОР.1" */
         {
          if(RSet1.Voltage > 800)RSet1.Voltage = RSet1.Voltage - 5;
         }
         if( cntSettings == 2 && FEnterSet )/* уменьшаем "FПP.1" */
         {
          if(RSet1.FanValue >= 5)RSet1.FanValue = RSet1.FanValue - 5;
         }
         
/*-----------------------*/
         /* Второй пресет */
       if( cntSettings == 3 && FEnterSet )/* уменьшаем "ПОР.2" */
         {
          if(RSet2.Voltage > 800)RSet2.Voltage = RSet2.Voltage - 5;
         }
         if( cntSettings == 4 && FEnterSet )/* уменьшаем "FПP.2" */
         {
          if(RSet2.FanValue >= 5)RSet2.FanValue = RSet2.FanValue - 5;
         }
/*-----------------------*/
         /* Третий пресет */
       if( cntSettings == 5 && FEnterSet )/* уменьшаем "ПОР.3" */
         {
          if(RSet3.Voltage > 800)RSet3.Voltage = RSet3.Voltage - 5;
         }
         if( cntSettings == 6 && FEnterSet )/* уменьшаем "FПP.3" */
         {
          if(RSet3.FanValue >= 5)RSet3.FanValue = RSet3.FanValue - 5;
         }
         
         
      }
      /* Защита от продолжительноого нажатия кнопки "Меньше" */
      if(KeySingleStatus.FlagDetectDePress[1] == true && KeySingleStatus.CountInOneMoment == 1 && FM == true )
      {
         FM = false;
         KeySingleStatus.FlagDetectDePress [1] = false;
      }
      
   /* Обработка входа в режим программирования */
   if( cntKeyR == 0 )
   {
      if(KeySingleStatus.FlagDetectPress[2] && KeySingleStatus.FlagDetectPress[1] && !FD )/* Вход в режим программирования порогов  */
      {
      //FD = true;
         if( DelModeSet <  200 )DelModeSet ++;
         if( DelModeSet == 200 )
         {
         FD          = true;
         FEnterSet   = true;
         SetBuzzer(50);
         cntSettings = 1;
         cntOK = 0;
         }
      }
 /*-----------------------------------------------------------*/
      /* Отображение пресета 1 */      
      if(cntSettings == 1) /* Отображение "ПОР.1" */
      {
         
         if(cntp1 == 0)       WriteString("   П");
         if(cntp1 == 20)      WriteString("  ПО");
         if(cntp1 == 40)      WriteString(" ПОР");
         if(cntp1 == 60)      WriteString("ПОР.1");
         if(cntp1 == 80)      WriteString("ПОР.1");
         if(cntp1 == 100)     WriteString("    ");
         if(cntp1 == 120)     WriteString("ПОР.1");
         if(cntp1 == 140)     WriteString("    ");
         if(cntp1 == 160)     WriteString("ПОР.1");
         
         if( cntp1 <= 180 )  cntp1++;
         if( cntp1 == 181 )
          {
           ConvVoltage(RSet1.Voltage);
          }
      }
      if(cntSettings == 2)/* Отображение "FПP.1" */ 
      {
         
         if(cntf1 == 0)       WriteString("   F");
         if(cntf1 == 20)      WriteString("  FП");
         if(cntf1 == 40)      WriteString(" FПР.");
         if(cntf1 == 60)      WriteString("FПР.1");
         if(cntf1 == 80)      WriteString("FПР.1");
         if(cntf1 == 100)     WriteString("    ");
         if(cntf1 == 120)     WriteString("FПР.1");
         if(cntf1 == 140)     WriteString("    ");
         if(cntf1 == 160)     WriteString("FПР.1");
         
         if( cntf1 <= 180 )  cntf1++;
         if( cntf1 == 181 )
          {
           Semiseg.Data.EnableDrop[2] = false;// гасим точку, ставшуюся от "FПР.1"
           Semiseg.Data.Data[0] = 'F';
           Semiseg.TempData[1] = RSet1.FanValue / 100;
           if(Semiseg.TempData[1] < 1)Semiseg.Data.Data[1] = 255;
           else Semiseg.Data.Data[1] = Semiseg.TempData[1];
           Semiseg.Data.Data[2] = (RSet1.FanValue % 100) / 10;
           Semiseg.Data.Data[3] = (RSet1.FanValue % 100) % 10 / 1;
          }
      }
/*-----------------------------------------------------------*/      
      /* Отображение пресета 2 */      
      if(cntSettings == 3) /* Отображение "ПОР.2" */
      {
         
         if(cntp2 == 0)       WriteString("   П");
         if(cntp2 == 20)      WriteString("  ПО");
         if(cntp2 == 40)      WriteString(" ПОР");
         if(cntp2 == 60)      WriteString("ПОР.2");
         if(cntp2 == 80)      WriteString("ПОР.2");
         if(cntp2 == 100)     WriteString("    ");
         if(cntp2 == 120)     WriteString("ПОР.2");
         if(cntp2 == 140)     WriteString("    ");
         if(cntp2 == 160)     WriteString("ПОР.2");
         
         if( cntp2 <= 180 )  cntp2++;
         if( cntp2 == 181 )
          {
           ConvVoltage(RSet2.Voltage);
          }
      }
      if(cntSettings == 4)/* Отображение "FПP.2" */ 
      {
         
         if(cntf2 == 0)       WriteString("   F");
         if(cntf2 == 20)      WriteString("  FП");
         if(cntf2 == 40)      WriteString(" FПР.");
         if(cntf2 == 60)      WriteString("FПР.2");
         if(cntf2 == 80)      WriteString("FПР.2");
         if(cntf2 == 100)     WriteString("    ");
         if(cntf2 == 120)     WriteString("FПР.2");
         if(cntf2 == 140)     WriteString("    ");
         if(cntf2 == 160)     WriteString("FПР.2");
         
         if( cntf2 <= 180 )  cntf2++;
         if( cntf2 == 181 )
          {
           Semiseg.Data.EnableDrop[2] = false;// гасим точку, ставшуюся от "FПР.1"
           Semiseg.Data.Data[0] = 'F';
           Semiseg.TempData[1] = RSet2.FanValue / 100;
           if(Semiseg.TempData[1] < 1)Semiseg.Data.Data[1] = 255;
           else Semiseg.Data.Data[1] = Semiseg.TempData[1];
           Semiseg.Data.Data[2] = (RSet2.FanValue % 100) / 10;
           Semiseg.Data.Data[3] = (RSet2.FanValue % 100) % 10 / 1;
          }
      }
      /*-----------------------------------------------------------*/      
      /* Отображение пресета 3 */      
      if(cntSettings == 5) /* Отображение "ПОР.3" */
      {
         
         if(cntp3 == 0)       WriteString("   П");
         if(cntp3 == 20)      WriteString("  ПО");
         if(cntp3 == 40)      WriteString(" ПОР");
         if(cntp3 == 60)      WriteString("ПОР.3");
         if(cntp3 == 80)      WriteString("ПОР.3");
         if(cntp3 == 100)     WriteString("    ");
         if(cntp3 == 120)     WriteString("ПОР.3");
         if(cntp3 == 140)     WriteString("    ");
         if(cntp3 == 160)     WriteString("ПОР.3");
         
         if( cntp3 <= 180 )  cntp3++;
         if( cntp3 == 181 )
          {
           ConvVoltage(RSet3.Voltage);
          }
      }
      if(cntSettings == 6)/* Отображение "FПP.3" */ 
      {
         
         if(cntf3 == 0)       WriteString("   F");
         if(cntf3 == 20)      WriteString("  FП");
         if(cntf3 == 40)      WriteString(" FПР.");
         if(cntf3 == 60)      WriteString("FПР.3");
         if(cntf3 == 80)      WriteString("FПР.3");
         if(cntf3 == 100)     WriteString("    ");
         if(cntf3 == 120)     WriteString("FПР.3");
         if(cntf3 == 140)     WriteString("    ");
         if(cntf3 == 160)     WriteString("FПР.3");
         
         if( cntf3 <= 180 )  cntf3++;
         if( cntf3 == 181 )
          {
           Semiseg.Data.EnableDrop[2] = false;// гасим точку, ставшуюся от "FПР.1"
           Semiseg.Data.Data[0] = 'F';
           Semiseg.TempData[1] = RSet3.FanValue / 100;
           if(Semiseg.TempData[1] < 1)Semiseg.Data.Data[1] = 255;
           else Semiseg.Data.Data[1] = Semiseg.TempData[1];
           Semiseg.Data.Data[2] = (RSet3.FanValue % 100) / 10;
           Semiseg.Data.Data[3] = (RSet3.FanValue % 100) % 10 / 1;
          }
      }
      if(cntSettings == 7)
      {
         if(cntOK == 20)      WriteString("3AП. ");
         if(cntOK == 40)      WriteString("    ");
         if(cntOK == 60)      WriteString("3AП. ");
         if(cntOK == 80)      WriteString("    ");
         if(cntOK == 100)     WriteString("3AП. ");
         
         if( cntOK <= 120 )  cntOK++;
         if(cntOK == 121)
         {
         SetBuzzer(50);
         ConvertDataBeforWriteFlash(RSet1.Voltage,RSet1.FanValue,WSet1);
         ConvertDataBeforWriteFlash(RSet2.Voltage,RSet2.FanValue,WSet2);
         ConvertDataBeforWriteFlash(RSet3.Voltage,RSet3.FanValue,WSet3);
         DatatoFlash(WSet1,WSet2,WSet3,WriteDatatoFlash,12);
         FlashWrite(WriteDatatoFlash,StartAdress,12);/* запись массива "WriteDatatoFlash" о область памяти начиная с StartAdress, размером 12 байт */
         FEnterSet = false;/* запрежаем вход в режим программирования */
         cntSettings = 0;
         cntp1 = 0; /* счетчики для отображения надписи "ПОР.Х" */
         cntp2 = 0;
         cntp3 = 0;
         
         cntf1 = 0; /* счетчики для отображения надписи "FПP.Х" */
         cntf2 = 0;
         cntf3 = 0;
         memcpy(R,RMass,sizeof(Rset)*3);
         SortValue(R);

         }
      }
      
      /* Защита от продолжительноого нажатия кнопок входа в режим программирования */
      if(KeySingleStatus.FlagDetectDePress[2] && KeySingleStatus.FlagDetectDePress[1] && FD )
      {
      FD = false;
      DelModeSet = 0;
      KeySingleStatus.FlagDetectDePress[2] = false;
      KeySingleStatus.FlagDetectDePress[1] = false;
      }
   }  

 }


   
   /*Функция сканирования индивидуальных кнопок*/
   void ScanSingleKey(void)
   {
   static uint8_t i = 0,n = 0;
  
      for( i = 0; i < KeySingleParametrs.CountKey; i++ )
      {
          /* Если обнаружено нажатие*/
       if(KeySingleParametrs.ActiveState == ActivHight)
       {
         if( Pin_Value(SingleKey[i]) )
         {
             if( KeySingleStatus.FlagDetectPress [ i] == false )KeySingleStatus.cntKeyPress[i] ++;
            
            if( KeySingleStatus.FlagDetectDePress [i] == true )
                     {
                     KeySingleStatus.Flag1 = false;
                     KeySingleStatus.FlagDetectDePress [i] = false;
                     generalflag  = false;
                     } 
         }
         /*Иначе*/
         else 
         {
             KeySingleStatus.cntKeyPress[i] = 0;/* если небыло подряд "TimeDrebezg/period_oprosa" событий то обнуляем соответствующий счектчик */
             
             /*если ранее был зафиксировано нажатие, и теперь нажатие отсутствует*/
             if(KeySingleStatus.FlagDetectPress[i] == true)
             {
              KeySingleStatus.FlagDetectDePress[i] = true;
              KeySingleStatus.FlagDetectPress[i] = false;
              KeySingleStatus.CountInOneMoment = 0;
             }
         }
      }
       
     /* Если обнаружено нажатие*/
       if(KeySingleParametrs.ActiveState == ActivLow)
       {
         if( !Pin_Value(SingleKey[i]) )
         {
             if( KeySingleStatus.FlagDetectPress [i] == false )  KeySingleStatus.cntKeyPress[i] ++;
             
         }
         /*Иначе*/
         else 
         {
             KeySingleStatus.cntKeyPress[i] = 0;/* если небыло подряд "TimeDrebezg/period_oprosa" событий то обнуляем соответствующий счетчик */

             /*если ранее был зафиксировано нажатие, и теперь нажатие отсутствует*/
             if(KeySingleStatus.FlagDetectPress[i] == true)
             {
              KeySingleStatus.FlagDetectDePress[i] = true;
              KeySingleStatus.FlagDetectPress[i] = false;
              KeySingleStatus.CountInOneMoment = 0;
             }
         }
      } 
      
      
    }
      
       for( n = 0; n < KeySingleParametrs.CountKey; n++ )
        {
            if( KeySingleStatus.cntKeyPress[n] > KeySingleParametrs.TimeDrebezg/KeySingleParametrs.period_oprosa )
            {
             KeySingleStatus.FlagDetectPress[n] = true;
             KeySingleStatus.cntKeyPress[n] = 0;       /*обнуляем соответствующий счетчик*/
            
            }
       }
       
       DecodeSingleKey(); /*Отправляем данные на декодировку*/
   
   }
