#include "NVIC_ini.h"

void NVIC_ini(void)
	
{
	NVIC_InitTypeDef NVIC_InitStructure;
  /* Configure the Priority Group to 2 bits */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 
  
  /* Enable the USARTx Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//  uart1 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; //
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //
	NVIC_Init(&NVIC_InitStructure); //
	
}

