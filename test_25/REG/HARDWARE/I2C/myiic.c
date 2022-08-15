#include "myiic.h"
#include "delay.h"

//初始化IIC
void IIC_Init(void)
{
	RCC->AHB1ENR|=1<<7;
	GPIO_Set(GPIOH,PIN4|PIN5,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PU);
	IIC_SCL(1);
	IIC_SDA(1);
}

//产生IIC起始信号
void IIC_Start(void)
{
	SDA_OUT();
	IIC_SDA(1);
	IIC_SCL(1);
	delay_us(4);
	IIC_SDA(0);		//SCL为高电平时，SDA下降沿开始IIC
	delay_us(4);
	IIC_SCL(0);		//钳住I2C总线，准备发送或接收数据
}

//产生IIC停止信号
void IIC_Stop(void)
{
	SDA_OUT();
	IIC_SCL(0);
	IIC_SDA(0);
	delay_us(4);
	IIC_SCL(1);
	delay_us(4);
	IIC_SDA(1);		//SCL为高电平时，SDA上升沿停止IIC
}

//等待应答信号到来
//返回值：1-失败 0-成功
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

//产生ACK应答
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

//不产生ACK应答
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

//IIC发送一个字节
//返回从机：1-有应答 0-无应答
void IIC_Send_Byte(u8 txd)
{
	u8 t;
	SDA_OUT();
	IIC_SCL(0);		//拉低时钟开始数据传输
	for(t=0;t<8;t++)
	{
		IIC_SDA((txd&0X80)>>7);		//取最高位数并写入SDA
		txd<<=1;		//每次左移都把下一位放到最高位上
		delay_us(2);
		IIC_SCL(1);
		delay_us(2);
		IIC_SCL(0);
		delay_us(2);
	}
}

//IIC读一个字节并返回
//ack：1-发送ACK  0-发送nACK
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
