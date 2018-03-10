#include "motor.h"

//stm32f10x IO时钟都是APB2
static Motor_FeedBack motor_feedback =
{
    {GPIO_Speed_50MHz,GPIO_Pin_5,GPIOE,GPIO_Mode_IN_FLOATING,0},
    {GPIO_PortSourceGPIOE,GPIO_PinSource5,EXTI_Line5,EXTI9_5_IRQn,EXTI_Trigger_Falling},
    {TIM3,TIM3_IRQn},
    0,
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
    0,//integral
    200,//kp
    15,//ki
    0,//kd
    0,//err
    0,//err_last
    0,//out
};


////////////////////////////////////////////////////////////
Pin_T  power1 ={GPIO_Speed_50MHz,GPIO_Pin_9,GPIOB,GPIO_Mode_Out_PP,1};
Pin_T  CTR_OE ={GPIO_Speed_50MHz,GPIO_Pin_15,GPIOE,GPIO_Mode_Out_PP,0};
//////////////////////////////////////////////////////////

static Motor_dev motor1;


void motor_rcc_config()
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);//复用引脚就需要使能AFIO时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE);
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
    
    //////////////////////////////
    Pin_Init(&power1);
    Pin_Init(&CTR_OE);
    ///////////////////////////////
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
     uint32_t tim;//一圈的时间 单位us
     tim = get_timer_count(&(motor1.feedback.timer))+60000*motor1.feedback.count;
     motor1.actul_speed = (60000000/tim);//转换为每分钟多少圈
     Motor_PID();
     motor1.feedback.count = 0;
     clear_timer_count(&(motor1.feedback.timer));
	 EXTI_ClearITPendingBit(EXTI_Line5);    
}

void TIM3_IRQHandler(void)   //TIM3中断
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  
    {
        motor1.feedback.count++;
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update); 
    }
}
