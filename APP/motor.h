#ifndef __MOTOR_H
#define __MOTOR_H

#include "hard_pwm.h"
#include "timer.h"
#include "Pin.h"

#define START_PWM_DUTY      700//��ռ�ձ���10000 ��ʼռ�ձ�
#define DEFAULT_SET_SPEED   1000//Ĭ�������ٶ� 1000r/min

typedef struct
{
   Pin_T   pin;
   Exti_T  exti; 
   TimerT  timer;
   uint16_t count; 
}Motor_FeedBack;

typedef struct
{
   Pin_T   pin;
   Hard_PWM_def hard_pwm;
}Motor_PWM;

typedef struct
{
    Pin_T   pin;
}Motor_Direction;//�����������

typedef struct
{
    Pin_T   pin;
}Motor_EN;//���ʹ������

typedef struct 
{
    int16_t    integral;
    int16_t     kp;
    int16_t     ki;
    int16_t     kd;
    int16_t    err;
    int16_t    err_last;
    int32_t    out;
}Motor_Pid;

typedef struct
{
    Motor_FeedBack  feedback;
    Motor_PWM       pwm;
    Motor_Direction dir; 
    Motor_EN        en;
    Motor_Pid       pid;
    uint16_t        actul_speed;
    uint16_t        set_speed;
}Motor_dev;

extern void motor_init(void);
extern void open_motor(void);
extern void close_motor(void);
extern void set_motor_fwd_speed(uint16_t duty);
extern void set_motor_rev_speed(uint16_t duty);
extern void motor_fwd(void);
extern void motor_rev(void);
extern void motor_stop(void);

#endif
