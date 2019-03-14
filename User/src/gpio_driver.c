 #include "gpio_driver.h"
 
 // установить высокий уровень 
    void output_hight(const TPin * PtrStructPin)
    {
      (PtrStructPin->Port)->BSRR = 1 << (PtrStructPin->Pin & 0x0f);
    }
    // установить низкий уровень 
     void output_low(const TPin * PtrStructPin)
    {
      (PtrStructPin->Port)->BSRR = 1 << (PtrStructPin->Pin & 0x0f) << 16;
    }
    
    
 void TogglePin(const TPin * PtrStructPin)
{
  
   (PtrStructPin->Port)->ODR ^=(1 << PtrStructPin->Pin);  
}

    
     // Настроить на выход или выход
    void PinMode(const TPin * PtrStructPin,MODE mode)
    {
     (PtrStructPin->Port)->MODER  &= ~(GPIO_MODER_MODER0 << (PtrStructPin->Pin * 2));
     (PtrStructPin->Port)->MODER |= (((uint32_t)mode) << (PtrStructPin->Pin * 2));
    }
     // Настроить режим виртуального порта
    void PinModeVirtualPort(const TPin* VPort[], uint8_t size, MODE mode)
    {
      uint8_t i = 0;   
      for(i = 0; i < size; i++)
         {
          PinMode(VPort[i],mode);
         }
    }
    /* Установка значения на ножке, в зависимости от маски */
    void SetPinValue(const TPin * PtrStructPin, uint16_t Value)      
    {
       (Value)? ((PtrStructPin->Port)->BSRR = 1 << (PtrStructPin->Pin & 0x0f)):((PtrStructPin->Port)->BSRR = 1 << (PtrStructPin->Pin & 0x0f) << 16);
    
    }
   //настройка тактирования для портов
   void TalkPort (const TPin * PtrStructPort, FunctionalState NewState)
{
  
  if (NewState != DISABLE)
  {
      if(PtrStructPort->Port == GPIOA) RCC->AHBENR |= RCC_AHBPeriph_GPIOA;
      if(PtrStructPort->Port == GPIOB) RCC->AHBENR |= RCC_AHBPeriph_GPIOB;
      if(PtrStructPort->Port == GPIOC) RCC->AHBENR |= RCC_AHBPeriph_GPIOC;
      if(PtrStructPort->Port == GPIOD) RCC->AHBENR |= RCC_AHBPeriph_GPIOD;
      if(PtrStructPort->Port == GPIOE) RCC->AHBENR |= RCC_AHBPeriph_GPIOE;
      if(PtrStructPort->Port == GPIOF) RCC->AHBENR |= RCC_AHBPeriph_GPIOF;
      
  }
  else
  {
      if(PtrStructPort->Port == GPIOA) RCC->AHBENR &= ~RCC_AHBPeriph_GPIOA;
      if(PtrStructPort->Port == GPIOB) RCC->AHBENR &= ~RCC_AHBPeriph_GPIOB;
      if(PtrStructPort->Port == GPIOC) RCC->AHBENR &= ~RCC_AHBPeriph_GPIOC;
      if(PtrStructPort->Port == GPIOD) RCC->AHBENR &= ~RCC_AHBPeriph_GPIOD;
      if(PtrStructPort->Port == GPIOE) RCC->AHBENR &= ~RCC_AHBPeriph_GPIOE;
      if(PtrStructPort->Port == GPIOF) RCC->AHBENR &= ~RCC_AHBPeriph_GPIOF;
  }
}
  // настроить порт на вход
void gpio_DigitalInput(const TPin * PtrStructPopt)
{
       (PtrStructPopt->Port)->MODER &= ~(0x03UL << (PtrStructPopt->Pin * 2));
}

// настройка скорости порта
  void SetSpeedPin(const TPin * PtrStructPopt,SPEED speed )
{
    PtrStructPopt->Port->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR0 << (PtrStructPopt->Pin * 2));
    PtrStructPopt->Port->OSPEEDR |= ((uint32_t)speed << (PtrStructPopt->Pin * 2));
   
}
  // настроить пин на режим с открытым колектором
  void PinOpenDrain(const TPin * PtrStructPopt)
  {
      (PtrStructPopt->Port)->OTYPER |= (1 << PtrStructPopt->Pin);
  }
  // настроить пин на режим работы пушпул
  void PinPushPull(const TPin * PtrStructPopt)
  {
      (PtrStructPopt->Port)->OTYPER &= ~(1 << PtrStructPopt->Pin); 
  }
  
  void PinPPorOD(const TPin * PtrStructPopt, PP_OD pp_od)
  {
     if(pp_od == 0) (PtrStructPopt->Port)->OTYPER &= ~(1 << PtrStructPopt->Pin); 
     else (PtrStructPopt->Port)->OTYPER |= (1 << PtrStructPopt->Pin);  
  }

   // подтяжка пина вверх
   void PinPullUp(const TPin * PtrStructPin)  
   {
      uint32_t tmpreg;
     
      tmpreg = (PtrStructPin->Port)->PUPDR;
      tmpreg &= ~(GPIO_PUPDR_PUPDR0 << ((uint16_t)PtrStructPin->Pin * 2));
      tmpreg |= (((uint32_t)PULLUP) << (PtrStructPin->Pin * 2));
      (PtrStructPin->Port)->PUPDR = tmpreg;
   }
   // подтяжка пина вниз
   void PinPullDown(const TPin * PtrStructPin)  
   {
      uint32_t tmpreg;
     
      tmpreg = (PtrStructPin->Port)->PUPDR;
      tmpreg &= ~(GPIO_PUPDR_PUPDR0 << ((uint16_t)PtrStructPin->Pin * 2));
      tmpreg |= (((uint32_t)PULLDOWN) << (PtrStructPin->Pin * 2));
      (PtrStructPin->Port)->PUPDR = tmpreg;
   }
   // без подтяжки
   void PinNoPUPD(const TPin * PtrStructPin)  
   {
      uint32_t tmpreg;
     
      tmpreg = (PtrStructPin->Port)->PUPDR;
      tmpreg &= ~(GPIO_PUPDR_PUPDR0 << ((uint16_t)PtrStructPin->Pin * 2));
      tmpreg |= (((uint32_t)NOPULL) << (PtrStructPin->Pin * 2));
      (PtrStructPin->Port)->PUPDR = tmpreg;
   }
    // ножка либо подтфнута ВВЕРХ, либо ВНИЗ, либо БЕЗПОДТЯЖКИ в параметре определяем ее ссотояние
    void PinPUPD(const TPin * PtrStructPin, PULLUPDOWN pulluppd)
    {
     uint32_t tmpreg;
     
      tmpreg = (PtrStructPin->Port)->PUPDR;
      tmpreg &= ~(GPIO_PUPDR_PUPDR0 << ((uint16_t)PtrStructPin->Pin * 2));
      tmpreg |= (((uint32_t)pulluppd) << (PtrStructPin->Pin * 2));
     (PtrStructPin->Port)->PUPDR = tmpreg;
       
//       GPIOx->PUPDR &= ~(GPIO_PUPDR_PUPDR0 << ((uint16_t)pinpos * 2));
//      GPIOx->PUPDR |= (((uint32_t)GPIO_InitStruct->GPIO_PuPd) << (pinpos * 2));
//       
 //      (PtrStructPin->Port)->PUPDR &= 
    }
   
     
  /**
  * @brief  Changes the mapping of the specified pin.
  * @param  GPIOx: where x can be (A..K) to select the GPIO peripheral for STM32F405xx/407xx and STM32F415xx/417xx devices
  *                      x can be (A..I) to select the GPIO peripheral for STM32F42xxx/43xxx devices.
  *                      x can be (A, B, C, D and H) to select the GPIO peripheral for STM32F401xx devices. 
  * @param  GPIO_PinSource: specifies the pin for the Alternate function.
  *         This parameter can be GPIO_PinSourcex where x can be (0..15).
  * @param  GPIO_AFSelection: selects the pin to used as Alternate function.
  *          This parameter can be one of the following values:
  *            @arg GPIO_AF_RTC_50Hz: Connect RTC_50Hz pin to AF0 (default after reset) 
  *            @arg GPIO_AF_MCO: Connect MCO pin (MCO1 and MCO2) to AF0 (default after reset) 
  *            @arg GPIO_AF_TAMPER: Connect TAMPER pins (TAMPER_1 and TAMPER_2) to AF0 (default after reset) 
  *            @arg GPIO_AF_SWJ: Connect SWJ pins (SWD and JTAG)to AF0 (default after reset) 
  *            @arg GPIO_AF_TRACE: Connect TRACE pins to AF0 (default after reset)
  *            @arg GPIO_AF_TIM1: Connect TIM1 pins to AF1
  *            @arg GPIO_AF_TIM2: Connect TIM2 pins to AF1
  *            @arg GPIO_AF_TIM3: Connect TIM3 pins to AF2
  *            @arg GPIO_AF_TIM4: Connect TIM4 pins to AF2
  *            @arg GPIO_AF_TIM5: Connect TIM5 pins to AF2
  *            @arg GPIO_AF_TIM8: Connect TIM8 pins to AF3
  *            @arg GPIO_AF_TIM9: Connect TIM9 pins to AF3
  *            @arg GPIO_AF_TIM10: Connect TIM10 pins to AF3
  *            @arg GPIO_AF_TIM11: Connect TIM11 pins to AF3
  *            @arg GPIO_AF_I2C1: Connect I2C1 pins to AF4
  *            @arg GPIO_AF_I2C2: Connect I2C2 pins to AF4
  *            @arg GPIO_AF_I2C3: Connect I2C3 pins to AF4
  *            @arg GPIO_AF_SPI1: Connect SPI1 pins to AF5
  *            @arg GPIO_AF_SPI2: Connect SPI2/I2S2 pins to AF5
  *            @arg GPIO_AF_SPI4: Connect SPI4 pins to AF5 
  *            @arg GPIO_AF_SPI5: Connect SPI5 pins to AF5 
  *            @arg GPIO_AF_SPI6: Connect SPI6 pins to AF5
  *            @arg GPIO_AF_SAI1: Connect SAI1 pins to AF6 for STM32F42xxx/43xxx devices.       
  *            @arg GPIO_AF_SPI3: Connect SPI3/I2S3 pins to AF6
  *            @arg GPIO_AF_I2S3ext: Connect I2S3ext pins to AF7
  *            @arg GPIO_AF_USART1: Connect USART1 pins to AF7
  *            @arg GPIO_AF_USART2: Connect USART2 pins to AF7
  *            @arg GPIO_AF_USART3: Connect USART3 pins to AF7
  *            @arg GPIO_AF_UART4: Connect UART4 pins to AF8
  *            @arg GPIO_AF_UART5: Connect UART5 pins to AF8
  *            @arg GPIO_AF_USART6: Connect USART6 pins to AF8
  *            @arg GPIO_AF_UART7: Connect UART7 pins to AF8
  *            @arg GPIO_AF_UART8: Connect UART8 pins to AF8
  *            @arg GPIO_AF_CAN1: Connect CAN1 pins to AF9
  *            @arg GPIO_AF_CAN2: Connect CAN2 pins to AF9
  *            @arg GPIO_AF_TIM12: Connect TIM12 pins to AF9
  *            @arg GPIO_AF_TIM13: Connect TIM13 pins to AF9
  *            @arg GPIO_AF_TIM14: Connect TIM14 pins to AF9
  *            @arg GPIO_AF_OTG_FS: Connect OTG_FS pins to AF10
  *            @arg GPIO_AF_OTG_HS: Connect OTG_HS pins to AF10
  *            @arg GPIO_AF_ETH: Connect ETHERNET pins to AF11
  *            @arg GPIO_AF_FSMC: Connect FSMC pins to AF12 
  *            @arg GPIO_AF_FMC: Connect FMC pins to AF12 for STM32F42xxx/43xxx devices.   
  *            @arg GPIO_AF_OTG_HS_FS: Connect OTG HS (configured in FS) pins to AF12
  *            @arg GPIO_AF_SDIO: Connect SDIO pins to AF12
  *            @arg GPIO_AF_DCMI: Connect DCMI pins to AF13
  *            @arg GPIO_AF_LTDC: Connect LTDC pins to AF14 for STM32F429xx/439xx devices. 
  *            @arg GPIO_AF_EVENTOUT: Connect EVENTOUT pins to AF15
  * @retval None
  */
   
   void PinAltFunc(const TPin * PtrStructPin, ALTFUNC altfunc)
   {
      uint32_t temp = 0x00;
      uint32_t temp_2 = 0x00;
  
      temp = ((uint32_t)(altfunc) << ((uint32_t)((uint32_t)PtrStructPin->Pin & (uint32_t)0x07) * 4));
      (PtrStructPin->Port)->AFR[PtrStructPin->Pin >> 0x03] &= ~((uint32_t)0xF << ((uint32_t)((uint32_t)PtrStructPin->Pin & (uint32_t)0x07) * 4));
      temp_2 = (PtrStructPin->Port)->AFR[PtrStructPin->Pin >> 0x03] | temp;
      PtrStructPin->Port->AFR[PtrStructPin->Pin >> 0x03] = temp_2;
   }

  /**
  *  @brief   Функция записи значения в виртуальный порт
  *  @param  Value - записываемое значение  
  *  @param  VPort[] - массив из указателей на TPin            
  *  @retval None
  */
   
void WriteToVirtualPort(const TPin* VPort[], uint8_t size,uint16_t Value)
 {   
    uint8_t i = 0;
    
     for(i = 0; i < size; i++)
     {
     if(Value & 1)output_hight(VPort[i]);
     else output_low(VPort[i]);
     Value >>= 1;
     }     
 }
 /**
  *  @brief   Функция конфигурирования виртуального порта   
  *  @param  VPort[] - массив из указателей на TPin 
  *  @param  size - размер виртуального порта
  *  @param  mode - OUT,IN,AF,AN
  *  @param  speed - 2,10,50 mgHz
  *  @param  PUPD - PU,PD, NOPUPD
  *  @param  pp_od - пишпул или открытый колектор 
  *  @retval None
  */
void ConfigVirualPort(const TPin* VPort[], uint8_t size, MODE mode,SPEED speed,PULLUPDOWN PUPD,PP_OD pp_od)
{
     uint8_t i = 0;
    
     for(i = 0; i < size; i++)
     {
       TalkPort(VPort[i],ENABLE);
       PinMode(VPort[i],mode);   
       SetSpeedPin(VPort[i],speed);   
       PinPUPD(VPort[i],PUPD);
       PinPPorOD(VPort[i],pp_od);
     } 
}

void ConfigVirualPortDinamic(TPin* VPort[], uint8_t size, MODE mode,SPEED speed,PULLUPDOWN PUPD,PP_OD pp_od)
{
     uint8_t i = 0;
    
     for(i = 0; i < size; i++)
     {
       TalkPort(VPort[i],ENABLE);
       PinMode(VPort[i],mode);   
       SetSpeedPin(VPort[i],speed);   
       PinPUPD(VPort[i],PUPD);
       PinPPorOD(VPort[i],pp_od);
     } 
}
/**
  *  @brief  Функция чтения состояния ножки   
  *  @param  PtrStructPin - указатель на структуру содержащую параметры порта и пина  
  *  @retval true - если на ноге "1", false - если на ноге "0"
  */
bool Pin_Value(const TPin * PtrStructPin)
{
  bool bitstatus = false;
     
  if (((PtrStructPin->Port)->IDR & (1 << PtrStructPin->Pin)) != 0 ) bitstatus = true;
  else  bitstatus = false;
  return bitstatus;
}
