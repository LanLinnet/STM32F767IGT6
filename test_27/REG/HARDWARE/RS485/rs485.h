#ifndef __RS485_H
#define __RS485_H

#include "sys.h"

extern u8 RS485_RX_BUF[64];		//���ջ��壬���64���ֽ�
extern u8 RS485_RX_CNT;			//���յ������ݳ���

#define EN_USART2_RX 1  //0-������  1-���մ����ж�

void RS485_Init(u32 pclk1, u32 bound);
void RS485_Send_Data(u8 *buf, u8 len);
void RS485_Receive_Data(u8 *buf, u8 *len);
void RS485_TX_Set(u8 en);

#endif
