#ifndef _HMI_PROCESS_H
#define _HMI_PROCESS_H


#include "hmi_config.h"
#include "hmi_driver.h"
#include "cmd_queue.h"
#include "stdio.h"

/******************************��������*********************************/
#define NOTIFY_TOUCH_PRESS      		0X01  //����������֪ͨ
#define NOTIFY_TOUCH_RELEASE  			0X03  //�������ɿ�֪ͨ
#define NOTIFY_WRITE_FLASH_OK 		    0X0C  //дFLASH�ɹ�
#define NOTIFY_WRITE_FLASH_FAILD  		0X0D  //дFLASHʧ��
#define NOTIFY_READ_FLASH_OK  			0X0B  //��FLASH�ɹ�
#define NOTIFY_READ_FLASH_FAILD  		0X0F  //��FLASHʧ��
#define NOTIFY_MENU                     0X14  //�˵��¼�֪ͨ
#define NOTIFY_TIMER                    0X43  //��ʱ����ʱ֪ͨ
#define NOTIFY_CONTROL                	0XB1  //�ؼ�����֪ͨ
#define NOTIFY_READ_RTC               	0XF7  //��ȡRTCʱ��
/***********************************************************************/

/****************************ctrl_msg***********************************/
#define MSG_GET_CURRENT_SCREEN 			0X01  //����ID�仯֪ͨ
#define MSG_GET_DATA                    0X11  //�ؼ�����֪ͨ
/***********************************************************************/

#define PTR2U16(PTR) ((((uint8 *)(PTR))[0]<<8)|((uint8 *)(PTR))[1])  //�ӻ�����ȡ16λ����
#define PTR2U32(PTR) ((((uint8 *)(PTR))[0]<<24)|(((uint8 *)(PTR))[1]<<16)|(((uint8 *)(PTR))[2]<<8)|((uint8 *)(PTR))[3])  //�ӻ�����ȡ32λ����

enum CtrlType
{
	kCtrlUnknown=0x0,
	kCtrlButton=0x10,  	//��ť
	kCtrlText,  		//�ı�
	kCtrlProgress,  	//������
	kCtrlSlider,    	//������
	kCtrlMeter,  		//�Ǳ�
	kCtrlDropList, 		//�����б�
	kCtrlAnimation,     //����
	kCtrlRTC, 			//ʱ����ʾ
	kCtrlGraph,			//����ͼ�ؼ�
	kCtrlTable, 		//���ؼ�
	kCtrlMenu,			//�˵��ؼ�
	kCtrlSelector,		//ѡ��ؼ�
	kCtrlQRCode,		//��ά��
};

#pragma pack(push)
#pragma pack(1)	//���ֽڶ���

typedef struct
{
    uint8    cmd_head;      //֡ͷ
    
	uint8    cmd_type;  	//��������(UPDATE_CONTROL)	
	uint8    ctrl_msg;   	
	uint16   screen_id;  	//������Ϣ�Ļ���ID
	uint16   control_id;  	//������Ϣ�Ŀؼ�ID
	uint8    control_type; 	//�ؼ�����
	uint8    param[24];	    //�ɱ䳤�Ȳ��������24���ֽ�
    
    uint8    cmd_tail[4];   //֡β
}CTRL_MSG,*PCTRL_MSG;

#pragma pack(pop)


typedef struct
{
    uint8 cmd_buffer[CMD_MAX_SIZE];	 //ָ���
    uint16 current_screen_id ;		//��ǰ����ID
}Hmi_Dev;


void Hmi_init(void);
void Hmi_App_Run(void);

#endif
