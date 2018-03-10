#ifndef __PIN_H
#define __PIN_H	 

#include "stm32f10x.h"
typedef struct
{
    uint8_t               GPIO_PortSourceGPIOx;
    uint8_t	              GPIO_PinSourcePinx;
    uint32_t              line;
    IRQn_Type             IRQn;
    EXTITrigger_TypeDef	  TriggerMode;
}Exti_T;

typedef struct
{
	GPIOSpeed_TypeDef   GPIO_Speed;
    uint16_t 	        pinx;
    GPIO_TypeDef 	    *GPIOx;        
    GPIOMode_TypeDef    mode;
    uint8_t 		    def_value;
} Pin_T;

extern void Pin_Init(Pin_T *pin);
extern void Exti_Init(Exti_T *TI);
#endif
