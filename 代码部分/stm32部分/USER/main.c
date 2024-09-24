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

extern u8 ov_sta;	//��exit.c���涨��
extern u8 ov_frame;	//��timer.c���涨��	   

	u8 res;							 
	u8 *pname;				//��·�����ļ��� 
	u8 key;					//��ֵ		   
	u8 i;						 
	u8 g=0;
	u8 k210;
	u8 sd_ok=1;				//0,sd��������;1,SD������.

	
 int main(void)
 {

	delay_init();	    	 //��ʱ������ʼ��	  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 	//���ڳ�ʼ��Ϊ115200
 	usmart_dev.init(72);		//��ʼ��USMART		
 	LED_Init();		  			//��ʼ����LED���ӵ�Ӳ���ӿ�
	KEY_Init();					//��ʼ������
	 LCD_Init();			   		//��ʼ��LCD  
	Ble_IoInit(); //BLE���ų�ʼ��
  Ble_SetInit();//BLE��������
//	
	TIM3_Int_Init(1000-1,36000-1);  //��ʱ1000*36000/72us=500ms�Ӷ�ʹLED�Ʋ�����˸ ����ϵͳ��������
	BEEP_Init();        		//��������ʼ��	 
	W25QXX_Init();				//��ʼ��W25Q128
 	my_mem_init(SRAMIN);		//��ʼ���ڲ��ڴ��
	exfuns_init();				//Ϊfatfs��ر��������ڴ�  
 	f_mount(fs[0],"0:",1); 		//����SD�� 
 	f_mount(fs[1],"1:",1); 		//����FLASH. 
	POINT_COLOR=RED;      
 	while(font_init()) 				//����ֿ�
	{	    
		LCD_ShowString(30,50,200,16,16,"Font Error!");
		delay_ms(200);				  
		LCD_Fill(30,50,240,66,WHITE);//�����ʾ	     
	}  	 
 	Show_Str(30,50,200,16,"�๦�������Ž�",16,0);				    	 
	Show_Str(30,70,200,16,"׼�����г�ʼ��...",16,0);				    	 				    	 
	Show_Str(30,110,200,16,"�����ѧ������������Ʒ",16,0);				    	 
	Show_Str(30,130,200,16,"ϲӭ��ʮ����Զ�����ߣ�",16,0);
	res=f_mkdir("0:/PHOTO");		//����PHOTO�ļ���
	if(res!=FR_EXIST&&res!=FR_OK) 	//�����˴���
	{		    
		Show_Str(30,150,240,16,"SD fail!",16,0);
		delay_ms(200);				  
		Show_Str(30,170,240,16,"cant take photo!",16,0);
		sd_ok=0;  	
	}
	else
	{
		Show_Str(30,150,240,16,"SD��׼������!",16,0);
		delay_ms(200);				  
		Show_Str(30,170,240,16,"ָ��ʶ����󽫻��Զ�����",16,0);
		sd_ok=1;  	  
	}										   						    
 	pname=mymalloc(SRAMIN,30);	//Ϊ��·�����ļ�������30���ֽڵ��ڴ�		    
 	while(pname==NULL)			//�ڴ�������
 	{	    
		Show_Str(30,190,240,16,"error molloc!",16,0);
		delay_ms(200);				  
		LCD_Fill(30,190,240,146,WHITE);//�����ʾ	     
		delay_ms(200);				  
	}   											  
	
	while(1)//��ʼ��OV7725_OV7670
	{
		
		if(OV7670_Init()==0)
		{
			//sensor=OV7670;
			LCD_ShowString(30,150,200,16,16,"����ͷ��ʼ���ɹ���      ");
			delay_ms(1500);	 	   
			OV7670_Light_Mode(0);
			OV7670_Color_Saturation(0);
			OV7670_Brightness(0);
			OV7670_Contrast(0);
			OV7670_Special_Effects(0);
			OV7670_Window_Set(12,176,240,320);//���ô���
		
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
	
	TIM6_Int_Init(10000,7199);			//10Khz����Ƶ��,1�����ж�									  
	EXTI8_Init();						//ʹ�ܶ�ʱ������
	LCD_Clear(WHITE);
 	
	finger_lock();
							    
}
 














