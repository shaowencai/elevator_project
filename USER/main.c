#include "delay.h"
#include "motor.h"
#include "printer.h"
#include "hmi_process.h"
#include "stmflash.h"

int main(void)
{	   
    delay_init(); 
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    Hmi_init();
    motor_init();
    printer_init();
    while(1)
    {     
        Hmi_App_Run();
    }
}





















//void Run_Print(void);

//int a[3]={0x12345678,0x12,0x12};
//int b[3] ={0};
//#define ADDR    ((0x8000000+0x20000-13)-(0x8000000+0x20000-13)%4)
////    Run_Print();
//b[1]=ADDR; STMFLASH_Read(ADDR,(uint16_t *)b,sizeof(a)/2);
//if(b[0] == 0x12345678)
//{
//STMFLASH_Write(ADDR,(uint16_t *)a,sizeof(a)/2);
//}


//b[0]=b[0];
//b[1] =b[1];
//b[2] = b[2];
//while(1);

