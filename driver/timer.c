#include "timer.h"


void Timerx_Init(TimerT *timerT)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	TIM_TimeBaseStructure.TIM_Period = 60000; 
	TIM_TimeBaseStructure.TIM_Prescaler =(72-1);//1us记一次数
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); 
 
	TIM_ITConfig(TIM3, TIM_IT_Update|TIM_IT_Trigger,ENABLE);
	NVIC_InitStructure.NVIC_IRQChannel = timerT->TIMx_IRQn;   
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure);  

	TIM_Cmd(timerT->TIMx, DISABLE);  							 
}

void open_timer(TimerT *timerT)
{
    TIM_Cmd(timerT->TIMx, ENABLE);
}

void close_timer(TimerT *timerT)
{
    TIM_Cmd(timerT->TIMx, DISABLE);
}

void clear_timer_count(TimerT *timerT)
{
    TIM_SetCounter(timerT->TIMx,0);
}

uint16_t get_timer_count(TimerT *timerT)
{
    return TIM_GetCounter(timerT->TIMx);
}
