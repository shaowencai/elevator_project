#include "Pin.h"

//�ⲿ�жϳ�ʼ������
void Exti_Init(Exti_T *TI)
{	
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    GPIO_EXTILineConfig(TI->GPIO_PortSourceGPIOx,TI->GPIO_PinSourcePinx);
    EXTI_InitStructure.EXTI_Line=TI->line;                        
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	//�ж�ģʽ
    EXTI_InitStructure.EXTI_Trigger = TI->TriggerMode;          //���ô���ģʽ
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);		//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���

    NVIC_InitStructure.NVIC_IRQChannel = TI->IRQn; 
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	    //��ռ���ȼ�
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;			    //�����ȼ�
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
    NVIC_Init(&NVIC_InitStructure);  	  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
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
       //�����������������������Ĭ�ϵ�ƽ����������Ĭ�ϵ�ƽ״̬
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
