#include "motor.h"

//stm32f10x IO时钟都是APB2
static Motor_FeedBack motor_feedback =
{
    {GPIO_Speed_50MHz,GPIO_Pin_2,GPIOC,GPIO_Mode_IN_FLOATING,0},//PC2
    {GPIO_PortSourceGPIOC,GPIO_PinSource2,EXTI_Line2,EXTI2_IRQn,EXTI_Trigger_Falling},
    {TIM3,TIM3_IRQn,60000},
    0,
};

static Input_Check Elec_Switch =
{
    {GPIO_Speed_50MHz,GPIO_Pin_4,GPIOC,GPIO_Mode_IN_FLOATING,0},//PC4
    {GPIO_PortSourceGPIOC,GPIO_PinSource4,EXTI_Line4,EXTI4_IRQn,EXTI_Trigger_Falling}, 
    
};

static Input_Check Mach_Switch =
{
    {GPIO_Speed_50MHz,GPIO_Pin_9,GPIOB,GPIO_Mode_IN_FLOATING,0},//PB9
    {GPIO_PortSourceGPIOB,GPIO_PinSource9,EXTI_Line9,EXTI9_5_IRQn,EXTI_Trigger_Falling}, 
};


static Motor_PWM motor_pwm =
{
    {GPIO_Speed_50MHz,GPIO_Pin_6,GPIOB,GPIO_Mode_AF_PP,0},
    {TIM4,PWMCHANNL1}
};

static Motor_Direction motor_dir =
{
    {GPIO_Speed_50MHz,GPIO_Pin_7,GPIOB,GPIO_Mode_Out_PP,0}
};

static Motor_EN motor_en =
{
    {GPIO_Speed_50MHz,GPIO_Pin_8,GPIOB,GPIO_Mode_Out_PP,0}
};


static Motor_Pid motor_pid =
{
    0,      //integral
    200,    //kp
    15,     //ki
    0,      //kd
    0,      //err
    0,      //err_last
    0,      //out
};


static Motor_dev motor1;


void motor_rcc_config()
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);//复用引脚就需要使能AFIO时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); 
}

void motor_init()
{
    motor_rcc_config();

    motor1.feedback = motor_feedback;
    motor1.pwm = motor_pwm;
    motor1.dir = motor_dir;
    motor1.en = motor_en;
    motor1.pid = motor_pid;
    motor1.set_speed=DEFAULT_SET_SPEED;
    
    Pin_Init(&(motor1.feedback.pin));
    Exti_Init(&(motor1.feedback.exti));
    Timerx_Init(&(motor1.feedback.timer));
    
    Pin_Init(&(motor1.pwm.pin));
    TIM_PWM_Init(&(motor1.pwm.hard_pwm));

    Pin_Init(&(motor1.dir.pin));    
    Pin_Init(&(motor1.en.pin));
    
    Pin_Init(&(Elec_Switch.pin));
    Exti_Init(&(Elec_Switch.exti));
    
    Pin_Init(&(Mach_Switch.pin));
    Exti_Init(&(Mach_Switch.exti));

    set_motor_rev_speed(DEFAULT_SET_SPEED);
}

void open_motor()
{
    GPIO_SetBits(motor1.en.pin.GPIOx,motor1.en.pin.pinx);
    open_pwm(&(motor1.pwm.hard_pwm));
    open_timer(&(motor1.feedback.timer));
}

void close_motor()
{
    GPIO_ResetBits(motor1.en.pin.GPIOx,motor1.en.pin.pinx);
    close_pwm(&(motor1.pwm.hard_pwm));
    close_timer(&(motor1.feedback.timer));
}

void set_motor_fwd_speed(uint16_t duty)
{
    set_pwm_duty(&(motor1.pwm.hard_pwm),PWM_MAX_DUTY-duty);
}


void set_motor_rev_speed(uint16_t duty)
{
    set_pwm_duty(&(motor1.pwm.hard_pwm),duty);
}


void motor_fwd()
{
    GPIO_ResetBits(motor1.dir.pin.GPIOx,motor1.dir.pin.pinx);
}

void motor_rev()
{
     GPIO_SetBits(motor1.dir.pin.GPIOx,motor1.dir.pin.pinx);

}
void motor_stop()
{
    GPIO_ResetBits(motor1.dir.pin.GPIOx,motor1.dir.pin.pinx);
    set_pwm_duty(&(motor1.pwm.hard_pwm),0);
    
}

//如果设置速度超过范围就立刻停止，然后记录为异常
//如果设置速度没有超过范围就有信号，那就记录为正常
static uint16_t Max_Speed = 0;
static uint16_t Elec_Speed = 0;
static uint16_t Elec_flag =0;
static uint16_t Mach_flag =0;
float Get_Elec_Speed()
{
    if(Elec_flag == 1)
    {
        Elec_flag = 0;
        return (float)Elec_Speed/1500;
    }
    return 0;
}

float Get_Mach_Speed()
{
    if(Mach_flag == 1)
    {
        Mach_flag = 0;
        return (float)Max_Speed/1500;
    }
    return 0;
}


void Set_Motor_Speed(float speed)//这个函数被动态调用，为确保加速度稳定
{
    motor1.set_speed = (uint16_t)(speed * 1500);//将m/s转换为 r/min
}

 
static void Motor_PID()
{
    motor1.pid.err = motor1.set_speed - motor1.actul_speed;
    motor1.pid.integral +=motor1.pid.err;
    motor1.pid.out = motor1.pid.kp*motor1.pid.err \
                    +motor1.pid.ki*motor1.pid.integral \
                    +motor1.pid.kd*(motor1.pid.err-motor1.pid.err_last);
    motor1.pid.err_last = motor1.pid.err;
    set_motor_rev_speed(motor1.pid.out/1000+DEFAULT_SET_SPEED);   
}


void EXTI9_5_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line9) != RESET)
    {
        Mach_flag = 1;
        motor_stop();
        EXTI_ClearITPendingBit(EXTI_Line9);
    }
}
void EXTI4_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line4) != RESET)
    {
        Elec_flag = 1;
        EXTI_ClearITPendingBit(EXTI_Line4);
    }  
}

void EXTI2_IRQHandler(void)
{
     uint32_t tim;//一圈的时间 单位us
     if(EXTI_GetITStatus(EXTI_Line2) != RESET)
     {
         tim = get_timer_count(&(motor1.feedback.timer))+60000*motor1.feedback.count;
         motor1.actul_speed = (60000000/tim);//转换为每分钟多少圈
         if(Max_Speed <  motor1.actul_speed)Max_Speed = motor1.actul_speed;
         Motor_PID();
         motor1.feedback.count = 0;
         clear_timer_count(&(motor1.feedback.timer));
         EXTI_ClearITPendingBit(EXTI_Line2);
     }
}

void TIM3_IRQHandler(void)   //TIM3中断
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  
    {
        motor1.feedback.count++;
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update); 
    }
}
