#include "hmi_process.h"
#include <string.h>
#include <stdlib.h>
#include "delay.h"
#include "motor.h"
#include "printer.h"
#include "hmi_process.h"
#include "stdio.h"
#include "stmflash.h"

static Hmi_Dev  hmi_dev; //����HMI�豸 

#define PARAM_SAVE_ADDR   ((0x8000000+0x80000/4-sizeof(elevator))-(0x8000000+0x80000/4-sizeof(elevator))%4)
#define PARAM_HEAD_ADDR   ((PARAM_SAVE_ADDR-sizeof(head))-(PARAM_SAVE_ADDR-sizeof(head))%4)

#pragma pack(push)
#pragma pack(1)	//���ֽڶ���
typedef struct
{
    uint8 year;
    uint8 month;
    uint8 week;
    uint8 day;
    uint8 hour;
    uint8 minute;
    uint8 second;
}DateT;
#pragma pack(pop)


#pragma pack(push)
#pragma pack(4)	//��4�ֽڶ���
typedef struct
{  
    char Register_Code[20]; //ע�����
    char Factory_Numbe[10]; //�������
    DateT Date;
    char Spec_Mode[15];     //����ͺ�
    char Dir;               //��˫���� 0���� 1˫�� //�û�û��ָ��Ĭ��Ϊ����
    char Rated_Speed[8];    //��ٶ�
    char Up_Elec_Speed[8];  //���е����ٶ�       
    char Up_Mach_Speed[8];  //���л�е�ٶ�       
    char Up_Result;         //���н�� 1�쳣 2���� 0��Ч
    char Down_Elec_Speed[8];//���е����ٶ�      
    char Down_Mach_Speed[8];//���л�е�ٶ�       
    char Down_Result;       //���н�� 1�쳣 2���� 0��Ч
}Elevator_ParamT;
#pragma pack(pop)

typedef struct
{
    unsigned int deedbeef;
    unsigned int begin_number;
    unsigned int end_number;
}Elevator_HeadT;



Elevator_ParamT elevator[100] = {0};
Elevator_ParamT elevator_tmp[100] = {0};
Elevator_HeadT  head= {0x12345678,0,0};   
unsigned int  current_number;
unsigned char dir_flag=0;
int Find_flag=-1;//0 ��ע������ѯ  1��������Ų�ѯ  2������ʱ���ѯ
char Match_Register_Code[20];
char Match_Factory_Numbe[10];
char Match_Date[15];
int  find_begin_number = -1;
int  find_cur_begin_number = -1;
int  find_cur_end_number = -1;
int  find_end_number = -1;
float Rated_Speed;
float Max_Speed;
float Min_Speed;
float UpElec_Speed;
float DownElec_Speed;
unsigned int old_Speed;
unsigned char motor_open_flag =0; //0�ر� 1��
unsigned int start_duty;


void Run_Print(void)
{
    char buf[50];
    SendStringToPrint("\r\n");
    SendStringToPrint("------<<���������Ա���>>------\r\n");
    SendStringToPrint("\r\n");
    sprintf(buf,"ע����룺%s\r\n",elevator[current_number].Register_Code);
    SendStringToPrint(buf);
    sprintf(buf,"������ţ�%s\r\n",elevator[current_number].Factory_Numbe);
    SendStringToPrint(buf);
    sprintf(buf,"����ͺţ�%s\r\n",elevator[current_number].Spec_Mode);
    SendStringToPrint(buf);
    if(elevator[current_number].Dir == 0)SendStringToPrint("��˫���򣺵���\r\n");
    else if(elevator[current_number].Dir == 1) SendStringToPrint("��˫����˫��\r\n");
    sprintf(buf,"��ٶȣ�%s(m/s)",elevator[current_number].Rated_Speed);
    SendStringToPrint(buf);
    SendStringToPrint(" [��׼:Vx~Vy]\r\n");
    SendStringToPrint("\r\n");
    SendStringToPrint("���� �����ٶ�V1 ��е�ٶ�V2  ���\r\n");
    SendStringToPrint("����");
    
    if(elevator[current_number].Up_Result == 0)
    {
        SendStringToPrint("     \\          \\       \\\r\n");
    }
    else if(elevator[current_number].Up_Result == 1)
    {
        elevator[current_number].Up_Elec_Speed[4] = 0;
        elevator[current_number].Up_Mach_Speed[4] = 0;
        sprintf(buf,"  %4s(m/s) ",elevator[current_number].Up_Elec_Speed);
        SendStringToPrint(buf);
        sprintf(buf," %4s(m/s)  ",elevator[current_number].Up_Mach_Speed);
        SendStringToPrint(buf);
        SendStringToPrint("�쳣\r\n");
    }
    else if(elevator[current_number].Up_Result == 2)
    {
        elevator[current_number].Up_Elec_Speed[4] = 0;
        elevator[current_number].Up_Mach_Speed[4] = 0;
        sprintf(buf,"  %4s(m/s) ",elevator[current_number].Up_Elec_Speed);
        SendStringToPrint(buf);
        sprintf(buf," %4s(m/s)  ",elevator[current_number].Up_Mach_Speed);
        SendStringToPrint(buf);
        SendStringToPrint("����\r\n");
    }
    SendStringToPrint("����");
    if(elevator[current_number].Down_Result == 0)
    {
        SendStringToPrint("     \\          \\       \\\r\n");
    }
    else if(elevator[current_number].Down_Result == 1)
    {
        elevator[current_number].Down_Elec_Speed[4] = 0;
        elevator[current_number].Down_Mach_Speed[4] = 0;
        sprintf(buf,"  %4s(m/s) ",elevator[current_number].Down_Elec_Speed);
        SendStringToPrint(buf);
        sprintf(buf," %4s(m/s)  ",elevator[current_number].Down_Mach_Speed);
        SendStringToPrint(buf);
        SendStringToPrint("�쳣\r\n");
    }
    else if(elevator[current_number].Down_Result == 2)
    {
        elevator[current_number].Down_Elec_Speed[4] = 0;
        elevator[current_number].Down_Mach_Speed[4] = 0;
        sprintf(buf,"  %4s(m/s) ",elevator[current_number].Down_Elec_Speed);
        SendStringToPrint(buf);
        sprintf(buf," %4s(m/s)  ",elevator[current_number].Down_Mach_Speed);
        SendStringToPrint(buf);
        SendStringToPrint("����\r\n");
    }
    SendStringToPrint("\r\n");
    SendStringToPrint("\r\n");
    sprintf(buf,"------<<20%d-%d-%d %d:%d>>------\r\n",(int)elevator[current_number].Date.year,\
                                                  (int)elevator[current_number].Date.month,\
                                                  (int)elevator[current_number].Date.day,\
                                                  (int)elevator[current_number].Date.hour,\
                                                  (int)elevator[current_number].Date.minute);
    SendStringToPrint(buf);
    SendStringToPrint("\r\n");
    SendStringToPrint("\r\n");
    SendStringToPrint("\r\n");
    SendStringToPrint("\r\n");
}




/*! 
 *  \brief      �����л�֪ͨ
 *  \details    ��ǰ����ı�ʱ(�����GetScreen)��ִ�д˺���
 *  \param      screen_id ��ǰ����ID
 */
static void NotifyScreen(uint16 screen_id)
{
    uchar i=0;
	hmi_dev.current_screen_id = screen_id;//�ڹ��������п��������л�֪ͨ����¼��ǰ����ID
    
    if(screen_id == 0)
    {
        //��������������ʷ����
        for(i=1; i<=5; i++) SetTextValue(1,i,"\0");
        for(i=5; i<=15; i++)SetTextValue(2,i,"\0");
        for(i=1; i<=3; i++) SetTextValue(4,i,"\0");
        for(i=1; i<=8; i++) SetTextValue(5,i,"\0");
        for(i=6; i<=33; i++) SetTextValue(4,i,"\0");
        GraphChannelDataClear(5,14,0);
        GraphChannelDataClear(5,15,0);
        Match_Register_Code[0] = '\0';
        Match_Factory_Numbe[0] = '\0';
        Match_Date[0] = '\0';
        Find_flag = -1;
        find_begin_number = -1;
        find_cur_begin_number = -1;
        find_cur_end_number = -1;
        find_end_number = -1;
        current_number = head.end_number;
    }
}

/*! 
 *  \brief  ���������¼���Ӧ
 *  \param  press 1���´�������3�ɿ�������
 *  \param  x x����
 *  \param  y y����
 */
static void NotifyTouchXY(uint8 press,uint16 x,uint16 y)
{
	
}

#if 0
static void SetTextValueInt32(uint16 screen_id, uint16 control_id,int32 value)
{
	uchar buffer[12] = {0};
	sprintf((char *)buffer,"%ld",value); //������ת��Ϊ�ַ���
	SetTextValue(screen_id,control_id,buffer);
}

static void SetTextValueFloat(uint16 screen_id, uint16 control_id,float value)
{
	uchar buffer[12] = {0};
	sprintf((char *)buffer,"%.1f",value);//�Ѹ�����ת��Ϊ�ַ���(����һλС��)
	SetTextValue(screen_id,control_id,buffer);
}
#endif

int display_delay = 0;
static void UpdateUI(void) 
{
    unsigned char value;
    char buf[10];
    float temp_speed;//��ʱ�ٶȱ���
    unsigned int  i_temp_speed;
    switch (hmi_dev.current_screen_id)
    {
        case 5:
            if(motor_open_flag == 1)
            {
                temp_speed = Get_motor_speed();
                i_temp_speed = temp_speed*100;
                if(i_temp_speed == old_Speed ||i_temp_speed == old_Speed-1 || i_temp_speed== old_Speed+1)
                {
                    if(start_duty < 10000)start_duty = start_duty + 100;
                }
                old_Speed = i_temp_speed;
                display_delay ++;
                if(display_delay != 2)break;
                display_delay = 0;
                if(dir_flag == 0)
                {
                    set_motor_rev_speed(start_duty);
                    temp_speed = Get_motor_speed();
                    sprintf(buf,"%3.2fm/s",temp_speed);
                    SetTextValue(5,1,buf);
                    value = (unsigned char)(temp_speed * 100);
                    GraphChannelDataAdd(5,14,0,&value,1);

                    if(temp_speed < Max_Speed * 1.5)
                    {
                        temp_speed  = Get_Elec_Speed();
                        
                        if(temp_speed > 0.0001) //������� ˵�����ز���0
                        {
                            UpElec_Speed = temp_speed;
                            sprintf(elevator[current_number].Up_Elec_Speed,"%3.2fm/s",temp_speed);
                            SetTextValue(5,2,elevator[current_number].Up_Elec_Speed);
                        }
                        
                        
                        temp_speed  = Get_Mach_Speed();
                        
                        if(temp_speed > 0.0001)
                        {
                            
                            sprintf(elevator[current_number].Up_Mach_Speed,"%3.2fm/s",temp_speed);
                            SetTextValue(5,3,elevator[current_number].Up_Mach_Speed);
                            if(temp_speed > UpElec_Speed || UpElec_Speed <Min_Speed || UpElec_Speed >Max_Speed ||temp_speed<Min_Speed ||temp_speed>Max_Speed)
                            {
                                elevator[current_number].Up_Result = 1;
                                SetTextValue(5,4,"�쳣");
                            }
                            else
                            {
                                elevator[current_number].Up_Result = 2;
                                SetTextValue(5,4,"����");
                            }
                            close_motor();
                            motor_open_flag = 0;
                            value = 0;
                            GraphChannelDataAdd(5,14,0,&value,1);
                        }
                        
                    }
                    else
                    {
                        value = 0;
                        GraphChannelDataAdd(5,14,0,&value,1);
                        close_motor();
                        SetTextValue(5,2,"\\");
                        SetTextValue(5,3,"\\");
                        elevator[current_number].Up_Result = 1;
                        SetTextValue(5,4,"�쳣");
                        motor_open_flag = 0;
                    }
         
                }
                else
                {
                    set_motor_fwd_speed(start_duty);
                    temp_speed = Get_motor_speed();
                    sprintf(buf,"%3.2fm/s",temp_speed);
                    SetTextValue(5,5,buf);
                    value = (unsigned char)(temp_speed * 100);
                    GraphChannelDataAdd(5,15,0,&value,1);
                    
                    if(temp_speed < Max_Speed * 1.5)
                    {
                        temp_speed  = Get_Elec_Speed();
                        
                        if(temp_speed > 0.0001)
                        {
                            DownElec_Speed = temp_speed;
                            sprintf(elevator[current_number].Down_Elec_Speed,"%3.2fm/s",temp_speed);
                            SetTextValue(5,6,elevator[current_number].Down_Elec_Speed);
                        }
                        temp_speed  = Get_Mach_Speed();
                        
                        if(temp_speed > 0.0001)
                        {
                            sprintf(elevator[current_number].Down_Mach_Speed,"%3.2fm/s",temp_speed);
                            SetTextValue(5,7,elevator[current_number].Down_Mach_Speed);
                            if(temp_speed > DownElec_Speed || DownElec_Speed <Min_Speed || DownElec_Speed >Max_Speed ||temp_speed<Min_Speed ||temp_speed>Max_Speed)
                            {
                                elevator[current_number].Down_Result = 1;
                                SetTextValue(5,8,"�쳣");
                            }
                            else
                            {
                                elevator[current_number].Down_Result = 2;
                                SetTextValue(5,8,"����");
                            }
                            value = 0;
                            GraphChannelDataAdd(5,15,0,&value,1);
                            close_motor();
                            motor_open_flag = 0;
                        }
                        
                    }
                    else
                    {
                        value = 0;
                        GraphChannelDataAdd(5,15,0,&value,1);
                        close_motor();
                        SetTextValue(5,6,"\\");
                        SetTextValue(5,7,"\\");
                        elevator[current_number].Down_Result = 1;
                        SetTextValue(5,8,"�쳣");
                        motor_open_flag = 0;
                    }
                }
                
            }
            break;
        default:
            break;
    }
}


void Find(void)
{
    unsigned char i;
    char buf[15];
    find_end_number = -1;
    find_begin_number = -1;
    
    if(Find_flag == 0)
    {
        for(i=0;i<100;i++)
        {
            if(!strcmp(elevator[i].Register_Code,Match_Register_Code))
            {
                find_begin_number = i;
                break;
            }
        }
        if(i == 100) return;
        if(i == 99) {find_end_number = i; return;}
        for(i=find_begin_number+1;i<100;i++)
        {
            if(strcmp(elevator[i].Register_Code,Match_Register_Code))//��ƥ���ֹͣ
            {
                find_end_number = i-1;
                break;
            }
        }
        if(i == 100) find_end_number = 99;//һֱƥ�� �����Ϊ���һ��
     
    } 
    else if(Find_flag == 1)
    {
        for(i=0;i<100;i++)
        {
            if(!strcmp(elevator[i].Factory_Numbe,Match_Factory_Numbe))
            {
                find_begin_number = i;
                break;
            }
        }
        if(i == 100) return;
        if(i == 99) {find_end_number = i; return;}
        for(i=find_begin_number+1;i<100;i++)
        {
            if(strcmp(elevator[i].Factory_Numbe,Match_Factory_Numbe))//��ƥ���ֹͣ
            {
                find_end_number = i-1;
                break;
            }
        }
        if(i == 100) find_end_number = 99;//һֱƥ�� �����Ϊ���һ��
     
    }
    else if(Find_flag == 2)
    {
        for(i=0;i<100;i++)
        {
            sprintf(buf,"20%d.%d.%d",elevator[i].Date.year,\
            elevator[i].Date.month,elevator[i].Date.day);
            if(!strcmp(buf,Match_Date))
            {
                find_begin_number = i;
                break;
            }
        }
        if(i == 100) return;
        if(i == 99) {find_end_number = i; return;}
        for(i=find_begin_number+1;i<100;i++)
        {
            sprintf(buf,"20%d.%d.%d",elevator[i].Date.year,\
            elevator[i].Date.month,elevator[i].Date.day);
            if(strcmp(buf,Match_Date))//��ƥ���ֹͣ
            {
                find_end_number = i-1;
                break;
            }
        }
        if(i == 100) find_end_number = 99;//һֱƥ�� �����Ϊ���һ��
    }
    else if(Find_flag == -1) //�ղ� ��ʾ����
    {
        if(head.end_number > head.begin_number)
        {
            find_end_number = head.end_number-1;
            find_begin_number = head.begin_number;
        }
        else if(head.begin_number - head.end_number == 1)
        {
            find_end_number = 99;
            find_begin_number = 0;            
        }
    }

}
void List_Show(void)//���²�ѯ��̬����
{
    unsigned char i;
    char buf[15];
    for(i=0;i<(find_cur_end_number-find_cur_begin_number+1);i++)
    {
        sprintf(buf,"20%d.%d.%d",elevator[find_cur_begin_number+i].Date.year,\
                elevator[find_cur_begin_number+i].Date.month,elevator[find_cur_begin_number+i].Date.day);
        SetTextValue(4,6+i*7,buf);//����ʱ��
        SetTextValue(4,7+i*7,elevator[find_cur_begin_number+i].Register_Code);//ע�����
        SetTextValue(4,8+i*7,elevator[find_cur_begin_number+i].Factory_Numbe);
        SetTextValue(4,9+i*7,elevator[find_cur_begin_number+i].Spec_Mode);
        if(elevator[find_cur_begin_number+i].Dir == 0)
        {
            SetTextValue(4,10+i*7,"����");
        }
        else if(elevator[find_cur_begin_number+i].Dir == 1)
        {
            SetTextValue(4,10+i*7,"˫��");
        }
        SetTextValue(4,11+i*7,elevator[find_cur_begin_number+i].Rated_Speed);
        if(elevator[find_cur_begin_number+i].Up_Result != 1 \
            &&elevator[find_cur_begin_number+i].Down_Result != 1)
        {
            if(elevator[find_cur_begin_number+i].Up_Result == 0 \
            &&elevator[find_cur_begin_number+i].Down_Result == 0)
            {
                SetTextValue(4,12+i*7,"\\");
            }
            else
            {
                SetTextValue(4,12+i*7,"����"); 
            }
            
        }
        else
        {
           SetTextValue(4,12+i*7,"�쳣"); 
        }
        
    }
}

void Show_screen_2(void)
{
    SetTextValue(2,5,elevator[current_number].Register_Code);//ע�����
    SetTextValue(2,6,elevator[current_number].Factory_Numbe);
    SetTextValue(2,7,elevator[current_number].Spec_Mode);
    if(elevator[current_number].Dir == 0)
        SetTextValue(2,8,"����");
    else if(elevator[current_number].Dir == 1)
        SetTextValue(2,8,"˫��");
    SetTextValue(2,9,elevator[current_number].Rated_Speed);

    if(elevator[current_number].Up_Result == 0)
    {
        SetTextValue(2,10,"\\");
        SetTextValue(2,11,"\\");
        SetTextValue(2,12,"\\");
    }
    else if(elevator[current_number].Up_Result == 2)
    {
        SetTextValue(2,10,elevator[current_number].Up_Elec_Speed);
        SetTextValue(2,11, elevator[current_number].Up_Mach_Speed);
        SetTextValue(2,12,"����");
    }
    else 
    {
        SetTextValue(2,10,"\\");
        SetTextValue(2,11,"\\");                            
        SetTextValue(2,12,"�쳣");
    }                            

    if(elevator[current_number].Down_Result == 0)
    {
        SetTextValue(2,13,"\\");
        SetTextValue(2,14,"\\");
        SetTextValue(2,15,"\\");

    }
    else if(elevator[current_number].Down_Result == 2)
    {
        SetTextValue(2,13,elevator[current_number].Down_Elec_Speed);
        SetTextValue(2,14,elevator[current_number].Down_Mach_Speed);                           
        SetTextValue(2,15,"����");
    }
    else
    {
        SetTextValue(2,13,"\\");
        SetTextValue(2,14,"\\");
        SetTextValue(2,15,"�쳣");
    }                                              
}

/*! 
 *  \brief      ��ť�ؼ�֪ͨ
 *  \details    ����ť״̬�ı�(�����GetControlValue)ʱ��ִ�д˺���
 *  \param      screen_id ����ID
 *  \param      control_id �ؼ�ID
 *  \param      state ��ť״̬��0����1����
 */

static void NotifyButton(uint16 screen_id, uint16 control_id, uint8  state)
{
    unsigned char value = 0; //�͵�����ʾ��ʼ���ٶ�0
    unsigned char i = 0;
	switch (screen_id)
    {
        case 2:
            switch(control_id)
            {
                case 3:
                    Run_Print();
                    break;
            }
            break;
        case 4:
            switch(control_id)
            {
                case 4:
                    find_begin_number = -1;
                    find_cur_begin_number = -1;
                    find_cur_end_number = -1;
                    find_end_number = -1;
                    for(i=6; i<=33; i++) SetTextValue(4,i,"\0");
                    Find();
                    Find_flag = -1;//������֮��ͽ���־�ָ�Ĭ��
                    if(find_begin_number != -1)
                    {
                        find_cur_begin_number = find_begin_number;
                        find_cur_end_number = find_cur_begin_number+3;
                        if(find_cur_end_number > find_end_number)
                        {
                            find_cur_end_number = find_end_number;
                        }
                        List_Show();
                    }
                    break;
                case 34:
                    if(find_begin_number != -1)//˵���ҵ���
                        current_number = find_cur_begin_number;
                    Show_screen_2();
                    break;
                case 35:
                    if(find_begin_number != -1)
                    {
                        if(find_cur_end_number-find_cur_begin_number>=1)
                            current_number = find_cur_begin_number+1;
                        else
                            current_number = find_cur_begin_number;
                    }
                    Show_screen_2();
                    break;
                case 36:
                    if(find_begin_number != -1)
                    {
                        if(find_cur_end_number-find_cur_begin_number>=2)
                            current_number = find_cur_begin_number+2;
                        else if(find_cur_end_number-find_cur_begin_number>=1)
                            current_number = find_cur_begin_number+1;
                        else
                            current_number = find_cur_begin_number;
                    }
                    Show_screen_2();
                    break;
                case 37:
                    if(find_begin_number != -1)
                    {
                        if(find_cur_end_number-find_cur_begin_number>=3)
                            current_number = find_cur_begin_number+3;
                        else if(find_cur_end_number-find_cur_begin_number>=2)
                            current_number = find_cur_begin_number+2;
                        else if(find_cur_end_number-find_cur_begin_number>=2)
                            current_number = find_cur_begin_number+1;
                        else
                            current_number = find_cur_begin_number;
                    }   
                    Show_screen_2();
                    break;
                case 38://����
                    if(find_begin_number != -1)
                    {
                        if(find_cur_end_number != find_end_number)
                        {
                            find_cur_begin_number++;
                            find_cur_end_number++;
                        }
                        List_Show();
                    }
                    break;
                case 39://����
                    if(find_begin_number != -1)
                    {
                        if(find_cur_begin_number != find_begin_number)
                        {
                            find_cur_begin_number--;
                            find_cur_end_number--;
                        }
                        List_Show();
                    }
                    break;
                default:
                    break;
            }
            break;
        case 5:
            switch(control_id)
            {
                case 9:
                    if(state == 1)
                    {
                        for(i=1; i<=8; i++) SetTextValue(5,i,"\0");
                        delay_ms(1000);
                        delay_ms(1000);
                        
                        old_Speed = 0;
                        motor_open_flag = 1;
                        start_duty = (unsigned int)(Rated_Speed * 10000);

                        if(dir_flag == 0)
                        {
                            motor_rev();
                            set_motor_rev_speed(8000);
                        }
                        else
                        {
                            motor_fwd();
                            set_motor_fwd_speed(8000);
                        }
                        open_motor(); 
                        delay_ms(10);//�õ��������
                        if(dir_flag == 0)
                        {
                            motor_rev();
                            set_motor_rev_speed(start_duty);
                            GraphChannelDataClear(5,14,0);
                            GraphChannelDataAdd(5,14,0,&value,1);  
                        }
                        else
                        {
                            motor_fwd();
                            set_motor_fwd_speed(start_duty);
                            GraphChannelDataClear(5,15,0);
                            GraphChannelDataAdd(5,15,0,&value,1);
                        }
                    }
                    else
                    {
                        close_motor();
                        motor_open_flag = 0;
                    }
                    break;
                case 10:
                    STMFLASH_Write(PARAM_SAVE_ADDR,(uint16_t *)&elevator[0],sizeof(elevator[0])/2*100);
                    head.end_number = current_number;
                    if(head.end_number == 99 && head.begin_number == 0)
                    {
                        head.end_number = 0;
                        head.begin_number = 1;
                    }
                    else if(head.end_number < head.begin_number)
                    {
                        head.end_number++;
                        head.begin_number++;
                        if( head.begin_number == 100) head.begin_number = 0;
                    }
                    else
                    {
                        head.end_number++;
                    }
                    STMFLASH_Write(PARAM_HEAD_ADDR,(uint16_t *)&head,sizeof(head)/2);
                    break;
                case 11:
                    Run_Print();
                    break;
                case 12:
                    motor_rev();
                    dir_flag = 0;
                    break;
                case 13:
                    motor_fwd();
                    dir_flag = 1;
                    break;
            }           
            break;
        default:
            break;
        
    }
}

/*! 
 *  \brief      �ı��ؼ�֪ͨ
 *  \details    ���ı�ͨ�����̸���(�����GetControlValue)ʱ��ִ�д˺���
 *  \param      screen_id ����ID
 *  \param      control_id �ؼ�ID
 *  \param      str �ı��ؼ�����
 */
static void NotifyText(uint16 screen_id, uint16 control_id, uint8 *str)
{
    unsigned int len = strlen((const char *)str);
	switch (screen_id)
    {
        case 1:
            switch(control_id)
            {
                case 1:
                    memcpy(elevator[current_number].Register_Code,(const char *)str,len);
                    elevator[current_number].Register_Code[len] = '\0';
                    break;
                case 2:
                    memcpy(elevator[current_number].Factory_Numbe,(const char *)str,len);
                    elevator[current_number].Factory_Numbe[len] = '\0';
                    break;
                case 3:
                    memcpy(elevator[current_number].Spec_Mode,(const char *)str,len);
                    elevator[current_number].Spec_Mode[len] = '\0';
                    break;
                case 5:
                    memcpy(elevator[current_number].Rated_Speed,(const char *)str,len);
                    elevator[current_number].Rated_Speed[len] = '\0';
                    Rated_Speed = atof(elevator[current_number].Rated_Speed);
                    if(Rated_Speed < 1.01)
                    {
                        Min_Speed = 1.15 * Rated_Speed;
                        Max_Speed = 1.5 * Rated_Speed;
                    }
                    else 
                    {
                        Min_Speed = 1.15 * Rated_Speed;
                        Max_Speed = 1.25 * Rated_Speed + 0.25/Rated_Speed;
                    }
                    
                break;
                default:
                    break;
            }
            break;
        case 4:
            switch(control_id)
            {
                case 1:
                    if(len != 0)
                    {
                        memcpy(Match_Register_Code,(const char *)str,len);
                        Match_Register_Code[len] = '\0';
                        Find_flag = 0;
                    }
                    else if(Find_flag == 0) Find_flag = -1;
                    break;
                case 2:
                    if(len != 0)
                    {
                        memcpy(Match_Factory_Numbe,(const char *)str,len);
                        Match_Factory_Numbe[len] = '\0';
                        Find_flag = 1;
                    }
                    else if(Find_flag == 1) Find_flag = -1;
                    break;
                case 3:
                    if(len != 0)
                    {
                        memcpy(Match_Date,(const char *)str,len);
                        Match_Date[len] = '\0';
                        Find_flag = 2;
                    }
                    else if(Find_flag == 2) Find_flag = -1;
                    break;
                default:
                    break;
                  
            }
            break;
        default:
            break;
        
    }
}

/*! 
 *  \brief      �������ؼ�֪ͨ
 *  \details    ����GetControlValueʱ��ִ�д˺���
 *  \param      screen_id ����ID
 *  \param      control_id �ؼ�ID
 *  \param      value ֵ
 */
static void NotifyProgress(uint16 screen_id, uint16 control_id, uint32 value)
{

}

/*! 
 *  \brief      �������ؼ�֪ͨ
 *  \details    ���������ı�(�����GetControlValue)ʱ��ִ�д˺���
 *  \param      screen_id ����ID
 *  \param      control_id �ؼ�ID
 *  \param      value ֵ
 */
static void NotifySlider(uint16 screen_id, uint16 control_id, uint32 value)
{

}

/*! 
 *  \brief      �Ǳ�ؼ�֪ͨ
 *  \details    ����GetControlValueʱ��ִ�д˺���
 *  \param      screen_id ����ID
 *  \param      control_id �ؼ�ID
 *  \param      value ֵ
 */
static void NotifyMeter(uint16 screen_id, uint16 control_id, uint32 value)
{
	
}


 
/*! 
 *  \brief      �˵��ؼ�֪ͨ
 *  \details    ���˵���»��ɿ�ʱ��ִ�д˺���
 *  \param      screen_id ����ID
 *  \param      control_id �ؼ�ID
 *  \param      item �˵�������
 *  \param      state ��ť״̬��0�ɿ���1����
 */
static void NotifyMenu(uint16 screen_id, uint16 control_id, uint8  item, uint8  state)
{
    if(screen_id == 1)
    {
        if(control_id == 9)elevator[current_number].Dir = item;
    }
    
}

/*! 
 *  \brief      ѡ��ؼ�֪ͨ
 *  \details    ��ѡ��ؼ��仯ʱ��ִ�д˺���
 *  \param      screen_id ����ID
 *  \param      control_id �ؼ�ID
 *  \param      item ��ǰѡ��
 */
static void NotifySelector(uint16 screen_id, uint16 control_id, uint8  item)
{
       
}

/*! 
 *  \brief  ��ʱ����ʱ֪ͨ����
 *  \param  screen_id ����ID
 *  \param  control_id �ؼ�ID
 */
static void NotifyTimer(uint16 screen_id, uint16 control_id)
{
	
}

/*! 
 *  \brief  ��ȡ�û�FLASH״̬����
 *  \param  status 0ʧ�ܣ�1�ɹ�
 *  \param  _data ��������
 *  \param  length ���ݳ���
 */
static void NotifyReadFlash(uint8 status,uint8 *_data,uint16 length)
{
	
}

/*! 
 *  \brief  д�û�FLASH״̬����
 *  \param  status 0ʧ�ܣ�1�ɹ�
 */
static void NotifyWriteFlash(uint8 status)
{
	
}


/*! 
 *  \brief  ��ȡRTCʱ�䣬ע�ⷵ�ص���BCD��
 *  \param  year �꣨BCD��
 *  \param  month �£�BCD��
 *  \param  week��BCD��
 *  \param  day �գ�BCD��
 *  \param  hour ʱ��BCD��
 *  \param  minute �֣�BCD��
 *  \param  second �루BCD��
 */

static void NotifyReadRTC(DateT *Date)
{
    elevator[current_number].Date.year = Date->year/16*10+Date->year%16;
    elevator[current_number].Date.month = Date->month/16*10+Date->month%16;
    elevator[current_number].Date.day  = Date->day/16*10+Date->day%16;
    elevator[current_number].Date.hour = Date->hour/16*10+Date->hour%16;
    elevator[current_number].Date.minute = Date->minute/16*10+Date->minute%16;
    elevator[current_number].Date.second = Date->second/16*10+Date->second%16;
    elevator[current_number].Date.week = Date->week/16*10+Date->week%16;
}



////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

/*! 
 *  \brief  ��Ϣ��������
 *  \param  msg ��������Ϣ
 *  \param  size ��Ϣ����
 */
static void ProcessMessage( PCTRL_MSG msg, uint16 size )
{
	uint8 cmd_type = msg->cmd_type;					//ָ������
	uint8 ctrl_msg = msg->ctrl_msg;   				//��Ϣ������
	uint8 control_type = msg->control_type;			//�ؼ�����
	uint16 screen_id = PTR2U16(&msg->screen_id);	//����ID
	uint16 control_id = PTR2U16(&msg->control_id);	//�ؼ�ID
	uint32 value = PTR2U32(msg->param);				//��ֵ
    
	switch(cmd_type)
	{		
		case NOTIFY_TOUCH_PRESS:					//����������
		case NOTIFY_TOUCH_RELEASE:					//�������ɿ�
			NotifyTouchXY(hmi_dev.cmd_buffer[1],PTR2U16(hmi_dev.cmd_buffer+2),\
                          PTR2U16(hmi_dev.cmd_buffer+4));
			break;	
		case NOTIFY_WRITE_FLASH_OK:					//дFLASH�ɹ�
			NotifyWriteFlash(1);
			break;
		case NOTIFY_WRITE_FLASH_FAILD:				//дFLASHʧ��
			NotifyWriteFlash(0);
			break;
		case NOTIFY_READ_FLASH_OK:					//��ȡFLASH�ɹ�
			NotifyReadFlash(1,hmi_dev.cmd_buffer+2,size-6);
			break;
		case NOTIFY_READ_FLASH_FAILD:				//��ȡFLASHʧ��
			NotifyReadFlash(0,0,0);
			break;
		case NOTIFY_READ_RTC:						//��ȡRTCʱ��
			NotifyReadRTC((DateT *)&hmi_dev.cmd_buffer[2]);
			break;
		case NOTIFY_CONTROL:
		{
			if(ctrl_msg==MSG_GET_CURRENT_SCREEN)	//����ID�仯֪ͨ
			{
				NotifyScreen(screen_id);
			}
			else
			{
				switch(control_type)
				{
					case kCtrlButton: 				//��ť�ؼ�
						NotifyButton(screen_id,control_id,msg->param[1]);
						break;
					case kCtrlText:					//�ı��ؼ�
						NotifyText(screen_id,control_id,msg->param);
						break;
					case kCtrlProgress: 			//�������ؼ�
						NotifyProgress(screen_id,control_id,value);
						break;
					case kCtrlSlider: 				//�������ؼ�
						NotifySlider(screen_id,control_id,value);
						break;
					case kCtrlMeter: 				//�Ǳ�ؼ�
						NotifyMeter(screen_id,control_id,value);
						break;
					case kCtrlMenu:					//�˵��ؼ�
						NotifyMenu(screen_id,control_id,msg->param[0],msg->param[1]);
						break;
					case kCtrlSelector:				//ѡ��ؼ�
						NotifySelector(screen_id,control_id,msg->param[0]);
						break;
					case kCtrlRTC:					//����ʱ�ؼ�
						NotifyTimer(screen_id,control_id);
						break;
					default:
						break;
				}
			}	
			break;		
		}	
		default:
		{
			break;
		}
        
	}
    
}



void Hmi_init(void)
{
    uint16 i=0;
    Elevator_HeadT head_temp;
    for(i=0;i<CMD_MAX_SIZE;i++)
    {
        hmi_dev.cmd_buffer[i] = 0;
    }
    hmi_dev.current_screen_id = 0;
    queue_reset();
    hmi_driver_init(UpdateUI);//��ʵ�ʸ��º����������󶨣�ͨ���ײ㶨ʱ������Եĵ���
    
    STMFLASH_Read(PARAM_HEAD_ADDR,(uint16_t *)&head_temp,sizeof(head_temp)/2);
    if(head_temp.deedbeef == 0x12345678)
    {
       head.begin_number = head_temp.begin_number;
       head.end_number = head_temp.end_number; 
       if(head.end_number > head.begin_number)
       {
           STMFLASH_Read(PARAM_SAVE_ADDR,(uint16_t *)elevator,sizeof(elevator[0])*head.end_number/2);
       }
       else
       {
           STMFLASH_Read(PARAM_SAVE_ADDR,(uint16_t *)elevator,sizeof(elevator[0])*50);
       }
    }
    current_number = head.end_number;
}
    

void Hmi_App_Run(void)
{
	qsize  size = 0;
    
	size = queue_find_cmd(hmi_dev.cmd_buffer,CMD_MAX_SIZE); //�ӻ������л�ȡһ��ָ��        
	if(size>0)//���յ�ָ��
	{
		ProcessMessage((PCTRL_MSG)hmi_dev.cmd_buffer, size);//ָ���
	}
 
}
