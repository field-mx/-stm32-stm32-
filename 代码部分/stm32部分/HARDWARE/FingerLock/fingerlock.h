
#include "stm32f10x.h"

//�ṹ�嶨��
typedef struct
{
	u8 mode;//ģʽ��0��Ĭ��ˢָ��ģʽ��1�����ָ�ƣ�2��ɾ��ָ��
}_finger;
extern _finger finger;

//��������
void main_window_show(void);
void finger_lock(void);
