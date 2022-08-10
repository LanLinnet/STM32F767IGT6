#include "24cxx.h"
#include "delay.h"

//初始化IIC接口
void AT24CXX_Init(void)
{
	IIC_Init();
}

//在AT24CXX指定地址读出一个数据并返回
//ReadAddr：开始读数的地址
u8 AT24CXX_ReadOneByte(u16 ReadAddr)
{
	u8 temp=0;
	IIC_Start();
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0xA0);
		IIC_Wait_Ack();
		IIC_Send_Byte(ReadAddr>>8);		//发送器件（芯片）地址(高地址)
	}else IIC_Send_Byte(0xA0+((ReadAddr/256)<<1));		//发送器件地址0xA0，写数据，除以256右移8位（获得高8位）
	IIC_Wait_Ack();
	IIC_Send_Byte(ReadAddr%256);		//发送低地址
	IIC_Wait_Ack();
	IIC_Start();
	IIC_Send_Byte(0xA1);				//发送器件地址，读
	IIC_Wait_Ack();
	temp=IIC_Read_Byte(0);				//读取数据
	IIC_Stop();
	return temp;
}

//在AT24CXX指定地址写入一个数据
void AT24CXX_WriteOneByte(u16 WriteAddr, u8 DataToWrite)
{
	IIC_Start();
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0xA0);
		IIC_Wait_Ack();
		IIC_Send_Byte(WriteAddr>>8);	//发送高地址
	}else IIC_Send_Byte(0xA0+((WriteAddr/256)<<1));		//发送器件地址  0XA0写数据
	IIC_Wait_Ack();
	IIC_Send_Byte(WriteAddr%256);		//发送低地址
	IIC_Wait_Ack();
	IIC_Send_Byte(DataToWrite);			//发送字节
	IIC_Wait_Ack();
	IIC_Stop();
	delay_ms(10);		//EEPROM写入过程比较慢，需要等待一段时间
}

//在AT24CXX里的指定地址写入长度位Len的数据
void AT24CXX_WriteLenByte(u16 WriteAddr, u32 DataToWrite, u8 Len)
{
	u8 t;
	for(t=0;t<Len;t++)
	{
		AT24CXX_WriteOneByte(WriteAddr+t,(DataToWrite>>(8*t))&0xff);		//一个字节一个字节写入，由低到高
	}
}

//在AT24CXX里指定地址开始读出长度为Len的数据
u32 AT24CXX_ReadLenByte(u16 ReadAddr, u8 Len)
{
	u8 t;
	u32 temp=0;
	for(t=0; t<Len; t++)
	{
		temp<<=8;
		temp+=AT24CXX_ReadOneByte(ReadAddr+Len-t-1);
	}
	return temp;
}

//检查AT24CXX是否正常
//返回值：1-失败 0-成功
u8 AT24CXX_Check(void)
{
	u8 temp;
	temp=AT24CXX_ReadOneByte(255);
	if(temp==0x55) return 0;
	else{
		AT24CXX_WriteOneByte(255,0x55);
		temp=AT24CXX_ReadOneByte(255);
		if(temp==0x55)return 0;
	}
	return 1;
}

//在AT24CXX指定地址读指定个数数据
void AT24CXX_Read(u16 ReadAddr, u8 *pBuffer, u16 NumToRead)
{
	while(NumToRead)
	{
		*pBuffer++=AT24CXX_ReadOneByte(ReadAddr++);
		NumToRead--;
	}
}

//在AT24CXX里指定地址写入指定个数数据
void AT24CXX_Write(u16 WriteAddr, u8 *pBuffer, u16 NumToWrite)
{
	while(NumToWrite--)
	{
		AT24CXX_WriteOneByte(WriteAddr,*pBuffer);
		WriteAddr++;
		pBuffer++;
	}
}

