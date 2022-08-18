#ifndef __RS485_H
#define __RS485_H

#include "sys.h"

extern u8 RS485_RX_BUF[64];		//接收缓冲，最大64个字节
extern u8 RS485_RX_CNT;			//接收到的数据长度

#define EN_USART2_RX 1  //0-不接收  1-接收串口中断

void RS485_Init(u32 pclk1, u32 bound);
void RS485_Send_Data(u8 *buf, u8 len);
void RS485_Receive_Data(u8 *buf, u8 *len);
void RS485_TX_Set(u8 en);

#endif
