#include "hard_pwm.h"
 

void TIM_PWM_Init(Hard_PWM_def *hard_pwm)
{  
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	TIM_TimeBaseStructure.TIM_Period = PWM_MAX_DUTY; 
	TIM_TimeBaseStructure.TIM_Prescaler = 10; 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
	TIM_TimeBaseInit(hard_pwm->TIMx, &TIM_TimeBaseStructure);
	
    
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; 
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 
    
    switch (hard_pwm->pwm_channel)
	{
	    case PWMCHANNL1:
	        TIM_OC1Init(hard_pwm->TIMx, &TIM_OCInitStructure);  
            TIM_OC1PreloadConfig(hard_pwm->TIMx, TIM_OCPreload_Enable); 
	        break;
	    case PWMCHANNL2:
            TIM_OC2Init(hard_pwm->TIMx, &TIM_OCInitStructure);  
            TIM_OC2PreloadConfig(hard_pwm->TIMx, TIM_OCPreload_Enable);  
	        break;
	    case PWMCHANNL3:
            TIM_OC3Init(hard_pwm->TIMx, &TIM_OCInitStructure);  
            TIM_OC3PreloadConfig(hard_pwm->TIMx, TIM_OCPreload_Enable); 
	        break;
	    case PWMCHANNL4:
            TIM_OC4Init(hard_pwm->TIMx, &TIM_OCInitStructure);  
            TIM_OC4PreloadConfig(hard_pwm->TIMx, TIM_OCPreload_Enable); 
	        break;
        default:
	        break;
	}
	TIM_Cmd(hard_pwm->TIMx,DISABLE);
}

void open_pwm(Hard_PWM_def *hard_pwm)
{
    TIM_Cmd(hard_pwm->TIMx,ENABLE);
}

void close_pwm(Hard_PWM_def *hard_pwm)
{
    TIM_Cmd(hard_pwm->TIMx,DISABLE);
}

void set_pwm_duty(Hard_PWM_def *hard_pwm ,uint16_t duty)
{
    if(duty > PWM_MAX_DUTY)
    {
        return;
    }
	switch (hard_pwm->pwm_channel)
	{
	    case PWMCHANNL1:
	        hard_pwm->TIMx->CCR1 = duty ;
	        break;
	    case PWMCHANNL2:
	        hard_pwm->TIMx->CCR2 = duty;
	        break;
	    case PWMCHANNL3:
            hard_pwm->TIMx->CCR3 = duty;
	        break;
	    case PWMCHANNL4:
            hard_pwm->TIMx->CCR4 = duty;
	        break;
        default:
	        break;
	}
}
