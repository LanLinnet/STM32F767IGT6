#ifndef __MYIIC_H
#define __MYIIC_H

#include "sys.h"

//IO方向设置
#define SDA_IN() {GPIOH->MODER&=~(3<<(5*2)); GPIOH->MODER|=0<<5*2;}		//输入
#define SDA_OUT() {GPIOH->MODER&=~(3<<(5*2)); GPIOH->MODER|=1<<5*2;}		//输出

//IO操作函数
#define IIC_SCL(x)	GPIO_Pin_Set(GPIOH, PIN4, x)		//SCL
#define IIC_SDA(x)	GPIO_Pin_Set(GPIOH, PIN5, x)		//SDA
#define READ_SDA	GPIO_Pin_Get(GPIOH, PIN5)			//读取SDA

//IIC所有操作函数
void IIC_Init(void);
void IIC_Start(void);
void IIC_Stop(void);
void IIC_Send_Byte(u8 txd);
u8 IIC_Read_Byte(unsigned char ack);
u8 IIC_Wait_Ack(void);
void IIC_Ack(void);
void IIC_NAck(void);
#endif
