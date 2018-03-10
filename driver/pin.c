#include "Pin.h"

//外部中断初始化函数
void Exti_Init(Exti_T *TI)
{	
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    GPIO_EXTILineConfig(TI->GPIO_PortSourceGPIOx,TI->GPIO_PinSourcePinx);
    EXTI_InitStructure.EXTI_Line=TI->line;                        
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	//中断模式
    EXTI_InitStructure.EXTI_Trigger = TI->TriggerMode;          //配置触发模式
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);		//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器

    NVIC_InitStructure.NVIC_IRQChannel = TI->IRQn; 
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	    //抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;			    //子优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
    NVIC_Init(&NVIC_InitStructure);  	  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
}



void Pin_Init(Pin_T *pin)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Speed = pin->GPIO_Speed;
    GPIO_InitStructure.GPIO_Pin = pin->pinx;
    GPIO_InitStructure.GPIO_Mode = pin->mode;
    GPIO_Init(pin->GPIOx, &GPIO_InitStructure);
    if (pin->mode == GPIO_Mode_Out_PP)
    {
       //如果引脚是推挽输出，则根据默认电平属性来设置默认电平状态
       if (pin->def_value)
	   {
		  GPIO_SetBits(pin->GPIOx, pin->pinx); 
	   }
       else
	   {
          GPIO_ResetBits(pin->GPIOx, pin->pinx);
	   }
    }
}
