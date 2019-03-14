
#include "can.h"

const TPin CAN_RX = {GPIOB,8};
const TPin CAN_TX = {GPIOB,9};
const TPin* VCAN[] = {&CAN_RX,&CAN_TX};

CAN_InitTypeDef CAN_InitStructure;
CAN_FilterInitTypeDef CAN_FilterInitStructure;
/**
  * @brief  Настройка портов ввода-вывода, CAN
  * @note   Настройка CAN
  * @param  Baundrate - требуемая частота 
  * @retval None
  */
void CanInit(uint32_t Baundrate)
{
   uint32_t temp = 0;
    
RCC->APB1ENR |= RCC_APB1ENR_CANEN; // включаем тактирование CAN

ConfigVirualPort(VCAN,2,AF,_50MHz,PULLUP,PP);
PinAltFunc(&CAN_RX,AF4);
PinAltFunc(&CAN_TX,AF4);

temp = (uint32_t) clock_GetAPB()/(Baundrate * 16);
    
	// Инициализация шины
	

	CAN_DeInit( CAN);
	CAN_StructInit(&CAN_InitStructure);

	// CAN cell init
	CAN_InitStructure.CAN_TTCM = DISABLE; /* Запуск внутреннего таймера для слежения за периодом прихода сообщений */
	CAN_InitStructure.CAN_ABOM = DISABLE; /* Автоматическое отключение от шины */
	CAN_InitStructure.CAN_AWUM = DISABLE; /* При активности на шине выходить из сна */
	CAN_InitStructure.CAN_NART = ENABLE;  /* Разрешение прослушки шины на предмет получения всеми узлами корректного сообщения, если хоть один не подтвердит-пересылка занова */
	CAN_InitStructure.CAN_RFLM = DISABLE; /* Запрещение блокировка FIFO, перезапись не прочитанного сообщения разрешена */
	CAN_InitStructure.CAN_TXFP = DISABLE; /* Приоритет передачи сообщений в шину, anable - по хранологии, disable - по идентификатору */
	CAN_InitStructure.CAN_Mode = CAN_Mode_Silent_LoopBack;	// Для тестирования без подключенных устройств шины
	CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
	CAN_InitStructure.CAN_BS1 = CAN_BS1_13tq;
	CAN_InitStructure.CAN_BS2 = CAN_BS2_2tq;
	CAN_InitStructure.CAN_Prescaler = temp;	// Выбираем нужную скорость 250кбит = Fapb / ((Prescaler)* (1 + BS1 + BS2))
	CAN_Init(CAN, &CAN_InitStructure);

	// CAN filter init

	CAN_FilterInitStructure.CAN_FilterNumber = 1;
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;
	CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FIFO0;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);

	// CAN FIFO0 message pending interrupt enable
	CAN_ITConfig(CAN, CAN_IT_FMP0, ENABLE);

    NVIC_SetPriority(CEC_CAN_IRQn, 0); 
    NVIC_EnableIRQ(CEC_CAN_IRQn);

}

/**
  * @brief  Обработчик прерывания CAN шины
  * @note   None
  * @param  None
  * @retval None
  */
void CEC_CAN_IRQHandler(void)
{
	CanRxMsg RxMessage;

	// Обнулим структуру для хранения пакета
	RxMessage.DLC = 	0x00;
	RxMessage.ExtId = 	0x00;
	RxMessage.FMI = 	0x00;
	RxMessage.IDE = 	0x00;
	RxMessage.RTR = 	0x00;
	RxMessage.StdId = 	0x00;
	RxMessage.Data [0] = 0x00;
	RxMessage.Data [1] = 0x00;
	RxMessage.Data [2] = 0x00;
	RxMessage.Data [3] = 0x00;
	RxMessage.Data [4] = 0x00;
	RxMessage.Data [5] = 0x00;
	RxMessage.Data [6] = 0x00;
	RxMessage.Data [7] = 0x00;

	if (CAN_GetITStatus(CAN, CAN_IT_FMP0) != RESET)		// Проверим почтовый ящик
	{
		CAN_Receive(CAN, CAN_FIFO0, &RxMessage); /*считываем сообщение, и записываем его в RxMessage */

		// Проверим тип заголовка сообщения
		if (RxMessage.IDE == CAN_Id_Standard)
		{
			if (RxMessage.StdId == CAN_CMD_Test_Send)		// Если получили тестовое сообщение, то отправим в ответ подтверждение о получении
			{
				CAN_Send_Ok();
			}
			if (RxMessage.StdId == CAN_CMD_Test_Ok)			// Если получили подтверждение получения
			{
				// Вставляем любой свой код, например мигание диодом
			}
		}
	}
}

/**
  * @brief  Отправка тестовой команды в шину
  * @note   Отправляем три тестовых байта
  * @param  None
  * @retval None
  */
void CAN_Send_Test(void)
{
	CanTxMsg TxMessage;
	TxMessage.StdId = CAN_CMD_Test_Send;			// Команда шины

	TxMessage.ExtId = 0x00;							// Расширенную команду указывать нет смысла

	TxMessage.IDE = CAN_Id_Standard;				// Формат кадра
	TxMessage.RTR = CAN_RTR_DATA;					// Тип сообщения
	TxMessage.DLC = 8;								// Длина блока данных 8 - передадим три байта

	TxMessage.Data[0] = 0x00;						// Байт данных №1
	TxMessage.Data[1] = 0x01;						// Байт данных №2
	TxMessage.Data[2] = 0x02;						// Байт данных №3
    TxMessage.Data[3] = 0x03;						// Байт данных №4
    TxMessage.Data[4] = 0x04;						// Байт данных №5
    TxMessage.Data[5] = 0x05;						// Байт данных №6
    TxMessage.Data[6] = 0x06;						// Байт данных №7
    TxMessage.Data[7] = 0x07;						// Байт данных №8
	CAN_Transmit(CAN, &TxMessage);
}


/**
  * @brief  Отправка подтверждения получения тестовой команды в шину
  * @note   Отправляем только команду без данных
  * @param  None
  * @retval None
  */
void CAN_Send_Ok(void)
{
	CanTxMsg TxMessage;
	TxMessage.StdId = CAN_CMD_Test_Ok;				// Команда шины

	TxMessage.ExtId = 0x00;							// Расширенную команду указывать нет смысла

	TxMessage.IDE = CAN_Id_Standard;				// Формат кадра
	TxMessage.RTR = CAN_RTR_DATA;					// Тип сообщения
	TxMessage.DLC = 0;								// Длина блока данных 0 - ничего не передаем

	CAN_Transmit(CAN, &TxMessage);
}

