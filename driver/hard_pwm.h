#ifndef _HARD_PWM
#define _HARD_PWM

#include "stm32f10x.h"  

typedef struct
{
	TIM_TypeDef *       TIMx; 
    uint8_t             pwm_channel;
}Hard_PWM_def;

#define PWMCHANNL1   1
#define PWMCHANNL2   2
#define PWMCHANNL3   3
#define PWMCHANNL4   4
#define PWM_MAX_DUTY 10000

extern void TIM_PWM_Init(Hard_PWM_def *hard_pwm);
extern void open_pwm(Hard_PWM_def *hard_pwm);
extern void close_pwm(Hard_PWM_def *hard_pwm);
extern void set_pwm_duty(Hard_PWM_def *hard_pwm, uint16_t duty);

#endif
