
#include "stm32f10x.h"

//������ʱ�Ӷ˿ڡ����Ŷ���
#define BEEP_PORT 			GPIOB
#define BEEP_PIN 			GPIO_Pin_8
#define BEEP_PORT_RCC		RCC_APB2Periph_GPIOB

#define BEEP PBout(8)

void BEEP_Init(void);











