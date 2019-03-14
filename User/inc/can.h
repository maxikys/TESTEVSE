#ifndef __CAN
#define __CAN

#include "stm32f0xx.h"
#include "stm32f0xx_gpio.h"
#include "stm32f0xx_can.h"
#include "gpio_driver.h"
#include "clock.h"


/******************************************************************************
 * Определение команд протокола
 ******************************************************************************/
#define CAN_CMD_Test_Send			0x0001		// Команда отправки тестового сообщения
#define CAN_CMD_Test_Ok				0x0002		// Команда подтверждения тестового сообщения




/******************************************************************************
 * Определение прототипов функций
 ******************************************************************************/
void CanInit(uint32_t Baundrate);
void USB_LP_CAN1_RX0_IRQHandler(void);

void CAN_Send_Test(void);
void CAN_Send_Ok(void);

#endif //__CAN
