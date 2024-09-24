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

void Screen_Init(void)//��ʾ&����&����&ʱ��
{
	//��ʼ��
	u8 key; //�����ñ���    
	delay_init();	    	 //��ʱ������ʼ��	  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 	//���ڳ�ʼ��Ϊ115200
	// 	usmart_dev.init(72);		//��ʼ��USMART	
	//	usmart_dev.init(SystemCoreClock/1000000);	//��ʼ��USMART	
 	LED_Init();		  			//��ʼ����LED���ӵ�Ӳ���ӿ�
	KEY_Init();					//��ʼ������
	LCD_Init();			   		//��ʼ��LCD  
	//WKUP_Init(); //�������ѳ�ʼ��
	RTC_Init();//��ʼ��ʵʱʱ��	
	W25QXX_Init();				//��ʼ��W25Q128
 	my_mem_init(SRAMIN);		//��ʼ���ڲ��ڴ��
	exfuns_init();				//Ϊfatfs��ر��������ڴ�  
 	f_mount(fs[0],"0:",1); 		//����SD�� 
 	f_mount(fs[1],"1:",1); 		//����FLASH.
	
	POINT_COLOR=RED;
	
	//�����ֿ��ʼ��
	while(font_init()) 			//����ֿ�
	{
		LCD_Clear(WHITE);		   	//����
 		POINT_COLOR=RED;			//��������Ϊ��ɫ	   	   	  
		LCD_ShowString(30,50,200,16,16,"WarShip STM32");
		while(SD_Init())			//���SD��
		{
			LCD_ShowString(30,70,200,16,16,"SD Card Failed!");
			delay_ms(200);
			LCD_Fill(30,70,200+30,70+16,WHITE);
			delay_ms(200);		    
		}								 						    
		LCD_ShowString(30,70,200,16,16,"SD Card OK");
		LCD_ShowString(30,90,200,16,16,"Font Updating...");
		key=update_font(20,110,16,"0:");//�����ֿ�
		while(key)//����ʧ��		
		{			 		  
			LCD_ShowString(30,110,200,16,16,"Font Update Failed!");
			delay_ms(200);
			LCD_Fill(20,110,200+20,110+16,WHITE);
			delay_ms(200);		       
		} 		  
		LCD_ShowString(30,110,200,16,16,"Font Update Success!   ");
		delay_ms(1500);	
		LCD_Clear(WHITE);//����	       
	}
	
	

//��ʾ	
//  	while(1) 
//	{		 
		LCD_Clear(WHITE);
		POINT_COLOR=BLACK;	
		Show_Str(60,10,200,16,"���ܰ���ϵͳ",16,0);
		Show_Str(30,50,200,16,"��ӭʹ�����ܰ���ϵͳ",16,0);
		Show_Str(30,70,200,16,"ɨ������ָ��",16,0);
		Show_Str(30,90,200,16,"��ʶ�����",16,0);
		delay_ms(1000);
	//��ʾʱ��
//	POINT_COLOR=BLUE;//��������Ϊ��ɫ
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
//					Show_Str(60,148,200,16,"����",16,0);
//					break;
//				case 1:
//					Show_Str(60,148,200,16,"��һ",16,0);
//					break;
//				case 2:
//					Show_Str(60,148,200,16,"�ܶ�",16,0);
//					break;
//				case 3:
//					Show_Str(60,148,200,16,"����",16,0);					
//				break;
//				case 4:
//					Show_Str(60,148,200,16,"����",16,0);
//					break;
//				case 5:
//					Show_Str(60,148,200,16,"����",16,0);
//					break;
//				case 6:
//					Show_Str(60,148,200,16,"����",16,0);
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
	POINT_COLOR=RED;  //����������ɫΪ��ɫ	
		Show_Str(30,50,200,16,"�����֤ʧ��",16,0);
		Show_Str(30,70,200,16,"������",16,0);
	POINT_COLOR=BRRED;
		Show_Str(30,90,200,16,"������ռ�����ͼ��",16,0);	
							
}

void Screen_True(void)
{
	
	LCD_Clear(WHITE);			
	POINT_COLOR=BLUE; 		
	Show_Str(30,50,200,16,"�����֤�ɹ�",16,0);	

}	

void Screen_Add1(void)
{
	
	LCD_Clear(WHITE);				
	POINT_COLOR=BRRED;
	Show_Str(30,50,200,16,"���ָ��",16,0);			
	Show_Str(30,70,200,16,"ID:0",16,0);		

}	

void Screen_Add2(void)
{
	
	LCD_Clear(WHITE);
	POINT_COLOR=BLACK; 
		Show_Str(30,50,200,16,"�뽫ָ������ɨ������",16,0);
}

void Screen_Add3(void)
{
	LCD_Clear(WHITE);	
	POINT_COLOR=BLACK; 
	Show_Str(30,50,200,16,"ָ��¼��ɹ�",16,0);	

}

void Screen_Add4(void)
{
	LCD_Clear(WHITE);				
	POINT_COLOR=BLACK; 
	Show_Str(30,50,200,16,"���ٴν�ָ������ɨ������",16,0);	
	Show_Str(30,70,200,16,"�ԶԱ�����ָ������",16,0);	
}

void Screen_Add5(void)
{
	LCD_Clear(WHITE);						
	POINT_COLOR=BLACK; 	
	Show_Str(30,50,200,16,"�Ա�����ָ��",16,0);

}

void Screen_Add6(void)
{
	LCD_Clear(WHITE);	
	POINT_COLOR=BLUE; 
	Show_Str(30,50,200,16,"ָ�ƶԱȳɹ�",16,0);
	Show_Str(30,70,200,16,"����ָ����ͬ",16,0);

}

void Screen_Add7(void)
{
	LCD_Clear(WHITE);			
	POINT_COLOR=RED; 	
	Show_Str(30,50,200,16,"ָ�ƶԱ�ʧ��",16,0);
	Show_Str(30,70,200,16,"���ٴ�¼��ָ��",16,0);
	
}

void Screen_Del1(void)
{
	LCD_Clear(WHITE);					
	POINT_COLOR=RED; 	
	Show_Str(30,50,200,16,"ɾ��ָ��",16,0);
	Show_Str(30,70,200,16,"ID:0",16,0);	
}

void Screen_Del2(void)
{
	LCD_Clear(WHITE);			
	POINT_COLOR=BLUE; 
	Show_Str(30,50,200,16,"�ɹ�ɾ��ָ��",16,0);	

}

void Screen_Trou(void)
{	
	LCD_Clear(WHITE);
	POINT_COLOR=GREEN; 
	Show_Str(30,50,200,16,"ģ�����ӳ���",16,0);
	Show_Str(30,70,200,16,"����������",16,0);

}

void Screen_Win1(void)
{
	LCD_Clear(WHITE);
	POINT_COLOR=BLACK; 
	Show_Str(30,50,200,16,"���ָ��",16,0);
	Show_Str(30,70,200,16,"ɾ��ָ��",16,0);
	Show_Str(00,50,200,16,"-->",16,0);
}

void Screen_Win2(void)
{
	LCD_Clear(WHITE);
	POINT_COLOR=BLACK; 
	Show_Str(30,50,200,16,"���ָ��",16,0);
	Show_Str(30,70,200,16,"ɾ��ָ��",16,0);
}

void Screen_Win3(void)
{
	LCD_Clear(WHITE);
	POINT_COLOR=BLACK; 
	Show_Str(30,50,200,16,"ɾ��һ��ָ��",16,0);
	Show_Str(30,70,200,16,"ɾ��ȫ��ָ��",16,0);
	Show_Str(00,50,200,16,"-->",16,0);
}	

void Screen_Win4(void)
{
	LCD_Clear(WHITE);
	POINT_COLOR=BLACK; 
	Show_Str(30,50,200,16,"ɾ��һ��ָ��",16,0);
	Show_Str(30,70,200,16,"ɾ��ȫ��ָ��",16,0);
}	

void Screen_Win5(void)
{
	LCD_Clear(WHITE);
	POINT_COLOR=BLACK; 
	Show_Str(30,50,200,16,"ָ��ȫ��ɾ��",16,0);
}	
