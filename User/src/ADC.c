#include "ADC.h"

extern bool EndStringHAPR;
extern uint8_t cntKeyR;


const TPin Pb0 = {GPIOB,0};
const TPin* VPb0[] = {&Pb0};


void ADC_Config(void)
{
   __IO uint32_t i = 0;
  ADC_InitTypeDef     ADC_InitStructure;
//  GPIO_InitTypeDef    GPIO_InitStructure;
  
  ConfigVirualPort(VPb0,1,AN,_2MHz,NOPULL,PP);
  RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;/*Тактирование ADC*/
  /* ADCs DeInit */  
  ADC_DeInit(ADC1);
  
  /* Initialize ADC structure */
  ADC_StructInit(&ADC_InitStructure);
  
  /* Configure the ADC1 in continuous mode with a resolution equal to 12 bits  */
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE; 
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_ScanDirection = ADC_ScanDirection_Upward;
  ADC_Init(ADC1, &ADC_InitStructure); 

  ADC_ChannelConfig(ADC1, ADC_Channel_8 , ADC_SampleTime_239_5Cycles);
  /* ADC Calibration */
  ADC_GetCalibrationFactor(ADC1);
for(i = 0; i < 100000; i++ ){} 
  /* Enable the ADC peripheral */
  ADC_Cmd(ADC1, ENABLE);     
  
  /* Wait the ADRDY flag */
  while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADRDY)); 
//  
  /* ADC1 regular Software Start Conv */ 
  ADC_StartOfConversion(ADC1);
  
}


uint16_t ADCConversion(void)
 {
      uint32_t ADC1ConvertedValue = 0, ADC1ConvertedVoltage = 0;
      float AdcValue = 0.0;
    /* Test EOC flag */
    
    while(ADC_GetFlagStatus(ADC1, ADC_ISR_EOSEQ) == RESET);

    /* Get ADC1 converted data */
    ADC1ConvertedValue = ADC_GetConversionValue(ADC1);
    
    /* Compute the voltage */
    ADC1ConvertedVoltage = ( ADC1ConvertedValue * 3270 )/( 1 << 12 ) - 1 ;
    AdcValue = (float)(12.0/1.102);
    AdcValue =  (float)(AdcValue * ADC1ConvertedVoltage);
    AdcValue = (uint32_t)AdcValue;

    ADC_StartOfConversion(ADC1);
    return AdcValue;
 }
 
 uint16_t GetAdcValue(void)
{
   uint32_t Temp = 0;
   
   Temp = ADCConversion()/10;
   
   
   
   if( cntKeyR == 0 )
   {
   Semiseg.TempData[0] = (Temp / 1000);
                              
   
   Semiseg.TempData[1] = ( (Temp % 1000) / 100 );
                                   
   
   Semiseg.TempData[2] = ( (Temp % 1000) % 100 / 10 );
   
   
   Semiseg.TempData[3] = ( (Temp % 1000) % 100 % 10 / 1 );
   
   memset(Semiseg.Data.EnableDrop,false,Semiseg.SizeDigit);
   
    if(Temp >= 1000 && Temp < 10000)
    {
      memcpy(Semiseg.Data.Data,Semiseg.TempData,Semiseg.SizeDigit);
      Semiseg.Data.EnableDrop[1] = true;
    }
    else if(Temp >= 100 && Temp < 1000)
    {
      Semiseg.TempData[0] = 255;
      memcpy(Semiseg.Data.Data,Semiseg.TempData,Semiseg.SizeDigit);
      Semiseg.Data.EnableDrop[1] = true;
    }
    else if(Temp >= 10 && Temp < 100)
    {
      Semiseg.TempData[0] = 255;
      memcpy(Semiseg.Data.Data,Semiseg.TempData,Semiseg.SizeDigit);
      Semiseg.Data.EnableDrop[1] = true;
    }
 }
 return Temp;  
}
 
 
 
