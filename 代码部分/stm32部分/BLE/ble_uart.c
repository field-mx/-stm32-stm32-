#include "ble_uart.h"
#include "delay.h"
#include "string.h"
#include "USART2.h"

//BLE���������,�����յ���Ӧ��
//str:�ڴ���Ӧ����
//����ֵ:0,û�еõ��ڴ���Ӧ����
//����,�ڴ�Ӧ������λ��(str��λ��)
u8 *ble_check_cmd(u8 *str)
{
    char *strx = 0;

    if (USART2_RX_STA & 0X8000) //���յ�һ��������
    {
        USART2_RX_BUF[USART2_RX_STA & 0X7FFF] = 0; //��ӽ�����
        strx = strstr((const char *)USART2_RX_BUF, (const char *)str);
    }    
    return (u8 *)strx;
}
//BLE��������
//cmd:���͵������ַ���(����Ҫ��ӻس���),��cmd<0XFF��ʱ��,��������(���緢��0X1A),���ڵ�ʱ�����ַ���.
//ack:�ڴ���Ӧ����,���Ϊ��,���ʾ����Ҫ�ȴ�Ӧ��
//waittime:�ȴ�ʱ��(��λ:10ms)
//����ֵ:0,���ͳɹ�(�õ����ڴ���Ӧ����)
//       1,����ʧ��
u8 ble_send_cmd(u8 *cmd, u8 *ack, u16 waittime)
{
    u8 res = 0;
    USART2_RX_STA = 0;

    if ((u32)cmd <= 0XFF)
    {
        while ((USART2->SR & 0X40) == 0); //�ȴ���һ�����ݷ������

        USART2->DR = (u32)cmd;
    }
    else u3_printf2("%s\r\n", cmd);//��������

    if (ack && waittime) //��Ҫ�ȴ�Ӧ��
    {
        while (--waittime) //�ȴ�����ʱ
        {
            delay_ms(10);

            if (USART2_RX_STA & 0X8000) //���յ��ڴ���Ӧ����
            {
                if (ble_check_cmd(ack))
                {
                   USART2_RX_STA = 0;
                   break; //�õ���Ч����
                }

                USART2_RX_STA = 0;
            }
        }

        if (waittime == 0)res = 1;
    }

    return res;
}


