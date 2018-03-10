#include "stdio.h"
#include "delay.h"
#include "motor.h"
#include "printer.h"
#include "hmi_process.h"


int main(void)
{	 
    //int i=0,j=0;
    //char buf[100];
    
    delay_init(); 
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    
    Hmi_init();
//    motor_init();
//    open_motor();
//    motor_rev();
//    printer_init();
//    SendStringToPrint("\r\n");
//    SendStringToPrint("  ----限速器测试报告(手持)----\r\n");
//    SendStringToPrint("  用户:\r\n");
//    SendStringToPrint("  电梯制造商:\r\n");
//    SendStringToPrint("  电梯型号:\r\n");
//    SendStringToPrint("  限速器制造商:\r\n");
//    SendStringToPrint("  型号编号:\r\n");
//    sprintf(buf,"  额定速度:%4.3f(m/s)\r\n",1.75);
//    SendStringToPrint(buf);
//    sprintf(buf,"  合格范围:%4.3f-%4.3f(m/s)\r\n",2.013,2.333);
//    SendStringToPrint(buf);
//    SendStringToPrint("  V1:电气    V2:机械\r\n");
//    SendStringToPrint("  序号 V1(m/s) V2(m/s) 方向 结果\r\n");
//    SendStringToPrint("\r\n");
//    SendStringToPrint("\r\n");
//    SendStringToPrint("\r\n");
//    SendStringToPrint("\r\n");


//    位图打印
//    SendCharToPrint(0x1D);
//    SendCharToPrint(0x76);
//    SendCharToPrint(0x30);
//    SendCharToPrint(0x00);
//    SendCharToPrint(0x10);
//    SendCharToPrint(0x00);
//    SendCharToPrint(0x10);
//    SendCharToPrint(0x00);
//    for(i=0;i<16;i++)
//    {
//         SendCharToPrint(0x70);
//         SendCharToPrint(0x70);
//         SendCharToPrint(0x70);
//         SendCharToPrint(0x70);
//         SendCharToPrint(0x70);
//         SendCharToPrint(0x70);
//         SendCharToPrint(0x70);
//         SendCharToPrint(0x70);
//         SendCharToPrint(0x70);
//         SendCharToPrint(0xf0);
//         SendCharToPrint(0xf0);
//         SendCharToPrint(0xf0);
//         SendCharToPrint(0xf0);
//         SendCharToPrint(0xf0);
//         SendCharToPrint(0xf0);
//         SendCharToPrint(0xf0);
//    }     
//    SendStringToPrint("\r\n");
//    SendStringToPrint("\r\n");
//    SendStringToPrint("\r\n");
//    SendStringToPrint("\r\n"); 

    while(1)
    {     
        Hmi_App_Run();
    }
}


