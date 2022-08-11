#include "pcf8574.h"
#include "delay.h"

//��ʼ��PCF8574
//����ֵ��0-��ʼ���ɹ� 1-��ʼ��ʧ��
u8 PCF8574_Init(void)
{
	u8 temp=0;
	RCC->AHB1ENR|=1<<1;
	GPIO_Set(GPIOB,PIN12,GPIO_MODE_IN,0,0,GPIO_PUPD_PU);
	IIC_Init();
	//���PCF8574�Ƿ���λ
	IIC_Start();
	IIC_Send_Byte(PCF8574_ADDR);
	temp=IIC_Wait_Ack();		//ACKӦ���ж�״̬
	IIC_Stop();
	PCF8574_WriteOneByte(0xFF);
	return temp;
}

//��ȡPCF8574��8λIOֵ�������ض���������
u8 PCF8574_ReadOneByte(void)
{
	u8 temp=0;
	IIC_Start();
	IIC_Send_Byte(PCF8574_ADDR|0x01);		//�������ģʽ
	IIC_Wait_Ack();
	temp=IIC_Read_Byte(0);
	IIC_Stop();
	return temp;
}

//��PCF8574д��8λIOֵ
void PCF8574_WriteOneByte(u8 DataToWrite)
{
	IIC_Start();
	IIC_Send_Byte(PCF8574_ADDR|0x00);
	IIC_Wait_Ack();
	IIC_Send_Byte(DataToWrite);
	IIC_Wait_Ack();
	IIC_Stop();
	delay_ms(10);
}

//����PCF8574ĳ��IO�ĸߵ͵�ƽ
void PCF8574_WriteBit(u8 bit, u8 sta)
{
	u8 data;
	data=PCF8574_ReadOneByte();
	if(sta==0)data&=~(1<<bit);		//�Ѹ�λ����
	else data|=1<<bit;				//�Ѹ�λ��1
	PCF8574_WriteOneByte(data);
}

//��ȡPCF8574��ĳ��IOֵ
u8 PCF8574_ReadBit(u8 bit)
{
	u8 data;
	data=PCF8574_ReadOneByte();
	if(data&(1<<bit))return 1;
	else return 0;
}
