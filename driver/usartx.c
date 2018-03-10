#include "usartx.h"

void USARTx_Init(UsartT *usartT)
{	
#if USART_RX_IT
    NVIC_InitTypeDef NVIC_InitStructure;
#endif
	USART_InitTypeDef USART_InitStructure;

 	USART_DeInit(usartT->Usartx);  
	USART_InitStructure.USART_BaudRate = usartT->Bound;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = usartT->StopBits;
	USART_InitStructure.USART_Parity = usartT->Parity;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
 	USART_Init(usartT->Usartx, &USART_InitStructure); 
	USART_Cmd(usartT->Usartx, ENABLE);              
	
#if USART_RX_IT
    USART_ITConfig(usartT->Usartx, USART_IT_RXNE, ENABLE);
	NVIC_InitStructure.NVIC_IRQChannel = usartT->IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			
	NVIC_Init(&NVIC_InitStructure);	
#endif
}
