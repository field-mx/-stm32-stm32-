#ifndef __BLE_UART_H__
#define __BLE_UART_H__
#include "sys.h"

u8 *ble_check_cmd(u8 *str);
u8 ble_send_cmd(u8 *cmd, u8 *ack, u16 waittime);


#endif





