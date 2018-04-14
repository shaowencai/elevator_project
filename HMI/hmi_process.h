#ifndef _HMI_PROCESS_H
#define _HMI_PROCESS_H


#include "hmi_config.h"
#include "hmi_driver.h"
#include "cmd_queue.h"
#include "stdio.h"

/******************************命令类型*********************************/
#define NOTIFY_TOUCH_PRESS      		0X01  //触摸屏按下通知
#define NOTIFY_TOUCH_RELEASE  			0X03  //触摸屏松开通知
#define NOTIFY_WRITE_FLASH_OK 		    0X0C  //写FLASH成功
#define NOTIFY_WRITE_FLASH_FAILD  		0X0D  //写FLASH失败
#define NOTIFY_READ_FLASH_OK  			0X0B  //读FLASH成功
#define NOTIFY_READ_FLASH_FAILD  		0X0F  //读FLASH失败
#define NOTIFY_MENU                     0X14  //菜单事件通知
#define NOTIFY_TIMER                    0X43  //定时器超时通知
#define NOTIFY_CONTROL                	0XB1  //控件更新通知
#define NOTIFY_READ_RTC               	0XF7  //读取RTC时间
/***********************************************************************/

/****************************ctrl_msg***********************************/
#define MSG_GET_CURRENT_SCREEN 			0X01  //画面ID变化通知
#define MSG_GET_DATA                    0X11  //控件数据通知
/***********************************************************************/

#define PTR2U16(PTR) ((((uint8 *)(PTR))[0]<<8)|((uint8 *)(PTR))[1])  //从缓冲区取16位数据
#define PTR2U32(PTR) ((((uint8 *)(PTR))[0]<<24)|(((uint8 *)(PTR))[1]<<16)|(((uint8 *)(PTR))[2]<<8)|((uint8 *)(PTR))[3])  //从缓冲区取32位数据

enum CtrlType
{
	kCtrlUnknown=0x0,
	kCtrlButton=0x10,  	//按钮
	kCtrlText,  		//文本
	kCtrlProgress,  	//进度条
	kCtrlSlider,    	//滑动条
	kCtrlMeter,  		//仪表
	kCtrlDropList, 		//下拉列表
	kCtrlAnimation,     //动画
	kCtrlRTC, 			//时间显示
	kCtrlGraph,			//曲线图控件
	kCtrlTable, 		//表格控件
	kCtrlMenu,			//菜单控件
	kCtrlSelector,		//选择控件
	kCtrlQRCode,		//二维码
};

#pragma pack(push)
#pragma pack(1)	//按字节对齐

typedef struct
{
    uint8    cmd_head;      //帧头
    
	uint8    cmd_type;  	//命令类型(UPDATE_CONTROL)	
	uint8    ctrl_msg;   	
	uint16   screen_id;  	//产生消息的画面ID
	uint16   control_id;  	//产生消息的控件ID
	uint8    control_type; 	//控件类型
	uint8    param[24];	    //可变长度参数，最多24个字节
    
    uint8    cmd_tail[4];   //帧尾
}CTRL_MSG,*PCTRL_MSG;

#pragma pack(pop)


typedef struct
{
    uint8 cmd_buffer[CMD_MAX_SIZE];	 //指令缓存
    uint16 current_screen_id ;		//当前画面ID
}Hmi_Dev;


void Hmi_init(void);
void Hmi_App_Run(void);

#endif
