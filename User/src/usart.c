#include "usart.h"

/*******************************************************************/

GPIO_InitTypeDef gpio;
USART_InitTypeDef usart;

uint8_t sendData[8];
uint8_t bytesToSend = 8;

uint8_t sendDataCounter = 0;



// буфер на прием
#define RX_BUFFER_SIZE 256 // размер буфера
volatile uint8_t    rx_buffer[RX_BUFFER_SIZE];
volatile uint16_t   rx_wr_index=0, //индекс хвоста буфера(куда писать данные)
                    rx_rd_index=0, //индекс начала буфера (откуда считывать данные)
                    rx_counter=0; //количество данных в буфере 
volatile uint8_t    rx_buffer_overflow=0; //инфа о переполнении буфера

// буфер на прием
#define TX_BUFFER_SIZE 256 //размер буфера
volatile uint8_t   tx_buffer[TX_BUFFER_SIZE];
volatile uint16_t  tx_wr_index=0, //индекс хвоста буфера(куда писать данные)
                   tx_rd_index=0, //индекс начала буфера (откуда считывать данные)
                   tx_counter=0; //количество данных в буфере 



 
 NVIC_InitTypeDef NVIC_InitStructure;
  /* Configure the Priority Group to 2 bits */

 const TPin TX ={GPIOA,2};
 const TPin RX ={GPIOA,3};
 const TPin* Usart1Pin[] = {&TX,&RX};
 


uint8_t get_char(void) //Прием данных
{
uint8_t data; //временная переменная
while (rx_counter==0);  //если данных нет, ждем
data=rx_buffer[rx_rd_index++]; //берем данные из буфера
if (rx_rd_index == RX_BUFFER_SIZE) rx_rd_index=0; //идем по кругу
USART_ITConfig(USART1, USART_IT_RXNE, DISABLE); //запрещаем прерывание
--rx_counter; //чтобы оно не помешало изменить переменную
USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//разрешаем прерывание
return data;
}

void put_char(uint8_t c) //Вывод данных
{
while (tx_counter == TX_BUFFER_SIZE); //если данных нет, ждем
USART_ITConfig(USART1, USART_IT_TXE, DISABLE); //Запрещаем прерывание, чтобы оно не мешало менять переменную
if (tx_counter || (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)) //если в буфере уже что-то есть или что-то передается
  {
  tx_buffer[tx_wr_index++]=c; //то кладем данные в буфер
  if (tx_wr_index == TX_BUFFER_SIZE) tx_wr_index=0; //?идем по кругу
  ++tx_counter; //увеличиваем счетчик данных в буфере
  USART_ITConfig(USART1, USART_IT_TXE, ENABLE); //разрешаем прерывание
  }
else //если UART свободен
  USART_SendData(USART1,c); //передаем данные без прерывания
}

 /*Обработчик прерывания*/
 void USART1_IRQHandler(void)
{ 
   /*------------------прием---------------------------------------*/
  if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET) //прерывание по приему данных
  {
    if ((USART1->ISR & (USART_FLAG_NE|USART_FLAG_FE|USART_FLAG_PE|USART_FLAG_ORE)) == 0) //проверяем флаги ошибок
    {                   
      rx_buffer[rx_wr_index++]= (uint8_t) (USART_ReceiveData(USART1)& 0xFF); //считаем данные в буфер, инкрементируя хвост
      if (rx_wr_index == RX_BUFFER_SIZE) rx_wr_index=0; //проверка на переполнение буфера приема(идем по кругу)
      if (++rx_counter == RX_BUFFER_SIZE) //если переполнение буфера
      {
        rx_counter=0; //начинаем сначало(обнуляем счетчик)
        rx_buffer_overflow=1;  //флаг перепонения
      }
    }
    else USART_ReceiveData(USART1); //обработка ошибок (просто пропускаем ошибочный байт)
  }
   
  if(USART_GetITStatus(USART1, USART_IT_ORE) == SET) //прерывание по переполнению буфера
  {
     USART_ReceiveData(USART1); //обработка ошибок (просто сбрасываем флаг чтением из буфера)
  }
   /*------------------передача---------------------------------------*/
  if(USART_GetITStatus(USART1, USART_IT_TXE) == SET) //проверяем флаг
  {   
    if (tx_counter) //если есть данные
    {
      --tx_counter; // уменьшаем счетчик
      USART_SendData(USART1,tx_buffer[tx_rd_index++]); //передаем данные инкрементируя хвост
      if (tx_rd_index == TX_BUFFER_SIZE) tx_rd_index=0; //идем по кругу
    }
    else //есле нечего передовать запрещаем прерывание по передаче
    {
      USART_ITConfig(USART1, USART_IT_TXE, DISABLE);                    
    }
  }
}
 
 
 
/*******************************************************************/
void UsartInit(void)
{
    
     ConfigVirualPort(Usart1Pin,2,AF,_50MHz,PULLUP,PP); 
     PinAltFunc(&TX,AF1);
     PinAltFunc(&RX,AF1);
 
    // Включаем тактирование
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);


  usart.USART_BaudRate = 19200;// скорость
  usart.USART_WordLength = USART_WordLength_8b; //8 бит данных
  usart.USART_StopBits = USART_StopBits_1; //один стоп бит
  usart.USART_Parity = USART_Parity_No; //четность - нет
  usart.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // управление потоком - нет
  usart.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;       // разрешение приема и передачи
  USART_Init(USART1, &usart);
  
   //NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //конфигурация групп и подгрупп прерываний
  
  /* Enable the USARTx Interrupt */
   NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn; //прерывание по uart2 
   NVIC_InitStructure.NVIC_IRQChannelPriority = 0; //приоритет в группе 
   //NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; //проритет в подгруппе
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //разрешаем прерывание
   NVIC_Init(&NVIC_InitStructure); //инициализация структуры
   
   USART_Cmd(USART1, ENABLE);/* включаем модуль usart1 */
   USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
}
