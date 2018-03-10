#ifndef __USART_H
#define __USART_H	 
#include "stm32f10x.h"

#define USART_RX_IT 1

typedef struct _UsartT
{
	USART_TypeDef         *Usartx;
	uint32_t              Bound;
	uint16_t              StopBits;
	uint16_t              Parity;
#if USART_RX_IT
	IRQn_Type             IRQn;
#endif
}UsartT;

extern void USARTx_Init(UsartT *usartT);	 			    
#endif
