#ifndef __TIMER_H
#define __TIMER_H

#include "stm32f10x.h"  

typedef struct
{
    TIM_TypeDef     *TIMx;
    IRQn_Type       TIMx_IRQn;
    
}TimerT;


extern void Timerx_Init(TimerT *timerT);
extern void open_timer(TimerT *timerT);
extern void close_timer(TimerT *timerT);
extern void clear_timer_count(TimerT *timerT);
extern uint16_t get_timer_count(TimerT *timerT);

#endif
