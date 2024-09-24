#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "lcd.h"
#include "key.h"
#include "usmart.h"
#include "malloc.h"
#include "sdio_sdcard.h" 
#include "w25qxx.h"
#include "ff.h"
#include "exfuns.h"
#include "text.h"
#include "piclib.h"
#include "string.h"
#include "math.h"
#include "ov7725.h"
#include "beep.h"
#include "timer.h"
#include "exti.h"
#include "OV7670.h"
#include "camera_f.h"
#include "fingerlock.h"
#include "ble_app.h"
#include "ble_uart.h"
#include "ble_cfg.h"

extern u8 ov_sta;	//在exit.c里面定义
extern u8 ov_frame;	//在timer.c里面定义	   

	u8 res;							 
	u8 *pname;				//带路径的文件名 
	u8 key;					//键值		   
	u8 i;						 
	u8 g=0;
	u8 k210;
	u8 sd_ok=1;				//0,sd卡不正常;1,SD卡正常.

	
 int main(void)
 {

	delay_init();	    	 //延时函数初始化	  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	uart_init(115200);	 	//串口初始化为115200
 	usmart_dev.init(72);		//初始化USMART		
 	LED_Init();		  			//初始化与LED连接的硬件接口
	KEY_Init();					//初始化按键
	 LCD_Init();			   		//初始化LCD  
	Ble_IoInit(); //BLE引脚初始化
  Ble_SetInit();//BLE参数配置
//	
	TIM3_Int_Init(1000-1,36000-1);  //定时1000*36000/72us=500ms从而使LED灯不断闪烁 表明系统正在运行
	BEEP_Init();        		//蜂鸣器初始化	 
	W25QXX_Init();				//初始化W25Q128
 	my_mem_init(SRAMIN);		//初始化内部内存池
	exfuns_init();				//为fatfs相关变量申请内存  
 	f_mount(fs[0],"0:",1); 		//挂载SD卡 
 	f_mount(fs[1],"1:",1); 		//挂载FLASH. 
	POINT_COLOR=RED;      
 	while(font_init()) 				//检查字库
	{	    
		LCD_ShowString(30,50,200,16,16,"Font Error!");
		delay_ms(200);				  
		LCD_Fill(30,50,240,66,WHITE);//清除显示	     
	}  	 
 	Show_Str(30,50,200,16,"多功能智能门禁",16,0);				    	 
	Show_Str(30,70,200,16,"准备进行初始化...",16,0);				    	 				    	 
	Show_Str(30,110,200,16,"北洋大学鹏翔硅谷荣誉出品",16,0);				    	 
	Show_Str(30,130,200,16,"喜迎二十大，永远跟党走！",16,0);
	res=f_mkdir("0:/PHOTO");		//创建PHOTO文件夹
	if(res!=FR_EXIST&&res!=FR_OK) 	//发生了错误
	{		    
		Show_Str(30,150,240,16,"SD fail!",16,0);
		delay_ms(200);				  
		Show_Str(30,170,240,16,"cant take photo!",16,0);
		sd_ok=0;  	
	}
	else
	{
		Show_Str(30,150,240,16,"SD卡准备就绪!",16,0);
		delay_ms(200);				  
		Show_Str(30,170,240,16,"指纹识别错误将会自动拍照",16,0);
		sd_ok=1;  	  
	}										   						    
 	pname=mymalloc(SRAMIN,30);	//为带路径的文件名分配30个字节的内存		    
 	while(pname==NULL)			//内存分配出错
 	{	    
		Show_Str(30,190,240,16,"error molloc!",16,0);
		delay_ms(200);				  
		LCD_Fill(30,190,240,146,WHITE);//清除显示	     
		delay_ms(200);				  
	}   											  
	
	while(1)//初始化OV7725_OV7670
	{
		
		if(OV7670_Init()==0)
		{
			//sensor=OV7670;
			LCD_ShowString(30,150,200,16,16,"摄像头初始化成功！      ");
			delay_ms(1500);	 	   
			OV7670_Light_Mode(0);
			OV7670_Color_Saturation(0);
			OV7670_Brightness(0);
			OV7670_Contrast(0);
			OV7670_Special_Effects(0);
			OV7670_Window_Set(12,176,240,320);//设置窗口
		
			break;
		}
		else
		{
			LCD_ShowString(30,150,200,16,16,"OV7725_OV7670 Error!!");
			delay_ms(200);
			LCD_Fill(30,150,200,246,WHITE);
			delay_ms(200);
		}
	} 
	
		OV7670_CS=0;
	
	TIM6_Int_Init(10000,7199);			//10Khz计数频率,1秒钟中断									  
	EXTI8_Init();						//使能定时器捕获
	LCD_Clear(WHITE);
 	
	finger_lock();
							    
}
 














