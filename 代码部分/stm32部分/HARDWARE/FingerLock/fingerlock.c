
#include "fingerlock.h"
#include "delay.h"
#include "oled.h"
#include "led.h"
#include "stdlib.h"
#include "beep.h"
#include "key.h"
#include "usart.h"
#include "as608.h"
#include "usart3.h"
#include "camera_f.h"
#include "bmp.h"
#include "lcd.h"
#include "screen.h"
#include "text.h"
#include "ble_app.h"
#include "ble_uart.h"
#include "ble_cfg.h"
#include "USART2.h"
#include "string.h"

#define AS608_baud  57600//通信波特率 根据指纹模块波特率更改 指纹模块波特率可通过上位机更改

extern u8 sd_ok;
extern u8 *pname;
extern u8 g;
extern u8 k210;

_finger finger;//_finger结构体变量 为三种功能模式赋予指标 规定 0：默认刷指纹模式，1：添加指纹，2：删除指纹
u16 ValidN;//模块内有效指纹个数
SysPara AS608Para;//SysPara结构体变量 内含AS608参数

//显示错误信息 留空以便添加功能
void ShowErrorMessage(u8 ensure)
{

}

void takephoto()
{
    u8 sendcnt=0;  
	while(sendcnt<1)
	{
		u3_printf2(" INVADER \r\n",sendcnt);
		sendcnt++;	
	}       
		if(sd_ok)
	{
		LED1=0;	//点亮DS1,提示正在拍照
		camera_new_pathname(pname);//得到文件名	
	
		if(bmp_encode(pname,(lcddev.width-240)/2,(lcddev.height-320)/2,240,320,0))//拍照有误
		{
					Show_Str(40,130,240,12,"写入文件错误!",12,0);		 
		}
		else 
		{
			Show_Str(40,130,240,12,"未知身份！请于管理员取得联系!",12,0);
			Show_Str(40,150,240,12,"如已经录入，请清理指纹传感器",12,0);	

			Ble_Test();
	    
 			BEEP=1;	//蜂鸣器短叫，提示拍照完成
			delay_ms(50);
			BEEP=0;
			delay_ms(50);
			BEEP=1;
			delay_ms(50);
			BEEP=0;
		} 
				
	}
	else //提示SD卡错误
	{					    
		Show_Str(40,130,240,12,"SD卡错误!",12,0);
 		Show_Str(40,150,240,12,"拍照功能不可用!",12,0);			    
 	}
}



//刷指纹
void Press_FP(void)
{
	SearchResult seach;//SearchResult结构体中定义了指纹ID和匹配得分
	u8 ensure;
	u8 strbuf[20];
	u8 key=0;
	
	while(1)
	{		
		camera_refresh();
		ensure=PS_GetImage();
		if(ensure==0x00)//获取图像成功 
		{	
			ensure=PS_GenChar(CharBuffer1);
			if(ensure==0x00) //生成特征成功
			{			
				ensure=PS_HighSpeedSearch(CharBuffer1,0,AS608Para.PS_max,&seach);//高速搜索函数 判断flash中是否有对应指纹模板
				if(ensure==0x00)//搜索成功确认码0x00
				{				
					BEEP=1;//打开蜂鸣器	
					LED1=0;//开锁
					OLED_Clear();
					OLED_ShowString(0,0,"Verified!",16);
					sprintf((char *)strbuf,"ID:%d  Score:%d",seach.pageID,seach.mathscore);//%d格式意为int型
					OLED_ShowString(0,20,strbuf,16);
					OLED_Refresh_Gram();
					Screen_True();//屏幕显示正确
					Show_Str_Mid(20,70,"ID:",16,0);
					Show_Str_Mid(20,90,"score",16,0);
					LCD_ShowNum(80,70,seach.pageID,4,16);
					LCD_ShowNum(80,90,seach.mathscore,4,16);
					delay_ms(500);
					BEEP=0;//关闭蜂鸣器
					delay_ms(1500);
					LED1=1;//关锁
					break;
				}
				else 
					camera_refresh();
					takephoto();
				delay_ms(1000);	
				Screen_Init();				
			}
			else
				ShowErrorMessage(ensure);
		}
		
		key=KEY_Scan(0);
		if(key==KEY1_PRESS)//当KEY1键按下退出主菜单，进入设置菜单界面
		{
			finger.mode=10;//将mode设置为0至5之外的值 避开进入对应的功能
			break;
		}
		if(key==KEY2_PRESS)
		{
			LCD_Clear(WHITE);
			Show_Str(30,50,200,16,"模式已切换",16,0);
			Show_Str(30,70,200,16,"进入人脸识别模式",16,0);
			delay_ms(5000);
			LCD_Clear(WHITE);
			while(1)
			{
				Show_Str(30,50,200,16,"请保持面部置于取景框内",16,0);
				Show_Str(30,70,200,16,"按下按键以录入人脸",16,0);
			
				if(USART_RX_STA&0x8000)
				{	    
					k210=USART_RX_BUF[0];
					if(k210==0x31)//接收到1
					{

						LED1=1;
						BEEP=1;
						delay_ms(1000);
						BEEP=0;
						LCD_Clear(WHITE);
						Show_Str(30,50,200,16,"验证成功，",16,0);
						Show_Str(30,70,200,16,"欢迎进入！",16,0);
						delay_ms(3000);
						k210=3;
					}
					if(k210==0x30)//接收到0
					{
						LCD_Clear(WHITE);
						Show_Str(30,50,200,16,"认证失败，",16,0);
						Show_Str(30,70,200,16,"请与管理员取得联系！",16,0);
						while(g<5)
						{
							BEEP=1;
							delay_ms(300);
							BEEP=0;
							delay_ms(300);
							g+=1;
						}
						g=0;
						k210=3;
					}
					USART_RX_STA=0;
				}
				key=KEY_Scan(0);
				if(key==4)
				{
					break;
				}
			}
		}
	}
}

//录入指纹函数
void Add_FP(void)
{
	u8 ensure ,processnum=0;
	u16 ID=0;//指纹序号 最大值为299
	u8 key=0;
	
	//选择指纹ID
	OLED_Clear();
	OLED_ShowString(0,0,"Add finger",16);
	OLED_ShowString(0,20,"Finger ID:  0",16);
	OLED_Refresh_Gram();
	Screen_Add1();
	while(1)
	{
		key=KEY_Scan(0);
		if(key==KEY_UP_PRESS)//返回主界面
		{
			
			break;
		}
		else if(key==KEY1_PRESS)//修改ID
		{
			ID++;
			if(ID>=AS608Para.PS_max)ID=0;
			OLED_ShowNum(80,20,ID,3,16);
			OLED_Refresh_Gram();
			LCD_Clear(WHITE);
			delay_ms(1);
		}
		else if(key==KEY0_PRESS)//确定
		{
			while(1)
			{
				switch (processnum)
				{
					case 0:
						OLED_ShowString(0,40,"Please Press Finger.",12);		
						OLED_Refresh_Gram();
						Screen_Add2();
						ensure=PS_GetImage();//录入图像函数 将指纹图像存储于ImageBuffer
						if(ensure==0x00) //录入成功返回码0x00
						{
							BEEP=1;//PB8输出高电平 开启蜂鸣器
							ensure=PS_GenChar(CharBuffer1);//生成特征函数 将ImageBuffer中的原始图像存储于CharBuffer1
							BEEP=0;//PB8输出低电平 关闭蜂鸣器
							if(ensure==0x00)//生成特征成功返回码0x00
							{
								OLED_ShowString(0,40,"                    ",12);
								OLED_ShowString(0,40,"Finger 1 is OK.",12);
								OLED_Refresh_Gram();
								Screen_Add3();
								processnum=1;//跳到第二步
							}else{ShowErrorMessage(ensure);}					
						}
						break;
					
					case 1:
						//请按再按一次指纹
						OLED_ShowString(0,40,"                    ",12);
						OLED_ShowString(0,40,"Press finger again.",12);
						OLED_Refresh_Gram();
						Screen_Add4();
						ensure=PS_GetImage();
						if(ensure==0x00) 
						{
							BEEP=1;
							ensure=PS_GenChar(CharBuffer2);//生成特征函数 将ImageBuffer中的原始图像存储于CharBuffer2
							BEEP=0;
							if(ensure==0x00)
							{
								OLED_ShowString(0,40,"                    ",12);
								OLED_ShowString(0,40,"Finger 2 is OK.",12);
								OLED_Refresh_Gram();
								processnum=2;//跳到第三步
							}else{ShowErrorMessage(ensure);	}
						}							
						break;//此处即使break仍然循环回case1 因此时processnum=1

					case 2:
						//对比两次指纹
						OLED_ShowString(0,40,"                    ",12);
						OLED_ShowString(0,40,"Comparing...",12);
						OLED_Refresh_Gram();
						Screen_Add5();
						ensure=PS_Match();//对比函数 精确对比CharBuffer1和CharBuffer2中的特征文件
						delay_ms(2000);
						if(ensure==0x00)//对比成功确认码为0x00
						{
							OLED_ShowString(0,40,"                    ",12);
							OLED_ShowString(0,40,"Success!",12);
							OLED_Refresh_Gram();
							Screen_Add6();
							processnum=3;//跳到第四步
						}
						else//对比失败
						{
							OLED_ShowString(0,40,"                    ",12);
							OLED_ShowString(0,40,"Error finger!",12);
							OLED_Refresh_Gram();
							Screen_Add7();
							ShowErrorMessage(ensure);
							processnum=0;//跳回第一步		
						}
						delay_ms(1200);
						break;

					case 3:
						//生成指纹模板
						ensure=PS_RegModel();//将CharBuffer1与CharBuffer2中的特征文件合并生成模板 模板仍然存于其中
						if(ensure==0x00)//模板生成成功确认码为0x00
						{
							//生成指纹模板成功
							processnum=4;//跳到第五步
						}else
						{
							processnum=0;//返回第一步
							ShowErrorMessage(ensure);
						}
						delay_ms(1200);
						break;
						
					case 4:	
						ensure=PS_StoreChar(CharBuffer2,ID);//储存模板函数将CharBuffer中的模板文件存到PageID号flash数据库位置
						if(ensure==0x00)//以下为指纹存储成功后OLED执行的内容 可修改
						{
							PS_ValidTempleteNum(&ValidN);//读库指纹个数并赋给变量ValidN
							OLED_ShowString(0,40,"                    ",12);
							OLED_ShowString(0,40,"Finger Save OK!",12);
							OLED_Refresh_Gram();
							
							//LCD显示
							LCD_Clear(WHITE);
							POINT_COLOR=BLUE; 
							Show_Str_Mid(30,50,"Finger Save OK!",16,0); 
							Show_Str_Mid(30,70,"Cap:",16,0);
							LCD_ShowNum(100,70,AS608Para.PS_max-ValidN,3,16);
							
							delay_ms(2000);
							return;//不能用break 否则依然在while循环内 使用return退出Add_FP函数 无法使用goto去往MENU 因其不在一个函数内
						}else{processnum=0;ShowErrorMessage(ensure);}
						break;
				}
				delay_ms(1000);//switch被break打断后延时1s
			}
		}
	}

}

//删除指纹函数
void Delete_FP(void)
{
	u8  ensure;
	u8 key=0;
	u16 ID=0;
	
	//选择指纹ID
	OLED_Clear();
	OLED_ShowString(0,0,"Delete Finger...",16);
	OLED_ShowString(0,20,"ID:  0",16);
	OLED_Refresh_Gram();
	Screen_Del1();
	
	while(1)
	{
		key=KEY_Scan(0);
		if(key==KEY_UP_PRESS)//返回设置界面
		{
			break;
		}
		else if(key==KEY1_PRESS)//修改ID 按一下KEY1则ID+1
		{
			ID++;
			if(ID>=AS608Para.PS_max)ID=0;//超过ID=299时再次按下KEY1将使其归0
			OLED_ShowNum(3*8,20,ID,3,16);
			OLED_Refresh_Gram();
		}
		else if(key==KEY0_PRESS)//确定
		{
			ensure=PS_DeletChar(ID,1);//删除指纹函数 删除flash数据库中指定ID号开始的N个指纹模板 此处删除单个指纹
			if(ensure==0x00)//删除成功确认码0x00
			{
				OLED_ShowString(0,40,"Delete OK!",12);
				OLED_Refresh_Gram();
				Screen_Del2();
			}
			else{ShowErrorMessage(ensure);}
			delay_ms(1200);
			OLED_Refresh_Gram();
			return;
		}
	}	
}


//主菜单显示
void main_window_show(void)
{
	u8 ensure=0;
	
	//使用goto语句
	MENU:	
	//默认刷指纹模式
	//mode未初始化 被编译器自动初始化为0
	if(finger.mode==0)
	{
		OLED_Clear();
		OLED_ShowString(15,0,"Press Finger",16);
		//与AS608模块握手 检测连接是否正常
		//PS_HandShake函数发送数据并判断AS608是否产生对应的响应
		//握手成功返回0 握手失败返回1 即执行while循环不断提示错误
		while(PS_HandShake(&AS608Addr))
		{
			OLED_ShowString(0,20,"AS608 Error!",16);
			OLED_Refresh_Gram();
			Screen_Trou();
			delay_ms(500);
			OLED_Fill_rectangle(0,20,128,16,0);
			OLED_Refresh_Gram();
			delay_ms(200);
		}
		//以下至while循环发生前为主界面显示内容 建议修改
		//Cap为剩余指纹容量 Level为对比等级 范围为1至5 等级越高认假率越低 拒认率最高 等级及波特率可通过上位机更改
		//默认等级为3 波特率57600
		OLED_ShowString(0,20,"Hello!",12);
		OLED_ShowString(0,35,"Welcome home!",12);
		Screen_Init();
		ensure=PS_ValidTempleteNum(&ValidN);//读取库指纹个数
		if(ensure!=0x00)
			ShowErrorMessage(ensure);//显示确认码错误信息
		ensure=PS_ReadSysPara(&AS608Para);  //读参数
		if(ensure==0x00)//读取成功确认码0x00
		{
			OLED_ShowString(0,50,"Prove yourself!",12);
		}
		else
			ShowErrorMessage(ensure);
		OLED_Refresh_Gram();  //刷新GRAM数组
		
		Press_FP();//刷指纹
		goto MENU;//回到函数开头
	}	
}

//设置菜单显示
void setup_window_show(void)
{
	u8 key=0;
	u8 mode=0;
	
	SET_MENU:
	//指纹设置模式
	if(finger.mode!=0)
	{
		OLED_Clear();
		OLED_ShowString(30,0,"GPA 4.0!",16);
		OLED_ShowString(40,20,"Add",16);
		OLED_ShowString(40,40,"Delete",16);
		OLED_ShowString(0,20,"-->",16);
		OLED_Refresh_Gram();  //刷新GRAM数组
		
		Screen_Win1();
		
		while(1)
		{
			key=KEY_Scan(0);//检测是否有按键按下 有则返回对应键值 无则返回0
			if(key==KEY1_PRESS)//第一次按下KEY1进入选择模式
			{
				mode++;
				OLED_ShowString(0,(mode)*20,"   ",16);//清除上次箭头指向
				Screen_Win2();
				if(mode>=2)//第二次按下KEY1移动箭头并将mode赋值为0 重复此过程
				{
					mode=0;
				}
				OLED_ShowString(0,(mode+1)*20,"-->",16);//箭头指向
				OLED_Refresh_Gram();
				Show_Str_Mid(0,(50+mode*20),"-->",16,0);//箭头指向
			}
			else if(key==KEY_UP_PRESS)//返回主菜单
			{
				finger.mode=0;
				break;
			}
			else if(key==KEY0_PRESS)//确认
			{
				finger.mode=mode+1;
			}
			//添加指纹
			if(finger.mode==1)
			{
				Add_FP();
				mode=0;
				finger.mode=10;
				//返回上一级菜单
				OLED_Clear();
				OLED_ShowString(30,0,"GPA 4.0!",16);
				OLED_ShowString(40,20,"Add",16);
				OLED_ShowString(40,40,"Delete",16);
				OLED_ShowString(0,20,"-->",16);
				OLED_Refresh_Gram();  //刷新GRAM数组
				
				Screen_Win1();
				
			}
			//删除指纹
			else if(finger.mode==2)
			{
				OLED_Clear();
				OLED_ShowString(30,0,"GPA 4.0!",16);
				OLED_ShowString(40,20,"Delete one",16);
				OLED_ShowString(40,40,"Delete all",16);
				OLED_ShowString(0,20,"-->",16);
				OLED_Refresh_Gram();
				Screen_Win3();
				
				while(1)
				{
					key=KEY_Scan(0);
					if(key==KEY_UP_PRESS)
					{
						key=0;
						mode=0;
						finger.mode=10;
						goto SET_MENU;
						
					}
					else if(key==KEY1_PRESS)
					{
						mode++;
						OLED_ShowString(0,(mode-1)*20,"   ",16);//清除上次箭头指向
						Screen_Win4();
						if(mode>=3)//第二次按下KEY1移动箭头并将mode赋值为3 重复此过程
						{
							mode=1;
						}
						OLED_ShowString(0,(mode)*20,"-->",16);//箭头指向
						OLED_Refresh_Gram();
						Show_Str_Mid(0,(50+(mode-1)*20),"-->",16,0);//箭头指向
					}
					else if(key==KEY0_PRESS)
					{
						finger.mode=mode+3;
					}
					if(finger.mode==4)
					{
						Delete_FP();
						key=0;
						mode=0;
						finger.mode=10;
						goto SET_MENU;
					}
					else if(finger.mode==5)
					{
						PS_Empty();
						key=0;
						mode=0;
						finger.mode=10;
						OLED_Clear();
						OLED_ShowString(0,20,"All deleted.",16);
						OLED_Refresh_Gram();
						Screen_Win5();
						delay_ms(3000);
						goto SET_MENU;
					}
				}
			}
		}
	}
}

void finger_lock(void)
{
	USART3_Init(AS608_baud);//AS608默认使用USART3串口进行通讯 设置波特率与指纹模块相同
	BEEP_Init();//蜂鸣器IO初始化
	KEY_Init();//按键IO初始化
	OLED_Init();//OLED初始化
	
	while(1)
	{
		main_window_show();//主窗口显示
		setup_window_show();//设置指纹窗口显示
	}
}
