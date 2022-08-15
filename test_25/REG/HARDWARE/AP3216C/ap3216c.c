#include "ap3216c.h"
#include "myiic.h"
#include "delay.h"

//��ʼ��AP3216C
//����ֵ��0-�ɹ� 1-ʧ��
u8 AP3216C_Init(void)
{
	u8 temp=0;
	IIC_Init();
	AP3216C_WriteOneByte(0x00, 0x04);		//��λ
	delay_ms(50);
	AP3216C_WriteOneByte(0x00, 0x03);		//ALS+PS+IR���ܼ���
	temp=AP3216C_ReadOneByte(0x00);
	if(temp==0x03) return 0;		//����
	else return 1;					//ʧ��
}

//��ȡAP3216C������
//�������ݶ�ȡ��ʱ����Ҫ����112.5ms
void AP3216C_ReadData(u16 *ir, u16 *ps, u16 *als)
{
	u8 buf[6];
	u8 i;
	//IR
	for(i=0;i<6;i++) buf[i]=AP3216C_ReadOneByte(0x0A+i);	
	if(buf[0]&0x80) *ir=0;  //IR������Ч
	else *ir=((u16)buf[1]<<2)|(buf[0]&0x03);		//��ȡIR������������
	//ALS
	*als=((u16)buf[3]<<8)|buf[2];
	//PS
	if(buf[4]&0x40) *ps=0;	//PS������Ч
	else *ps=((u16)(buf[5]&0x3f)<<4)|(buf[4]&0x0f);
}

//IICдһ���ֽ�
//����ֵ��0-����  ����-�������
u8 AP3216C_WriteOneByte(u8 reg, u8 data)
{
	IIC_Start();
	IIC_Send_Byte(AP3216C_ADDR|0x00);	//����������ַ+д����
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

//IIC��һ���ֽڣ�������
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
	res=IIC_Read_Byte(0);		//�����ݣ�����nACK
	IIC_Stop();
	return res;
}
