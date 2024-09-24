
#include "stm32f10x.h"

//结构体定义
typedef struct
{
	u8 mode;//模式，0：默认刷指纹模式，1：添加指纹，2：删除指纹
}_finger;
extern _finger finger;

//函数声明
void main_window_show(void);
void finger_lock(void);
