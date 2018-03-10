#include "hmi_process.h"


static uint8 cmd_buffer[CMD_MAX_SIZE];	    //指令缓存
static uint16 current_screen_id = 0;		//当前画面ID
static int32 test_value = 0;               //测试值
static uint32 update_en = 0;                //更新标记



/*! 
 *  \brief  画面切换通知
 *  \details  当前画面改变时(或调用GetScreen)，执行此函数
 *  \param screen_id 当前画面ID
 */
static void NotifyScreen(uint16 screen_id)
{
	current_screen_id = screen_id;//在工程配置中开启画面切换通知，记录当前画面ID

	if(current_screen_id==4)//温度曲线
	{
		uint16 i = 0;
		uint8 dat[100] = {0};

		//生成方波
		for (i=0;i<100;++i)
		{
			if((i%20)>=10)
				dat[i] = 200;
			else
				dat[i] = 20;
		}
		GraphChannelDataAdd(4,1,0,dat,100);//添加数据到通道0

		//生成锯齿波
		for (i=0;i<100;++i)
		{
			dat[i] = 16*(i%15);
		}
		GraphChannelDataAdd(4,1,1,dat,100);//添加数据到通道1
	}
	else if(current_screen_id==9)//二维码
	{
		//二维码控件显示中文字符时，需要转换为UTF8编码，
		//通过“指令助手”，转换“广州大彩123” ，得到字符串编码如下
		uint8 dat[] = {0xE5,0xB9,0xBF,0xE5,0xB7,0x9E,0xE5,0xA4,0xA7,0xE5,0xBD,0xA9,0x31,0x32,0x33};
		SetTextValue(9,1,dat);
	}
}

/*! 
 *  \brief  触摸坐标事件响应
 *  \param press 1按下触摸屏，3松开触摸屏
 *  \param x x坐标
 *  \param y y坐标
 */
static void NotifyTouchXY(uint8 press,uint16 x,uint16 y)
{
	//TODO: 添加用户代码
}

static void SetTextValueInt32(uint16 screen_id, uint16 control_id,int32 value)
{
	uchar buffer[12] = {0};
	sprintf((char *)buffer,"%ld",value); //把整数转换为字符串
	SetTextValue(screen_id,control_id,buffer);
}

static void SetTextValueFloat(uint16 screen_id, uint16 control_id,float value)
{
	uchar buffer[12] = {0};
	sprintf((char *)buffer,"%.1f",value);//把浮点数转换为字符串(保留一位小数)
	SetTextValue(screen_id,control_id,buffer);
}

static void UpdateUI()
{
	if(current_screen_id==2)//文本设置和显示
	{
		//当前电流、温度从0到100循环显示，艺术字从0.0-99.9循环显示
		SetTextValueInt32(2,5,test_value%100);//当前电流
		SetTextValueInt32(2,6,test_value%100);//温度
		SetTextValueFloat(2,7,(test_value%1000)/10.0);//艺术字

		++test_value;
	}
	else if(current_screen_id==5)//进度条和滑块控制
	{
		SetProgressValue(5,1,test_value%100);

		++test_value;
	}
	else if(current_screen_id==6)//仪表控件
	{
		SetMeterValue(6,1,test_value%360);
		SetMeterValue(6,2,test_value%360);

		++test_value;
	}
}

/*! 
 *  \brief  按钮控件通知
 *  \details  当按钮状态改变(或调用GetControlValue)时，执行此函数
 *  \param screen_id 画面ID
 *  \param control_id 控件ID
 *  \param state 按钮状态：0弹起，1按下
 */
static void NotifyButton(uint16 screen_id, uint16 control_id, uint8  state)
{
	//TODO: 添加用户代码
	if(screen_id==3)//按钮、图标、动画控制
	{
		if(control_id==3)//运行按钮
		{
			if(state==0)//停止运行
			{				
				AnimationPlayFrame(3,1,1);//显示停止图标
				AnimationStop(3,2);//动画停止播放
			}
			else//开始运行
			{				
				SetControlVisiable(3,1,1);//显示图标
				SetControlVisiable(3,2,1);//显示动画

				AnimationPlayFrame(3,1,0);//显示运行图标
				AnimationStart(3,2);//动画开始播放
			}		
		}
		else if(control_id==4)//复位按钮
		{
			SetControlVisiable(3,1,0);//隐藏图标
			SetControlVisiable(3,2,0);//隐藏动画
			SetButtonValue(3,3,0);//显示开始运行
		}
	}
}

/*! 
 *  \brief  文本控件通知
 *  \details  当文本通过键盘更新(或调用GetControlValue)时，执行此函数
 *  \param screen_id 画面ID
 *  \param control_id 控件ID
 *  \param str 文本控件内容
 */
static void NotifyText(uint16 screen_id, uint16 control_id, uint8 *str)
{
	//TODO: 添加用户代码
	int32 value = 0; 

	if(screen_id==2)//画面ID2：文本设置和显示
	{
		sscanf((const char *)str,"%ld",&value);//把字符串转换为整数

		if(control_id==1)//最高电压
		{
			//限定数值范围（也可以在文本控件属性中设置）
			if(value<0)
				value = 0;
			else if(value>380)
				value = 380;

			SetTextValueInt32(2,1,value);  //更新最高电压
			SetTextValueInt32(2,4,value/2);  //更新最高电压/2
		}
	}
}

/*! 
 *  \brief  进度条控件通知
 *  \details  调用GetControlValue时，执行此函数
 *  \param screen_id 画面ID
 *  \param control_id 控件ID
 *  \param value 值
 */
static void NotifyProgress(uint16 screen_id, uint16 control_id, uint32 value)
{
	//TODO: 添加用户代码
}

/*! 
 *  \brief  滑动条控件通知
 *  \details  当滑动条改变(或调用GetControlValue)时，执行此函数
 *  \param screen_id 画面ID
 *  \param control_id 控件ID
 *  \param value 值
 */
static void NotifySlider(uint16 screen_id, uint16 control_id, uint32 value)
{
	//TODO: 添加用户代码
	if(screen_id==5&&control_id==2)//滑块控制
	{
		test_value = value;

		SetProgressValue(5,1,test_value); //更新进度条数值
	}
}

/*! 
 *  \brief  仪表控件通知
 *  \details  调用GetControlValue时，执行此函数
 *  \param screen_id 画面ID
 *  \param control_id 控件ID
 *  \param value 值
 */
static void NotifyMeter(uint16 screen_id, uint16 control_id, uint32 value)
{
	//TODO: 添加用户代码
}

/*! 
 *  \brief  菜单控件通知
 *  \details  当菜单项按下或松开时，执行此函数
 *  \param screen_id 画面ID
 *  \param control_id 控件ID
 *  \param item 菜单项索引
 *  \param state 按钮状态：0松开，1按下
 */
static void NotifyMenu(uint16 screen_id, uint16 control_id, uint8  item, uint8  state)
{
	//TODO: 添加用户代码
}

/*! 
 *  \brief  选择控件通知
 *  \details  当选择控件变化时，执行此函数
 *  \param screen_id 画面ID
 *  \param control_id 控件ID
 *  \param item 当前选项
 */
static void NotifySelector(uint16 screen_id, uint16 control_id, uint8  item)
{
	//TODO: 添加用户代码
}

/*! 
 *  \brief  定时器超时通知处理
 *  \param screen_id 画面ID
 *  \param control_id 控件ID
 */
static void NotifyTimer(uint16 screen_id, uint16 control_id)
{
	//TODO: 添加用户代码
}

/*! 
 *  \brief  读取用户FLASH状态返回
 *  \param status 0失败，1成功
 *  \param _data 返回数据
 *  \param length 数据长度
 */
static void NotifyReadFlash(uint8 status,uint8 *_data,uint16 length)
{
	//TODO: 添加用户代码
}

/*! 
 *  \brief  写用户FLASH状态返回
 *  \param status 0失败，1成功
 */
static void NotifyWriteFlash(uint8 status)
{
	//TODO: 添加用户代码
}

/*! 
 *  \brief  读取RTC时间，注意返回的是BCD码
 *  \param year 年（BCD）
 *  \param month 月（BCD）
 *  \param week 星期（BCD）
 *  \param day 日（BCD）
 *  \param hour 时（BCD）
 *  \param minute 分（BCD）
 *  \param second 秒（BCD）
 */
static void NotifyReadRTC(uint8 year,uint8 month,uint8 week,uint8 day,uint8 hour,uint8 minute,uint8 second)
{

}


/*! 
 *  \brief  消息处理流程
 *  \param msg 待处理消息
 *  \param size 消息长度
 */
static void ProcessMessage( PCTRL_MSG msg, uint16 size )
{
	uint8 cmd_type = msg->cmd_type;					//指令类型
	uint8 ctrl_msg = msg->ctrl_msg;   				//消息的类型
	uint8 control_type = msg->control_type;			//控件类型
	uint16 screen_id = PTR2U16(&msg->screen_id);	//画面ID
	uint16 control_id = PTR2U16(&msg->control_id);	//控件ID
	uint32 value = PTR2U32(msg->param);				//数值

    
	switch(cmd_type)
	{		
		case NOTIFY_TOUCH_PRESS:					//触摸屏按下
		case NOTIFY_TOUCH_RELEASE:					//触摸屏松开
			NotifyTouchXY(cmd_buffer[1],PTR2U16(cmd_buffer+2),PTR2U16(cmd_buffer+4));
			break;	
		case NOTIFY_WRITE_FLASH_OK:					//写FLASH成功
			NotifyWriteFlash(1);
			break;
		case NOTIFY_WRITE_FLASH_FAILD:				//写FLASH失败
			NotifyWriteFlash(0);
			break;
		case NOTIFY_READ_FLASH_OK:					//读取FLASH成功
			NotifyReadFlash(1,cmd_buffer+2,size-6);	//去除帧头帧尾
			break;
		case NOTIFY_READ_FLASH_FAILD:				//读取FLASH失败
			NotifyReadFlash(0,0,0);
			break;
		case NOTIFY_READ_RTC:						//读取RTC时间
			NotifyReadRTC(cmd_buffer[1],cmd_buffer[2],cmd_buffer[3],cmd_buffer[4],cmd_buffer[5],cmd_buffer[6],cmd_buffer[7]);
			break;
		case NOTIFY_CONTROL:
		{
			if(ctrl_msg==MSG_GET_CURRENT_SCREEN)	//画面ID变化通知
			{
				NotifyScreen(screen_id);
			}
			else
			{
				switch(control_type)
				{
					case kCtrlButton: 				//按钮控件
						NotifyButton(screen_id,control_id,msg->param[1]);
						break;
					case kCtrlText:					//文本控件
						NotifyText(screen_id,control_id,msg->param);
						break;
					case kCtrlProgress: 			//进度条控件
						NotifyProgress(screen_id,control_id,value);
						break;
					case kCtrlSlider: 				//滑动条控件
						NotifySlider(screen_id,control_id,value);
						break;
					case kCtrlMeter: 				//仪表控件
						NotifyMeter(screen_id,control_id,value);
						break;
					case kCtrlMenu:					//菜单控件
						NotifyMenu(screen_id,control_id,msg->param[0],msg->param[1]);
						break;
					case kCtrlSelector:				//选择控件
						NotifySelector(screen_id,control_id,msg->param[0]);
						break;
					case kCtrlRTC:					//倒计时控件
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
static void Hmi_var_init()
{
    uint16 i=0;
    
    for(i=0;i<CMD_MAX_SIZE;i++)
    {
        cmd_buffer[i] = 0;
    }
    current_screen_id = 0;
    test_value = 0;               
    update_en = 0;
}


void Hmi_init(void)
{
    Hmi_var_init();
    queue_reset();
    
    hmi_driver_init();
}

void Hmi_App_Run(void)
{
	qsize  size = 0;
	//uint32 timer_tick_last_update = 0; //上一次更新的时间

	size = queue_find_cmd(cmd_buffer,CMD_MAX_SIZE); //从缓冲区中获取一条指令        
	if(size>0)//接收到指令
	{
		ProcessMessage((PCTRL_MSG)cmd_buffer, size);//指令处理
	}
    
    /****************************************************************************************************************
        特别注意
        MCU不要频繁向串口屏发送数据，否则串口屏的内部缓存区会满，从而导致数据丢失(缓冲区大小：标准型8K，基本型4.7K)
        1) 一般情况下，控制MCU向串口屏发送数据的周期大于100ms，就可以避免数据丢失的问题；
        2) 如果仍然有数据丢失的问题，请判断串口屏的BUSY引脚，为高时不能发送数据给串口屏。
    ******************************************************************************************************************/
    
	if(update_en == 100000)
	{
		update_en = 0;
		UpdateUI(); 
	}
    update_en++;
}

