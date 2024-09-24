#include "ble_app.h"
#include "ble_uart.h"
#include "ble_cfg.h"
#include "USART2.h"
#include "usart.h"
#include "string.h"
#include "led.h"
#include "delay.h"
#include "key.h"
#include "bmp.h"



//BLEģ��IO��ʼ��
void Ble_IoInit(void)
{

    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE); /*ʹ��PA�˿�ʱ��*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); /*ʹ�ܸ��ù���ʱ��*/

    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); /*��ֹJTAG,�Ӷ�PA15��������ͨIOʹ��,����PA15��������ͨIO!!!*/

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;          /*BLE_WKUP*/
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    /*�������*/
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   /*IO���ٶ�Ϊ50MHz*/
    GPIO_Init(GPIOE, &GPIO_InitStructure);              /*������� ��IO���ٶ�Ϊ50MHz*/

    BLE_WKUP = 1;
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;            /*BLE_STA*/
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;        /*��������*/
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;    /*IO���ٶ�Ϊ50MHz*/
    GPIO_Init(GPIOE, &GPIO_InitStructure);               /*�����趨������ʼ��GPIOA.4*/
    
    USART2_init(115200);/*��������*/

}

u8 sendbuf[50];

//BLEģ���������
void Ble_SetInit(void)
{

    LED0 = 0; /*LED0��*/

    /*****��������ģʽ*****/
//    while (ble_send_cmd("+++a", "a+ok", 5))
//    {
//        if (!ble_send_cmd("+++a", "ERROR", 10)) break; /**�п���ģ����������ģʽ,��ʱ����+++a,�᷵��error*/

//        LED1 = ~LED1;
//        delay_ms(100);
//    }

    /*****AT����*****/
    if (!ble_send_cmd("AT", "OK", 5))
    {
        printf("AT OK\r\n");
    }

    /*****������������*****/
    sprintf((char *)sendbuf, "AT+NAME=%s", BLE_NAME);

    if (!ble_send_cmd(sendbuf, "OK", 10))
    {
        printf("%s  OK\r\n", sendbuf);
    }else
    {
        printf("%s  ERROR\r\n", sendbuf);
    }

    /*****������ӭ������*****/
    sprintf((char *)sendbuf, "AT+HELLO=%s", BLE_HELLO);

    if (!ble_send_cmd(sendbuf, "OK", 10))
    {
        printf("%s  OK\r\n", sendbuf);
    }else
    {
        printf("%s  ERROR\r\n", sendbuf);
    }

    /*****���书������*****/
    sprintf((char *)sendbuf, "AT+TPL=%d", BLE_TP_P2dBm);

    if (!ble_send_cmd(sendbuf, "OK", 10))
    {
        printf("%s  OK\r\n", sendbuf);
    }
    else{
        printf("%s  ERROR\r\n", sendbuf);
    }

    /*****��������*****/
    sprintf((char *)sendbuf, "AT+UART=%d,%d,%d,%d", BLE_UARTBPS, BLE_UARTDATE, BLE_UARTPR, BLE_UARTSTOP);

    if (!ble_send_cmd(sendbuf, "OK", 10))
    {
        printf("%s  OK\r\n", sendbuf);
    }else
    {
        printf("%s  ERROR\r\n", sendbuf);
    }

    /*****�㲥�ٶ�����*****/
    sprintf((char *)sendbuf, "AT+ADPTIM=%d", BLE_ADPTIM );

    if (!ble_send_cmd(sendbuf, "OK", 10))
    {
        printf("%s  OK\r\n", sendbuf);
    }else
    {
        printf("%s  ERROR\r\n", sendbuf);
    }

    /*****��·ƥ����������*****/
#if BLE_LINKPASSEN

    if (!ble_send_cmd("AT+LINKPASSEN=ON", "OK", 10)) /*ʹ��*/
    {
        printf("AT+LINKPASSEN=ON  OK\r\n");
    }else
    {
        printf("AT+LINKPASSEN=ON  ERROR\r\n");
    }

#else
    
    if (!ble_send_cmd("AT+LINKPASSEN=OFF", "OK", 10)) /*�ر�*/
    {
        printf("AT+LINKPASSEN=OFF  OK\r\n");
    }else
    {
        printf("AT+LINKPASSEN=OFF  ERROR\r\n");
    }
    
#endif    
    
    /*****����LED����*****/
#if BLE_LEDEN

    if (!ble_send_cmd("AT+LEDEN=ON", "OK", 10)) /*ʹ��*/
    {
        printf("AT+LEDEN=ON  OK\r\n");
    }else
    {
        printf("AT+LEDEN=ON  ERROR\r\n");
    }

#else

    if (!ble_send_cmd("AT+LEDEN=OFF", "OK", 50)) /*�ر�*/
    {
        printf("AT+LEDEN=OFF  OK\r\n");
    }else
    {
        printf("AT+LEDEN=OFF  ERROR\r\n");
    }

#endif

    /*****���豸����˯������*****/
#if BLE_SLAVESLEEPEN

    if (!ble_send_cmd("AT+SLAVESLEEPEN=ON", "OK", 10)) /*ʹ��*/
    {
        printf("AT+SLAVESLEEPEN=ON  OK\r\n");
    }else
    {
        printf("AT+SLAVESLEEPEN=ON  ERROR\r\n");
    }

#else

    if (!ble_send_cmd("AT+SLAVESLEEPEN=OFF", "OK", 10)) /*�ر�*/
    {
        printf("AT+SLAVESLEEPEN=OFF  OK\r\n");
    }else
    {
        printf("AT+SLAVESLEEPEN=OFF  ERROR\r\n");
    }

#endif

    /*****��������������*****/
#if BLE_MAXPUT

    if (!ble_send_cmd("AT+MAXPUT=ON", "OK", 10)) /*ʹ��*/
    {
        printf("AT+MAXPUT=ON  OK\r\n");
    }else
    {
        printf("AT+MAXPUT=ON  ERROR\r\n");
    }

#else

    if (!ble_send_cmd("AT+MAXPUT=OFF", "OK", 10)) /*�ر�*/
    {
        printf("AT+MAXPUT=OFF  OK\r\n");
    }else
    {
        printf("AT+MAXPUT=OFF  ERROR\r\n");
    }

#endif

    /*****����ģʽ����(ģʽ���ú�,ģ��������λ����͸��ģʽ)*****/
    sprintf((char *)sendbuf, "AT+MODE=%c", BLE_MODE_S );

    if (!ble_send_cmd(sendbuf, "OK", 10))
    {
        printf("%s  OK\r\n", sendbuf);
    }else
    {
        printf("%s  ERROR\r\n", sendbuf);
    }

    LED0 = 1;/*LED0��*/
}

/*BLEģ�����ݷ��ʹ���*/
void Ble_SendData(void)
{

	  USART_SendData(USART2,'O');	

}
//ͼƬ���ݷ���
void Ble_Sendpic(u8 color)
{
		u8 r, g, b, temp;
		u16 send;
		r = 0x1f&(color>>11);
		g = 0x1f&(color>>6);
		b = 0x1f&color;
		send = (r<<10)|(g<<5)|b;
		 
		temp = send&0x00ff;							//׼���������ݣ��Ͱ�λ
		USART_SendData(USART2,temp);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC) != SET);
		temp = send>>8;								//�߰�λ
		USART_SendData(USART2,temp);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC) != SET);
}


/*BLEģ��������ݴ���*/
void Ble_ReceData(void)
{
    u16 i = 0;
    u16 len = 0;

    //����������
    if (USART2_RX_STA & 0x8000)
    {
        len = USART2_RX_STA & 0X7FFF;
        USART2_RX_BUF[len] = 0; //��ӽ�����
        USART2_RX_STA = 0;

        printf("R:");

        for (i = 0; i < len; i++)
        {
            while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET); //ѭ������,ֱ���������
            USART_SendData(USART1, USART2_RX_BUF[i]);
        }

        if (strstr((const char *)USART2_RX_BUF, LED0_ON) && (len == (sizeof(LED0_ON) - 1)))        LED0 = 0; /*LED0����*/
        else if (strstr((const char *)USART2_RX_BUF, LED0_OFF) && (len == (sizeof(LED0_OFF) - 1))) LED0 = 1; /*LED0�ر�*/

        memset((char *)USART2_RX_BUF, 0x00, len); //���ڽ��ջ�������0
        printf("\r\n");
    }

}

/*BLE���̴���*/
void Ble_Process(void)
{
    u8 key = 0;
    u8 t = 0;
    u8 BLE_LINK = 0; //1:�������� 0:δ����
    static u8 flag1, flag2 = 0;

    printf("\r\nKEY1:��������\r\n");

    while (1)
    {

        key = KEY_Scan(0);

        if(key == KEY1_PRESS) //��������
        {
            if (BLE_LINK) /*����������*/
            {
                Ble_SendData();/*���ݷ��ʹ���*/
								Ble_Sendpic(0x00);
            }
        }

        /*���ݽ��մ���*/
        Ble_ReceData();

        /*BLE���Ӵ���*/
        if (BLE_STA)
        {
            BLE_LINK = 1; //��������

            if (!flag1)
            {
                flag1 = 1;
                flag2 = 0;
                printf("\r\nble connect!\r\n");
            }
        }
        else
        {
            BLE_LINK = 0; //�����Ͽ�

            if (!flag2)
            {

                flag2 = 1;
                flag1 = 0;
                printf("\r\nble disconnect!\r\n");
            }
        }

        t++;
        if (t == 20)
        {
            t = 0;
            LED1 = ~LED1;
        }
        delay_ms(10);

    }

}

//�����Ժ���
void Ble_Test(void)
{

    Ble_IoInit(); //BLE���ų�ʼ��
    Ble_SetInit();//BLE��������
    Ble_Process();//���ݲ���

}
