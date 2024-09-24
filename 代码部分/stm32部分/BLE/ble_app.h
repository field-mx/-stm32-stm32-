#ifndef _BLE_APP_H_
#define _BLE_APP_H_

#include "sys.h"
#include "ble_cfg.h"

#define BLE_STA   PAin(4)   
#define BLE_WKUP  PAout(15)  

void Ble_Test(void);
void Ble_SendData(void);
void Ble_Process(void);
void Ble_IoInit(void); //BLE���ų�ʼ��
void Ble_SetInit(void);//BLE��������
void Ble_Sendpic(u8 color);
#endif

