#include "semiseg.h"
   
   TSemisegment Semiseg;
   
   extern bool Del;
   bool EndStringHAPR = false;
   
   
   const TPin  DIG1 = {GPIOB,7};
   const TPin  DIG2 = {GPIOB,6};
   const TPin  DIG3 = {GPIOA,1};
   const TPin  DIG4 = {GPIOA,6};
   
   const TPin* SEG[] = {&DIG1,&DIG2,&DIG3,&DIG4};
   
   char* STR = "Н.А.ПР";
   
   const TPin  A  = {GPIOB,4};
   const TPin  B  = {GPIOA,7};
   const TPin  C  = {GPIOA,2};
   const TPin  D  = {GPIOA,3};
   const TPin  E  = {GPIOC,14};
   const TPin  F  = {GPIOB,5};
   const TPin  G  = {GPIOA,5};
   const TPin  DP = {GPIOC,13};
   const TPin  DD = {GPIOA,4};
   
   const TPin* DataSeg[] = {&A,&B,&C,&D,&E,&F,&G,&DP,&DD};

   /* Конвертировать данные для семисегмента
      data  - данные
      count - позиция текущих данных в массиве, для привязки точки
   */
   static uint8_t ConvertData(uint8_t data, uint8_t count)
   {
     static uint8_t ConvertData = 0;
      
     switch((uint8_t) data)
     {
        case 0:   ConvertData = 0x3F; break; /* "0" */
        case 1:   ConvertData = 0x06; break; /* "1" */
        case 2:   ConvertData = 0x5B; break; /* "2" */
        case 3:   ConvertData = 0x4F; break; /* "3" */
        case 4:   ConvertData = 0x66; break; /* "4" */
        case 5:   ConvertData = 0x6D; break; /* "5" */
        case 6:   ConvertData = 0x7D; break; /* "6" */
        case 7:   ConvertData = 0x07; break; /* "7" */
        case 8:   ConvertData = 0x7F; break; /* "8" */
        case 9:   ConvertData = 0x6F; break; /* "9" */
        
        case 48:  ConvertData = 0x3F; break; /* "0" */
        case 49:  ConvertData = 0x06; break; /* "1" */
        case 50:  ConvertData = 0x5B; break; /* "2" */
        case 51:  ConvertData = 0x4F; break; /* "3" */
        case 52:  ConvertData = 0x66; break; /* "4" */
        case 53:  ConvertData = 0x6D; break; /* "5" */
        case 54:  ConvertData = 0x7D; break; /* "6" */
        case 55:  ConvertData = 0x07; break; /* "7" */
        case 56:  ConvertData = 0x7F; break; /* "8" */
        case 60:  ConvertData = 0x6F; break; /* "9" */
        
        case 65:  ConvertData = 0x77; break; /* "A" */
        case 67:  ConvertData = 0x39; break; /* "C" */
        case 69:  ConvertData = 0x79; break; /* "E" */
        case 70:  ConvertData = 0x71; break; /* "F" */
        case 72:  ConvertData = 0x76; break; /* "H" */
        case 74:  ConvertData = 0x0E; break; /* "J" */
        case 76:  ConvertData = 0x38; break; /* "L" */
        case 79:  ConvertData = 0x3F; break; /* "O" */
        case 80:  ConvertData = 0x73; break; /* "P" */
        case 83:  ConvertData = 0x6D; break; /* "S" */
        case 85:  ConvertData = 0x3E; break; /* "U" */
        case 86:  ConvertData = 0x3E; break; /* "V" */
        
        case 98:  ConvertData = 0x7C; break; /* "b" */
        case 99:  ConvertData = 0x58; break; /* "c" */
        case 104: ConvertData = 0x74; break; /* "h" */
        case 110: ConvertData = 0x54; break; /* "n" */
        case 111: ConvertData = 0x54; break; /* "o" */
        case 114: ConvertData = 0x50; break; /* "r" */
        
        
        case 192: ConvertData = 0x77; break; /* "А" */
        case 193: ConvertData = 0x7B; break; /* "Б" */
        case 194: ConvertData = 0x7F; break; /* "В" */
        
        case 205: ConvertData = 0x76; break; /* "Н" */
        case 206: ConvertData = 0x3F; break; /* "О" */
        case 207: ConvertData = 0x37; break; /* "П" */
        case 208: ConvertData = 0x73; break; /* "Р" */
        
        
        case 255: ConvertData = 0x00; break; /* пусто */
        default : ConvertData = 0x00;
        break;
     }
        if(Semiseg.Data.EnableDrop[count]) ConvertData |= ( 1 << 7 );
        return ConvertData;
   }
   
   
   
   
   /* Запись сдвинутых данных для текущей позиции цифры */
  static void ShiftSegment(uint8_t CurrentSegment)
   {
      WriteToVirtualPort(SEG,Semiseg.SizeDigit,CurrentSegment);
   }
   
   
   
   /* Запись информации в текущую цифру */
   void WriteDatatoDigit(uint8_t Data)
   {
    WriteToVirtualPort(DataSeg,Semiseg.SizeSeg,ConvertData(Semiseg.Data.Data[Data],Data));
   }
   
   /* Запись в семисегментник, общий алгоритм, ее необходимо выполнять переодически */
   void WriteDatatoSemisegment(void)
   {
      static uint8_t ShiftData = 0, i = 4;
      uint8_t Temp = 0;
      
      /* Перед сдвигом обнуляем данные, иначе будет паразитный засвет */
       WriteToVirtualPort(DataSeg,Semiseg.SizeSeg,0);
      
      i--;
      if(i == Semiseg.Brightness)
      {
      Temp |= (1 << ShiftData);
      ShiftSegment(Temp);
      
     //WriteToVirtualPort(DataSeg,Semiseg.SizeSeg,ConvertData(Semiseg.DataMass[ShiftData],false));
     WriteDatatoDigit(ShiftData);
       i = 4;
      ShiftData++;
      if(ShiftData > Semiseg.SizeDigit) ShiftData = 0;
      }
   }
   
    /* записывает в выходной массив строку, обрабатывает точки, обрезает лишнее */
   void WriteString(char* str)
   {
      uint8_t i = 0, k = 0,z = 0,lenght = 0;
      lenght  = strlen(str);
      
      memset(Semiseg.Data.Data,' ',Semiseg.SizeDigit);
      memset(Semiseg.Data.EnableDrop,false,Semiseg.SizeDigit);
      for( i = 0; i < lenght; i++ )
      {
         if( str[i] != '.' && str[i] != '\0' )
         {
            Semiseg.Data.Data[k] = str[i];
            k++;
         }
         else 
         {
            if( i == 0 )
               {
                  Semiseg.Data.EnableDrop[0] = true;
                  k = 1;
               }
            else
               {
                  z++;
                  Semiseg.Data.EnableDrop[i-z] = true;
               }
         }
      }
   }
   
   
   /* тестовая статическая функция вывода строки */
   void ShiftString(void)
   {  
       static uint8_t i = 0;
       
      
      if(Del) i++;
      
      if(!EndStringHAPR && Del)
      {
         switch( i )
         {
            case 1: { WriteString("   Н");Del = false; }
            break;
            case 2: { WriteString("  НА");Del = false; }
            break;
            case 3: { WriteString(" НАП");Del = false; }
            break;
            case 4: { WriteString("НАПР.");Del = false; }
            break;
            case 5: { WriteString("    ");Del = false; }
            break;
            case 6: { WriteString("НАПР.");Del = false; }
            break;
            case 7: { WriteString("    ");Del = false; }
            break;
            case 8: { WriteString("НАПР.");Del = false; }
            break;
            case 9: { WriteString("    ");Del = false; }
            break;
            case 10: { WriteString("НАПР.");Del = false; EndStringHAPR = true; i = 0; }
            break;
         }
     }
   }
   
   
   void PrintStr(char* str)
   {
    static uint8_t i = 0;
    bool EndString = false;
    

     if(EndString == false )
     {
      if(Del) i++;
        
      switch(i)
      {
         case 1: {WriteString(str); Del = false; }
         break;
         
      
      }
   
     }
   }
  
/* служебная функция для вывода бегущей строки */
static void fanc1(uint8_t lenght)
   {
      uint8_t i = 0, k = Semiseg.SizeDigit - 1;
      
      for( i = lenght;  i > 0; i-- )
      {
            Semiseg.Data.Data[k]       =  Semiseg.Temp.Data[i-1];
            Semiseg.Data.EnableDrop[k] =  Semiseg.Temp.EnableDrop[i-1];
            k--;
      }
   }
   
   /* копирование отформатированной строки, для дальнейшего вывода на семисегментник */
 static void CopyFormatStr(char* str)
   {
    uint8_t i = 0, k = 0,z = 0,lenght = 0;
      lenght  = strlen(str);
      
      memset(Semiseg.Temp.Data,' ',Semiseg.SizeDigit);
      memset(Semiseg.Temp.EnableDrop,false,Semiseg.SizeDigit);
      for( i = 0; i < lenght; i++ )
      {
         if( str[i] != '.' && str[i] != '\0' )
         {
            Semiseg.Temp.Data[k] = str[i];
            k++;
         }
         else 
         {
            if( i == 0 )
               {
                  Semiseg.Temp.EnableDrop[0] = true;
                  k = 1;
               }
            else
               {
                  z++;
                  Semiseg.Temp.EnableDrop[i-z] = true;
               }
         }
      }
   }
/* Главня функция формирующая бегущую строку, используется дополнительный таймер задающий интервал, Del - глобальный флаг разрешающий следующий сдвиг */
  static void ShiftSTR(char* str)     
   {
    static uint8_t n = 0;
      
       CopyFormatStr(str);
       
       if(Del) n++;
      
      if(!Semiseg.EndString && Del)
      {
         memset(Semiseg.Data.Data,' ',Semiseg.SizeDigit);
         memset(Semiseg.Data.EnableDrop,false,Semiseg.SizeDigit);
         fanc1(n);
         Del = false;
         if(n == Semiseg.SizeDigit){n = 0;Semiseg.EndString = true;}
      }
   }
   /* для вывода строки использовать эту функцию */
   void ShiftStr(char* str)
   {
       memset(Semiseg.TempData,' ',Semiseg.SizeDigit * 2);
       memcpy(Semiseg.TempData,str,Semiseg.SizeDigit * 2);
       Semiseg.EndString  = false;
   }
  
/*  функция выполняемая в потоке */
void PrintShiftString(void)
    {
     ShiftSTR(Semiseg.TempData);
    }

   
   
   /* Инициализация семисегментника */
    void InitSemiseg(TypeSemisegment Type, uint8_t SizeDigit, uint8_t SizeSeg)
    {
       Semiseg.Data.Data         = (uint8_t*) malloc(Semiseg.SizeDigit * sizeof(uint8_t));
       Semiseg.Data.EnableDrop   = (bool*) malloc(Semiseg.SizeDigit * sizeof(bool));
       
       Semiseg.Temp.Data         = (uint8_t*) malloc(Semiseg.SizeDigit * sizeof(uint8_t));
       Semiseg.Temp.EnableDrop   = (bool*) malloc(Semiseg.SizeDigit * sizeof(bool));
       
       Semiseg.TempData          = (char*) malloc(Semiseg.SizeDigit * 2 * sizeof(char)); /* в два раза больше с учетом точек */
       Semiseg.Type              = Type;
       Semiseg.SizeDigit         = SizeDigit;
       Semiseg.SizeSeg           = SizeSeg;
       Semiseg.Brightness        = 2;
       Semiseg.EndString         = false;
       
       ConfigVirualPort(SEG,Semiseg.SizeDigit,OUT,_50MHz,NOPULL,PP);
       ConfigVirualPort(DataSeg,Semiseg.SizeSeg,OUT,_50MHz,NOPULL,PP);
       
       //WriteString("");
       
       //ShiftString();

       
    }
