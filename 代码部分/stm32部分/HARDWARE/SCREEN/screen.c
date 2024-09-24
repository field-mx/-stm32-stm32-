#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"
#include "rtc.h" 
#include "wkup.h"
#include "usmart.h" 
#include "malloc.h"
#include "sdio_sdcard.h"  
#include "w25qxx.h"    
#include "ff.h"  
#include "exfuns.h"   
#include "text.h"
#include "delay.h"
#include "screen.h"

void Screen_Init(void)//显示&汉显&待机&时钟
{
	//初始化
	u8 key; //汉字用变量    
	delay_init();	    	 //延时函数初始化	  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	uart_init(115200);	 	//串口初始化为115200
	// 	usmart_dev.init(72);		//初始化USMART	
	//	usmart_dev.init(SystemCoreClock/1000000);	//初始化USMART	
 	LED_Init();		  			//初始化与LED连接的硬件接口
	KEY_Init();					//初始化按键
	LCD_Init();			   		//初始化LCD  
	//WKUP_Init(); //待机唤醒初始化
	RTC_Init();//初始化实时时钟	
	W25QXX_Init();				//初始化W25Q128
 	my_mem_init(SRAMIN);		//初始化内部内存池
	exfuns_init();				//为fatfs相关变量申请内存  
 	f_mount(fs[0],"0:",1); 		//挂载SD卡 
 	f_mount(fs[1],"1:",1); 		//挂载FLASH.
	
	POINT_COLOR=RED;
	
	//汉字字库初始化
	while(font_init()) 			//检查字库
	{
		LCD_Clear(WHITE);		   	//清屏
 		POINT_COLOR=RED;			//设置字体为红色	   	   	  
		LCD_ShowString(30,50,200,16,16,"WarShip STM32");
		while(SD_Init())			//检测SD卡
		{
			LCD_ShowString(30,70,200,16,16,"SD Card Failed!");
			delay_ms(200);
			LCD_Fill(30,70,200+30,70+16,WHITE);
			delay_ms(200);		    
		}								 						    
		LCD_ShowString(30,70,200,16,16,"SD Card OK");
		LCD_ShowString(30,90,200,16,16,"Font Updating...");
		key=update_font(20,110,16,"0:");//更新字库
		while(key)//更新失败		
		{			 		  
			LCD_ShowString(30,110,200,16,16,"Font Update Failed!");
			delay_ms(200);
			LCD_Fill(20,110,200+20,110+16,WHITE);
			delay_ms(200);		       
		} 		  
		LCD_ShowString(30,110,200,16,16,"Font Update Success!   ");
		delay_ms(1500);	
		LCD_Clear(WHITE);//清屏	       
	}
	
	

//显示	
//  	while(1) 
//	{		 
		LCD_Clear(WHITE);
		POINT_COLOR=BLACK;	
		Show_Str(60,10,200,16,"智能安保系统",16,0);
		Show_Str(30,50,200,16,"欢迎使用智能安保系统",16,0);
		Show_Str(30,70,200,16,"扫描您的指纹",16,0);
		Show_Str(30,90,200,16,"以识别身份",16,0);
		delay_ms(1000);
	//显示时间
//	POINT_COLOR=BLUE;//设置字体为蓝色
//	LCD_ShowString(60,130,200,16,16,"    -  -  ");	   
//	LCD_ShowString(60,172,200,16,16,"  :  :  ");		    
//	while(1)
//	{								    
//		if(t!=calendar.sec)
//		{
//			t=calendar.sec;
//			LCD_ShowNum(60,130,calendar.w_year,4,16);									  
//			LCD_ShowNum(100,130,calendar.w_month,2,16);									  
//			LCD_ShowNum(124,130,calendar.w_date,2,16);	 
//			switch(calendar.week)
//			{
//				case 0:
//					Show_Str(60,148,200,16,"周日",16,0);
//					break;
//				case 1:
//					Show_Str(60,148,200,16,"周一",16,0);
//					break;
//				case 2:
//					Show_Str(60,148,200,16,"周二",16,0);
//					break;
//				case 3:
//					Show_Str(60,148,200,16,"周三",16,0);					
//				break;
//				case 4:
//					Show_Str(60,148,200,16,"周四",16,0);
//					break;
//				case 5:
//					Show_Str(60,148,200,16,"周五",16,0);
//					break;
//				case 6:
//					Show_Str(60,148,200,16,"周六",16,0);
//					break;  
//			}
//			LCD_ShowNum(60,168,calendar.hour,2,16);									  
//			LCD_ShowNum(84,168,calendar.min,2,16);									  
//			LCD_ShowNum(108,168,calendar.sec,2,16);
//		}	
//		delay_ms(10);								  
//	};  

//	} 
	
}

void Screen_Clear(void)
{
		Show_Str(30,50,200,16,"                        ",16,0);
		Show_Str(30,70,200,16,"                        ",16,0);
		Show_Str(30,90,200,16,"                        ",16,0);
}	


void Screen_False(void)
{
							 
	LCD_Clear(WHITE);
	POINT_COLOR=RED;  //设置字体颜色为红色	
		Show_Str(30,50,200,16,"身份验证失败",16,0);
		Show_Str(30,70,200,16,"请重试",16,0);
	POINT_COLOR=BRRED;
		Show_Str(30,90,200,16,"相机已收集您的图像",16,0);	
							
}

void Screen_True(void)
{
	
	LCD_Clear(WHITE);			
	POINT_COLOR=BLUE; 		
	Show_Str(30,50,200,16,"身份验证成功",16,0);	

}	

void Screen_Add1(void)
{
	
	LCD_Clear(WHITE);				
	POINT_COLOR=BRRED;
	Show_Str(30,50,200,16,"添加指纹",16,0);			
	Show_Str(30,70,200,16,"ID:0",16,0);		

}	

void Screen_Add2(void)
{
	
	LCD_Clear(WHITE);
	POINT_COLOR=BLACK; 
		Show_Str(30,50,200,16,"请将指纹置于扫描器处",16,0);
}

void Screen_Add3(void)
{
	LCD_Clear(WHITE);	
	POINT_COLOR=BLACK; 
	Show_Str(30,50,200,16,"指纹录入成功",16,0);	

}

void Screen_Add4(void)
{
	LCD_Clear(WHITE);				
	POINT_COLOR=BLACK; 
	Show_Str(30,50,200,16,"请再次将指纹置于扫描器处",16,0);	
	Show_Str(30,70,200,16,"以对比两次指纹输入",16,0);	
}

void Screen_Add5(void)
{
	LCD_Clear(WHITE);						
	POINT_COLOR=BLACK; 	
	Show_Str(30,50,200,16,"对比两次指纹",16,0);

}

void Screen_Add6(void)
{
	LCD_Clear(WHITE);	
	POINT_COLOR=BLUE; 
	Show_Str(30,50,200,16,"指纹对比成功",16,0);
	Show_Str(30,70,200,16,"两次指纹相同",16,0);

}

void Screen_Add7(void)
{
	LCD_Clear(WHITE);			
	POINT_COLOR=RED; 	
	Show_Str(30,50,200,16,"指纹对比失败",16,0);
	Show_Str(30,70,200,16,"请再次录入指纹",16,0);
	
}

void Screen_Del1(void)
{
	LCD_Clear(WHITE);					
	POINT_COLOR=RED; 	
	Show_Str(30,50,200,16,"删除指纹",16,0);
	Show_Str(30,70,200,16,"ID:0",16,0);	
}

void Screen_Del2(void)
{
	LCD_Clear(WHITE);			
	POINT_COLOR=BLUE; 
	Show_Str(30,50,200,16,"成功删除指纹",16,0);	

}

void Screen_Trou(void)
{	
	LCD_Clear(WHITE);
	POINT_COLOR=GREEN; 
	Show_Str(30,50,200,16,"模块连接出错",16,0);
	Show_Str(30,70,200,16,"请重新连接",16,0);

}

void Screen_Win1(void)
{
	LCD_Clear(WHITE);
	POINT_COLOR=BLACK; 
	Show_Str(30,50,200,16,"添加指纹",16,0);
	Show_Str(30,70,200,16,"删除指纹",16,0);
	Show_Str(00,50,200,16,"-->",16,0);
}

void Screen_Win2(void)
{
	LCD_Clear(WHITE);
	POINT_COLOR=BLACK; 
	Show_Str(30,50,200,16,"添加指纹",16,0);
	Show_Str(30,70,200,16,"删除指纹",16,0);
}

void Screen_Win3(void)
{
	LCD_Clear(WHITE);
	POINT_COLOR=BLACK; 
	Show_Str(30,50,200,16,"删除一个指纹",16,0);
	Show_Str(30,70,200,16,"删除全部指纹",16,0);
	Show_Str(00,50,200,16,"-->",16,0);
}	

void Screen_Win4(void)
{
	LCD_Clear(WHITE);
	POINT_COLOR=BLACK; 
	Show_Str(30,50,200,16,"删除一个指纹",16,0);
	Show_Str(30,70,200,16,"删除全部指纹",16,0);
}	

void Screen_Win5(void)
{
	LCD_Clear(WHITE);
	POINT_COLOR=BLACK; 
	Show_Str(30,50,200,16,"指纹全部删除",16,0);
}	
