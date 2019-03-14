 #include "Spi.h"


extern TKeyboardParametrs       KeyboardParametrs;
extern TKeyStatus               KeyStatus;

 

/*Маски для проецировки на выходной массив*/
enum
{

    SB8_W,      /*маска 1<<1*/
    SB7_W,      /*маска 1<<2*/
    SB6_W,      /*маска 1<<3*/
    SB5_W,      /*маска 1<<4*/
    SB4_W,      /*маска 1<<5*/
    SB3_W,      /*маска 1<<6*/
    SB2_W,      /*маска 1<<7*/
    SB1_W,      /*маска 1<<8*/

} LedInDD3;

enum
{
    SB8_Y,      /*маска 1<<1*/
    SB7_Y,      /*маска 1<<2*/
    SB6_Y,      /*маска 1<<3*/
    SB5_Y,      /*маска 1<<4*/
    SB4_Y,      /*маска 1<<5*/
    SB3_Y,      /*маска 1<<6*/
    SB2_Y,      /*маска 1<<7*/
    SB1_Y,      /*маска 1<<8*/
} LedInDD2;

 

  
 /*Тип хранящий количестов микросхем и указатель на буфер записи в эти микросхемы*/
 typedef struct
 {
    uint8_t Count_Hct;
    uint8_t* buffer_SPI_OUT;
 
 }THct;
 THct Spi_Led;

/* Тип хранящий маску светодиода и номер микросхемы в которую будут записываться эти маски */ 
 

 
/*маски с привязкой к микросхеме, "0" - последняя микросхема в цепочке(первый байт в массиве,) "2" - первая микросхема в цепочке ( последний байт в массиве)*/

 
 const TMaskLed MaskSB1W = {SB1_W,0,(void*)0};
 const TMaskLed MaskSB1Y = {SB1_Y,1,(void*)0};
 
 const TMaskLed MaskSB2W = {SB2_W,0,(void*)0};
 const TMaskLed MaskSB2Y = {SB2_Y,1,(void*)0};
 
 const TMaskLed MaskSB3W = {SB3_W,0,(void*)0};
 const TMaskLed MaskSB3Y = {SB3_Y,1,(void*)0};
 
 const TMaskLed MaskSB4W = {SB4_W,0,(void*)0};
 const TMaskLed MaskSB4Y = {SB4_Y,1,(void*)0};
 
 const TMaskLed MaskSB5W = {SB5_W,0,(void*)0};
 const TMaskLed MaskSB5Y = {SB5_Y,1,(void*)0};
 
 const TMaskLed MaskSB6W = {SB6_W,0,(void*)0};
 const TMaskLed MaskSB6Y = {SB6_Y,1,(void*)0};
 
 const TMaskLed MaskSB7W = {SB7_W,0,(void*)0};
 const TMaskLed MaskSB7Y = {SB7_Y,1,(void*)0};
 
 const TMaskLed MaskSB8W = {SB8_W,0,(void*)0};
 const TMaskLed MaskSB8Y = {SB8_Y,1,(void*)0};
 
 
/* массив указателей для кнопок зеленый цвет */
 const TMaskLed* MassButLedGreen[] =    {
                                                &MaskSB1Y,      &MaskSB2Y,      &MaskSB3Y,
                                                &MaskSB4Y,      &MaskSB5Y,      &MaskSB6Y,
                                                &MaskSB7Y,      &MaskSB8Y,      (void*)0,
                                        };



/* массив указателей для кнопок белый цвет */
 const TMaskLed* MassButLedWhite[] =    {
                                                &MaskSB1W,      &MaskSB2W,      &MaskSB3W,
                                                &MaskSB4W,      &MaskSB5W,      &MaskSB6W,
                                                &MaskSB7W,      &MaskSB8W,      (void*)0,
                                        };

                         
/* Объявления портов */

const TPin SPI1MOSI ={GPIOA,7};
const TPin SPI1SCK ={GPIOA,5};
const TPin SPI1LE ={GPIOA,6};

const TPin BR_W ={GPIOB,0};
const TPin BR_Y ={GPIOB,1};

const TPin* PortSpi[] = {&SPI1MOSI,&SPI1SCK};
const TPin* LE[] = {&SPI1LE};

const TPin* V_BR[] = {&BR_Y,&BR_W};

//SPI_InitTypeDef SPI_ini;

extern TKeyboardParametrs       KeyboardParametrs;


    

void SPI1_ini(void)
{
    //uint8_t i = 0;
//    ConfigVirualPort(V_BR,2,OUT,_50MHz,NOPULL,PP);     /* Конфинурирование ОЕ */
//    output_low(V_BR[0]);                               /* Включаем выход */
//    output_low(V_BR[1]);
   
    ConfigVirualPort(LE,1,OUT,_50MHz,NOPULL,PP);                 /* Конфинурирование LE */
    
    //RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);        /*Тактирование SPI*/
    
    
    ConfigVirualPort(PortSpi,2,OUT,_50MHz,NOPULL,PP);            /* Конфинурирование SPI1MOSI,SPI1SCK */
       
    Spi_Led.Count_Hct = 2;
    Spi_Led.buffer_SPI_OUT = (uint8_t*) malloc( Spi_Led.Count_Hct * sizeof(uint8_t));
       
    Spi_Led.buffer_SPI_OUT[0] = 0x00;
    Spi_Led.buffer_SPI_OUT[1] = 0x00;
    
    
}    

/* Универсальная функция управления светодиодами */
 void DirectLed( const TMaskLed* Mass[], uint8_t NumberKey, TLedState State )
{
    if(!Mass[NumberKey]->Handler)
    {
        if(State == LedOn)
        {
            Spi_Led.buffer_SPI_OUT[ Mass[NumberKey]->NumDDMask ] |= ( 1 << Mass[NumberKey]->Mask );
        }
        else
        {
            Spi_Led.buffer_SPI_OUT[ Mass[NumberKey]->NumDDMask ] &= ~( 1 << Mass[NumberKey]->Mask );
        }
   }
}



 /*функция опроса состояния флагов и записи масок в выходной буфер HCT*/
 void CheckPressButton(void)
{  

//         uint8_t i = 0;
//         
//         for( i = 0; i < KeyboardParametrs.SizeColumn * KeyboardParametrs.SizeRow; i++ )
//         {
//            if(KeyStatus.FlagDetectPress[i] == true)
//            {
//                DirectLed(MassButLedGreen,i,LedOn);
//                DirectLed(MassButLedWhite,i,LedOff);
//            }
//            else
//            {
//            DirectLed(MassButLedGreen,i,LedOff);
//            DirectLed(MassButLedWhite,i,LedOn);
//            }
//         }
   
 //  DirectLed(MassButLedWhite,0,LedOn);
 //  DirectLed(MassButLedGreen,1,LedOn);
}


/*Процедура записи данных в HCT595*/
void Write_SPI(void)
{
    uint8_t i;

    for(i = 0; i < Spi_Led.Count_Hct; i++)
    {
     WriteShiftReg(*(Spi_Led.buffer_SPI_OUT + i));
    }
    output_hight(LE[0]);
    output_low(LE[0]);
}

 /*Программый SPI*/
void WriteShiftReg(uint8_t value)
{   
    uint8_t i;
    
    for(i = 0; i < 8; i++)
    {
        if(value & 0x01)
        output_hight(PortSpi[0]);
        else
        output_low(PortSpi[0]);
        
        output_hight(PortSpi[1]);
        value >>= 1;
        output_low(PortSpi[1]);
    }
}

