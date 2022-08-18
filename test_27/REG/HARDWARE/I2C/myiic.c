#include "myiic.h"
#include "delay.h"

//��ʼ��IIC
void IIC_Init(void)
{
	RCC->AHB1ENR|=1<<7;
	GPIO_Set(GPIOH,PIN4|PIN5,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PU);
	IIC_SCL(1);
	IIC_SDA(1);
}

//����IIC��ʼ�ź�
void IIC_Start(void)
{
	SDA_OUT();
	IIC_SDA(1);
	IIC_SCL(1);
	delay_us(4);
	IIC_SDA(0);		//SCLΪ�ߵ�ƽʱ��SDA�½��ؿ�ʼIIC
	delay_us(4);
	IIC_SCL(0);		//ǯסI2C���ߣ�׼�����ͻ��������
}

//����IICֹͣ�ź�
void IIC_Stop(void)
{
	SDA_OUT();
	IIC_SCL(0);
	IIC_SDA(0);
	delay_us(4);
	IIC_SCL(1);
	delay_us(4);
	IIC_SDA(1);		//SCLΪ�ߵ�ƽʱ��SDA������ֹͣIIC
}

//�ȴ�Ӧ���źŵ���
//����ֵ��1-ʧ�� 0-�ɹ�
u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	SDA_IN();
	IIC_SDA(1);
	delay_us(1);
	IIC_SCL(1);
	delay_us(1);
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL(0);
	return 0;
}

//����ACKӦ��
void IIC_Ack(void)
{
	IIC_SCL(0);
	SDA_OUT();
	IIC_SDA(0);
	delay_us(2);
	IIC_SCL(1);
	delay_us(2);
	IIC_SCL(0);
}

//������ACKӦ��
void IIC_NAck(void)
{
	IIC_SCL(0);
	SDA_OUT();
	IIC_SDA(1);
	delay_us(2);
	IIC_SCL(1);
	delay_us(2);
	IIC_SCL(0);
}

//IIC����һ���ֽ�
//���شӻ���1-��Ӧ�� 0-��Ӧ��
void IIC_Send_Byte(u8 txd)
{
	u8 t;
	SDA_OUT();
	IIC_SCL(0);		//����ʱ�ӿ�ʼ���ݴ���
	for(t=0;t<8;t++)
	{
		IIC_SDA((txd&0X80)>>7);		//ȡ���λ����д��SDA
		txd<<=1;		//ÿ�����ƶ�����һλ�ŵ����λ��
		delay_us(2);
		IIC_SCL(1);
		delay_us(2);
		IIC_SCL(0);
		delay_us(2);
	}
}

//IIC��һ���ֽڲ�����
//ack��1-����ACK  0-����nACK
u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i, receive=0;
	SDA_IN();
	for(i=0;i<8;i++)
	{
		IIC_SCL(0);
		delay_us(2);
		IIC_SCL(1);
		receive<<=1;
		if(READ_SDA) receive++;
		delay_us(1);
	}
	if(!ack)IIC_NAck();
	else IIC_Ack();
	return receive;
}
