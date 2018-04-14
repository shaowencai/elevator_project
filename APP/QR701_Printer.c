#include "Printer.h"


static PrinterT qr701_printer=
{
    {GPIO_Speed_50MHz,GPIO_Pin_2,GPIOA,GPIO_Mode_AF_PP,0},//TX_pin
    {GPIO_Speed_50MHz,GPIO_Pin_3,GPIOA,GPIO_Mode_IN_FLOATING,0},//Rx_pin
    
    {USART2,9600,USART_StopBits_1,USART_Parity_No,
#if USART_RX_IT
        USART2_IRQn
#endif
    }
    
};


static void printer_rcc_config()
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);//复用引脚就需要使能AFIO时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
}

void printer_init(void)
{
    printer_rcc_config();
    
    Pin_Init(&(qr701_printer.Tx_pin));
    Pin_Init(&(qr701_printer.Rx_pin));
    
    USARTx_Init(&(qr701_printer.usart));
}

void SendCharToPrint(u8 ch)
{
    while((qr701_printer.usart.Usartx->SR&0X40)==0);//循环发送,直到发送完毕   
    qr701_printer.usart.Usartx->DR = (u8) ch;  
}

void SendStringToPrint(char *buf)
{
    u16 i=0;
    while(buf[i]!='\0')
    {
        SendCharToPrint(buf[i]);
        i++;
    }
}


