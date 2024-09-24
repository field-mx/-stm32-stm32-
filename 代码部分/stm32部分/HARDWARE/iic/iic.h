#ifndef _iic_H
#define _iic_H

#include "stm32f10x.h"

/*  IIC_SCLʱ�Ӷ˿ڡ����Ŷ��� */
#define IIC_SCL_PORT 			GPIOF   
#define IIC_SCL_PIN 			(GPIO_Pin_12)
#define IIC_SCL_PORT_RCC		RCC_APB2Periph_GPIOF

/*  IIC_SDAʱ�Ӷ˿ڡ����Ŷ��� */
#define IIC_SDA_PORT 			GPIOF  
#define IIC_SDA_PIN 			(GPIO_Pin_14)
#define IIC_SDA_PORT_RCC		RCC_APB2Periph_GPIOF

//IO��������	 
#define IIC_SCL    PFout(12) //SCL
#define IIC_SDA    PFout(14) //SDA	 
#define READ_SDA   PFin(14)  //����SDA

//IIC���в�������
void IIC_Init(void);                //��ʼ��IIC��IO��				 
void IIC_Start(void);				//����IIC��ʼ�ź�
void IIC_Stop(void);	  			//����IICֹͣ�ź�
void IIC_Send_Byte(u8 txd);			//IIC����һ���ֽ�
u8 IIC_Read_Byte(u8 ack);			//IIC��ȡһ���ֽ�
u8 IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void IIC_Ack(void);					//IIC����ACK�ź�
void IIC_NAck(void);				//IIC������ACK�ź�


#endif
