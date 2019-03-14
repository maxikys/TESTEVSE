#include "LED_ini.h"

void LED_ini(void)
	
{
	GPIO_InitTypeDef GPIO_ini_LED;
	//RCC_AHBPeriph_GPIOE
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOE,ENABLE);
	GPIO_ini_LED.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_ini_LED.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_ini_LED.GPIO_OType = GPIO_OType_PP;
	GPIO_ini_LED.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_ini_LED.GPIO_Speed = GPIO_Speed_Level_1;
		
	GPIO_Init(GPIOE,&GPIO_ini_LED);

}

void button_ini(void)
	
{
	GPIO_InitTypeDef GPIO_ini_button;
	//RCC_AHBPeriph_GPIOA
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE);
	GPIO_ini_button.GPIO_Pin = GPIO_Pin_2;
	GPIO_ini_button.GPIO_Mode = GPIO_Mode_IN;
	GPIO_ini_button.GPIO_OType = GPIO_OType_PP;
	GPIO_ini_button.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_ini_button.GPIO_Speed = GPIO_Speed_Level_1;
		
	GPIO_Init(GPIOA,&GPIO_ini_button);
}
