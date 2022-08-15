#include "ap3216c.h"
#include "myiic.h"
#include "delay.h"

//初始化AP3216C
//返回值：0-成功 1-失败
u8 AP3216C_Init(void)
{
	u8 temp=0;
	IIC_Init();
	AP3216C_WriteOneByte(0x00, 0x04);		//软复位
	delay_ms(50);
	AP3216C_WriteOneByte(0x00, 0x03);		//ALS+PS+IR功能激活
	temp=AP3216C_ReadOneByte(0x00);
	if(temp==0x03) return 0;		//正常
	else return 1;					//失败
}

//读取AP3216C的数据
//两次数据读取的时间间隔要大于112.5ms
void AP3216C_ReadData(u16 *ir, u16 *ps, u16 *als)
{
	u8 buf[6];
	u8 i;
	//IR
	for(i=0;i<6;i++) buf[i]=AP3216C_ReadOneByte(0x0A+i);	
	if(buf[0]&0x80) *ir=0;  //IR数据无效
	else *ir=((u16)buf[1]<<2)|(buf[0]&0x03);		//读取IR传感器的数据
	//ALS
	*als=((u16)buf[3]<<8)|buf[2];
	//PS
	if(buf[4]&0x40) *ps=0;	//PS数据无效
	else *ps=((u16)(buf[5]&0x3f)<<4)|(buf[4]&0x0f);
}

//IIC写一个字节
//返回值：0-正常  其它-错误代码
u8 AP3216C_WriteOneByte(u8 reg, u8 data)
{
	IIC_Start();
	IIC_Send_Byte(AP3216C_ADDR|0x00);	//发送器件地址+写命令
	if(IIC_Wait_Ack())
	{
		IIC_Stop();
		return 1;
	}
	IIC_Send_Byte(reg);
	IIC_Wait_Ack();
	IIC_Send_Byte(data);
	if(IIC_Wait_Ack())
	{
		IIC_Stop();
		return 1;
	}
	IIC_Stop();
	return 0;
}

//IIC读一个字节，并返回
u8 AP3216C_ReadOneByte(u8 reg)
{
	u8 res;
	IIC_Start();
	IIC_Send_Byte(AP3216C_ADDR|0x00);
	IIC_Wait_Ack();
	IIC_Send_Byte(reg);
	IIC_Wait_Ack();
	IIC_Start();
	IIC_Send_Byte(AP3216C_ADDR|0x01);
	IIC_Wait_Ack();
	res=IIC_Read_Byte(0);		//读数据，发送nACK
	IIC_Stop();
	return res;
}
