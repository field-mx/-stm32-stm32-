
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

#define AS608_baud  57600//ͨ�Ų����� ����ָ��ģ�鲨���ʸ��� ָ��ģ�鲨���ʿ�ͨ����λ������

extern u8 sd_ok;
extern u8 *pname;
extern u8 g;
extern u8 k210;

_finger finger;//_finger�ṹ����� Ϊ���ֹ���ģʽ����ָ�� �涨 0��Ĭ��ˢָ��ģʽ��1�����ָ�ƣ�2��ɾ��ָ��
u16 ValidN;//ģ������Чָ�Ƹ���
SysPara AS608Para;//SysPara�ṹ����� �ں�AS608����

//��ʾ������Ϣ �����Ա���ӹ���
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
		LED1=0;	//����DS1,��ʾ��������
		camera_new_pathname(pname);//�õ��ļ���	
	
		if(bmp_encode(pname,(lcddev.width-240)/2,(lcddev.height-320)/2,240,320,0))//��������
		{
					Show_Str(40,130,240,12,"д���ļ�����!",12,0);		 
		}
		else 
		{
			Show_Str(40,130,240,12,"δ֪��ݣ����ڹ���Աȡ����ϵ!",12,0);
			Show_Str(40,150,240,12,"���Ѿ�¼�룬������ָ�ƴ�����",12,0);	

			Ble_Test();
	    
 			BEEP=1;	//�������̽У���ʾ�������
			delay_ms(50);
			BEEP=0;
			delay_ms(50);
			BEEP=1;
			delay_ms(50);
			BEEP=0;
		} 
				
	}
	else //��ʾSD������
	{					    
		Show_Str(40,130,240,12,"SD������!",12,0);
 		Show_Str(40,150,240,12,"���չ��ܲ�����!",12,0);			    
 	}
}



//ˢָ��
void Press_FP(void)
{
	SearchResult seach;//SearchResult�ṹ���ж�����ָ��ID��ƥ��÷�
	u8 ensure;
	u8 strbuf[20];
	u8 key=0;
	
	while(1)
	{		
		camera_refresh();
		ensure=PS_GetImage();
		if(ensure==0x00)//��ȡͼ��ɹ� 
		{	
			ensure=PS_GenChar(CharBuffer1);
			if(ensure==0x00) //���������ɹ�
			{			
				ensure=PS_HighSpeedSearch(CharBuffer1,0,AS608Para.PS_max,&seach);//������������ �ж�flash���Ƿ��ж�Ӧָ��ģ��
				if(ensure==0x00)//�����ɹ�ȷ����0x00
				{				
					BEEP=1;//�򿪷�����	
					LED1=0;//����
					OLED_Clear();
					OLED_ShowString(0,0,"Verified!",16);
					sprintf((char *)strbuf,"ID:%d  Score:%d",seach.pageID,seach.mathscore);//%d��ʽ��Ϊint��
					OLED_ShowString(0,20,strbuf,16);
					OLED_Refresh_Gram();
					Screen_True();//��Ļ��ʾ��ȷ
					Show_Str_Mid(20,70,"ID:",16,0);
					Show_Str_Mid(20,90,"score",16,0);
					LCD_ShowNum(80,70,seach.pageID,4,16);
					LCD_ShowNum(80,90,seach.mathscore,4,16);
					delay_ms(500);
					BEEP=0;//�رշ�����
					delay_ms(1500);
					LED1=1;//����
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
		if(key==KEY1_PRESS)//��KEY1�������˳����˵����������ò˵�����
		{
			finger.mode=10;//��mode����Ϊ0��5֮���ֵ �ܿ������Ӧ�Ĺ���
			break;
		}
		if(key==KEY2_PRESS)
		{
			LCD_Clear(WHITE);
			Show_Str(30,50,200,16,"ģʽ���л�",16,0);
			Show_Str(30,70,200,16,"��������ʶ��ģʽ",16,0);
			delay_ms(5000);
			LCD_Clear(WHITE);
			while(1)
			{
				Show_Str(30,50,200,16,"�뱣���沿����ȡ������",16,0);
				Show_Str(30,70,200,16,"���°�����¼������",16,0);
			
				if(USART_RX_STA&0x8000)
				{	    
					k210=USART_RX_BUF[0];
					if(k210==0x31)//���յ�1
					{

						LED1=1;
						BEEP=1;
						delay_ms(1000);
						BEEP=0;
						LCD_Clear(WHITE);
						Show_Str(30,50,200,16,"��֤�ɹ���",16,0);
						Show_Str(30,70,200,16,"��ӭ���룡",16,0);
						delay_ms(3000);
						k210=3;
					}
					if(k210==0x30)//���յ�0
					{
						LCD_Clear(WHITE);
						Show_Str(30,50,200,16,"��֤ʧ�ܣ�",16,0);
						Show_Str(30,70,200,16,"�������Աȡ����ϵ��",16,0);
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

//¼��ָ�ƺ���
void Add_FP(void)
{
	u8 ensure ,processnum=0;
	u16 ID=0;//ָ����� ���ֵΪ299
	u8 key=0;
	
	//ѡ��ָ��ID
	OLED_Clear();
	OLED_ShowString(0,0,"Add finger",16);
	OLED_ShowString(0,20,"Finger ID:  0",16);
	OLED_Refresh_Gram();
	Screen_Add1();
	while(1)
	{
		key=KEY_Scan(0);
		if(key==KEY_UP_PRESS)//����������
		{
			
			break;
		}
		else if(key==KEY1_PRESS)//�޸�ID
		{
			ID++;
			if(ID>=AS608Para.PS_max)ID=0;
			OLED_ShowNum(80,20,ID,3,16);
			OLED_Refresh_Gram();
			LCD_Clear(WHITE);
			delay_ms(1);
		}
		else if(key==KEY0_PRESS)//ȷ��
		{
			while(1)
			{
				switch (processnum)
				{
					case 0:
						OLED_ShowString(0,40,"Please Press Finger.",12);		
						OLED_Refresh_Gram();
						Screen_Add2();
						ensure=PS_GetImage();//¼��ͼ���� ��ָ��ͼ��洢��ImageBuffer
						if(ensure==0x00) //¼��ɹ�������0x00
						{
							BEEP=1;//PB8����ߵ�ƽ ����������
							ensure=PS_GenChar(CharBuffer1);//������������ ��ImageBuffer�е�ԭʼͼ��洢��CharBuffer1
							BEEP=0;//PB8����͵�ƽ �رշ�����
							if(ensure==0x00)//���������ɹ�������0x00
							{
								OLED_ShowString(0,40,"                    ",12);
								OLED_ShowString(0,40,"Finger 1 is OK.",12);
								OLED_Refresh_Gram();
								Screen_Add3();
								processnum=1;//�����ڶ���
							}else{ShowErrorMessage(ensure);}					
						}
						break;
					
					case 1:
						//�밴�ٰ�һ��ָ��
						OLED_ShowString(0,40,"                    ",12);
						OLED_ShowString(0,40,"Press finger again.",12);
						OLED_Refresh_Gram();
						Screen_Add4();
						ensure=PS_GetImage();
						if(ensure==0x00) 
						{
							BEEP=1;
							ensure=PS_GenChar(CharBuffer2);//������������ ��ImageBuffer�е�ԭʼͼ��洢��CharBuffer2
							BEEP=0;
							if(ensure==0x00)
							{
								OLED_ShowString(0,40,"                    ",12);
								OLED_ShowString(0,40,"Finger 2 is OK.",12);
								OLED_Refresh_Gram();
								processnum=2;//����������
							}else{ShowErrorMessage(ensure);	}
						}							
						break;//�˴���ʹbreak��Ȼѭ����case1 ���ʱprocessnum=1

					case 2:
						//�Ա�����ָ��
						OLED_ShowString(0,40,"                    ",12);
						OLED_ShowString(0,40,"Comparing...",12);
						OLED_Refresh_Gram();
						Screen_Add5();
						ensure=PS_Match();//�ԱȺ��� ��ȷ�Ա�CharBuffer1��CharBuffer2�е������ļ�
						delay_ms(2000);
						if(ensure==0x00)//�Աȳɹ�ȷ����Ϊ0x00
						{
							OLED_ShowString(0,40,"                    ",12);
							OLED_ShowString(0,40,"Success!",12);
							OLED_Refresh_Gram();
							Screen_Add6();
							processnum=3;//�������Ĳ�
						}
						else//�Ա�ʧ��
						{
							OLED_ShowString(0,40,"                    ",12);
							OLED_ShowString(0,40,"Error finger!",12);
							OLED_Refresh_Gram();
							Screen_Add7();
							ShowErrorMessage(ensure);
							processnum=0;//���ص�һ��		
						}
						delay_ms(1200);
						break;

					case 3:
						//����ָ��ģ��
						ensure=PS_RegModel();//��CharBuffer1��CharBuffer2�е������ļ��ϲ�����ģ�� ģ����Ȼ��������
						if(ensure==0x00)//ģ�����ɳɹ�ȷ����Ϊ0x00
						{
							//����ָ��ģ��ɹ�
							processnum=4;//�������岽
						}else
						{
							processnum=0;//���ص�һ��
							ShowErrorMessage(ensure);
						}
						delay_ms(1200);
						break;
						
					case 4:	
						ensure=PS_StoreChar(CharBuffer2,ID);//����ģ�庯����CharBuffer�е�ģ���ļ��浽PageID��flash���ݿ�λ��
						if(ensure==0x00)//����Ϊָ�ƴ洢�ɹ���OLEDִ�е����� ���޸�
						{
							PS_ValidTempleteNum(&ValidN);//����ָ�Ƹ�������������ValidN
							OLED_ShowString(0,40,"                    ",12);
							OLED_ShowString(0,40,"Finger Save OK!",12);
							OLED_Refresh_Gram();
							
							//LCD��ʾ
							LCD_Clear(WHITE);
							POINT_COLOR=BLUE; 
							Show_Str_Mid(30,50,"Finger Save OK!",16,0); 
							Show_Str_Mid(30,70,"Cap:",16,0);
							LCD_ShowNum(100,70,AS608Para.PS_max-ValidN,3,16);
							
							delay_ms(2000);
							return;//������break ������Ȼ��whileѭ���� ʹ��return�˳�Add_FP���� �޷�ʹ��gotoȥ��MENU ���䲻��һ��������
						}else{processnum=0;ShowErrorMessage(ensure);}
						break;
				}
				delay_ms(1000);//switch��break��Ϻ���ʱ1s
			}
		}
	}

}

//ɾ��ָ�ƺ���
void Delete_FP(void)
{
	u8  ensure;
	u8 key=0;
	u16 ID=0;
	
	//ѡ��ָ��ID
	OLED_Clear();
	OLED_ShowString(0,0,"Delete Finger...",16);
	OLED_ShowString(0,20,"ID:  0",16);
	OLED_Refresh_Gram();
	Screen_Del1();
	
	while(1)
	{
		key=KEY_Scan(0);
		if(key==KEY_UP_PRESS)//�������ý���
		{
			break;
		}
		else if(key==KEY1_PRESS)//�޸�ID ��һ��KEY1��ID+1
		{
			ID++;
			if(ID>=AS608Para.PS_max)ID=0;//����ID=299ʱ�ٴΰ���KEY1��ʹ���0
			OLED_ShowNum(3*8,20,ID,3,16);
			OLED_Refresh_Gram();
		}
		else if(key==KEY0_PRESS)//ȷ��
		{
			ensure=PS_DeletChar(ID,1);//ɾ��ָ�ƺ��� ɾ��flash���ݿ���ָ��ID�ſ�ʼ��N��ָ��ģ�� �˴�ɾ������ָ��
			if(ensure==0x00)//ɾ���ɹ�ȷ����0x00
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


//���˵���ʾ
void main_window_show(void)
{
	u8 ensure=0;
	
	//ʹ��goto���
	MENU:	
	//Ĭ��ˢָ��ģʽ
	//modeδ��ʼ�� ���������Զ���ʼ��Ϊ0
	if(finger.mode==0)
	{
		OLED_Clear();
		OLED_ShowString(15,0,"Press Finger",16);
		//��AS608ģ������ ��������Ƿ�����
		//PS_HandShake�����������ݲ��ж�AS608�Ƿ������Ӧ����Ӧ
		//���ֳɹ�����0 ����ʧ�ܷ���1 ��ִ��whileѭ��������ʾ����
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
		//������whileѭ������ǰΪ��������ʾ���� �����޸�
		//CapΪʣ��ָ������ LevelΪ�Աȵȼ� ��ΧΪ1��5 �ȼ�Խ���ϼ���Խ�� ��������� �ȼ��������ʿ�ͨ����λ������
		//Ĭ�ϵȼ�Ϊ3 ������57600
		OLED_ShowString(0,20,"Hello!",12);
		OLED_ShowString(0,35,"Welcome home!",12);
		Screen_Init();
		ensure=PS_ValidTempleteNum(&ValidN);//��ȡ��ָ�Ƹ���
		if(ensure!=0x00)
			ShowErrorMessage(ensure);//��ʾȷ���������Ϣ
		ensure=PS_ReadSysPara(&AS608Para);  //������
		if(ensure==0x00)//��ȡ�ɹ�ȷ����0x00
		{
			OLED_ShowString(0,50,"Prove yourself!",12);
		}
		else
			ShowErrorMessage(ensure);
		OLED_Refresh_Gram();  //ˢ��GRAM����
		
		Press_FP();//ˢָ��
		goto MENU;//�ص�������ͷ
	}	
}

//���ò˵���ʾ
void setup_window_show(void)
{
	u8 key=0;
	u8 mode=0;
	
	SET_MENU:
	//ָ������ģʽ
	if(finger.mode!=0)
	{
		OLED_Clear();
		OLED_ShowString(30,0,"GPA 4.0!",16);
		OLED_ShowString(40,20,"Add",16);
		OLED_ShowString(40,40,"Delete",16);
		OLED_ShowString(0,20,"-->",16);
		OLED_Refresh_Gram();  //ˢ��GRAM����
		
		Screen_Win1();
		
		while(1)
		{
			key=KEY_Scan(0);//����Ƿ��а������� ���򷵻ض�Ӧ��ֵ ���򷵻�0
			if(key==KEY1_PRESS)//��һ�ΰ���KEY1����ѡ��ģʽ
			{
				mode++;
				OLED_ShowString(0,(mode)*20,"   ",16);//����ϴμ�ͷָ��
				Screen_Win2();
				if(mode>=2)//�ڶ��ΰ���KEY1�ƶ���ͷ����mode��ֵΪ0 �ظ��˹���
				{
					mode=0;
				}
				OLED_ShowString(0,(mode+1)*20,"-->",16);//��ͷָ��
				OLED_Refresh_Gram();
				Show_Str_Mid(0,(50+mode*20),"-->",16,0);//��ͷָ��
			}
			else if(key==KEY_UP_PRESS)//�������˵�
			{
				finger.mode=0;
				break;
			}
			else if(key==KEY0_PRESS)//ȷ��
			{
				finger.mode=mode+1;
			}
			//���ָ��
			if(finger.mode==1)
			{
				Add_FP();
				mode=0;
				finger.mode=10;
				//������һ���˵�
				OLED_Clear();
				OLED_ShowString(30,0,"GPA 4.0!",16);
				OLED_ShowString(40,20,"Add",16);
				OLED_ShowString(40,40,"Delete",16);
				OLED_ShowString(0,20,"-->",16);
				OLED_Refresh_Gram();  //ˢ��GRAM����
				
				Screen_Win1();
				
			}
			//ɾ��ָ��
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
						OLED_ShowString(0,(mode-1)*20,"   ",16);//����ϴμ�ͷָ��
						Screen_Win4();
						if(mode>=3)//�ڶ��ΰ���KEY1�ƶ���ͷ����mode��ֵΪ3 �ظ��˹���
						{
							mode=1;
						}
						OLED_ShowString(0,(mode)*20,"-->",16);//��ͷָ��
						OLED_Refresh_Gram();
						Show_Str_Mid(0,(50+(mode-1)*20),"-->",16,0);//��ͷָ��
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
	USART3_Init(AS608_baud);//AS608Ĭ��ʹ��USART3���ڽ���ͨѶ ���ò�������ָ��ģ����ͬ
	BEEP_Init();//������IO��ʼ��
	KEY_Init();//����IO��ʼ��
	OLED_Init();//OLED��ʼ��
	
	while(1)
	{
		main_window_show();//��������ʾ
		setup_window_show();//����ָ�ƴ�����ʾ
	}
}
