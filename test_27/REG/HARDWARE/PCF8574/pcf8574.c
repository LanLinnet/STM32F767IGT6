#include "pcf8574.h"
#include "delay.h"

//初始化PCF8574
//返回值：0-初始化成功 1-初始化失败
u8 PCF8574_Init(void)
{
	u8 temp=0;
	RCC->AHB1ENR|=1<<1;
	GPIO_Set(GPIOB,PIN12,GPIO_MODE_IN,0,0,GPIO_PUPD_PU);
	IIC_Init();
	//检查PCF8574是否在位
	IIC_Start();
	IIC_Send_Byte(PCF8574_ADDR);
	temp=IIC_Wait_Ack();		//ACK应答，判断状态
	IIC_Stop();
	PCF8574_WriteOneByte(0xFF);
	return temp;
}

//读取PCF8574的8位IO值，并返回读到的数据
u8 PCF8574_ReadOneByte(void)
{
	u8 temp=0;
	IIC_Start();
	IIC_Send_Byte(PCF8574_ADDR|0x01);		//进入接收模式
	IIC_Wait_Ack();
	temp=IIC_Read_Byte(0);
	IIC_Stop();
	return temp;
}

//向PCF8574写入8位IO值
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

//设置PCF8574某个IO的高低电平
void PCF8574_WriteBit(u8 bit, u8 sta)
{
	u8 data;
	data=PCF8574_ReadOneByte();
	if(sta==0)data&=~(1<<bit);		//把该位清零
	else data|=1<<bit;				//把该位置1
	PCF8574_WriteOneByte(data);
}

//读取PCF8574的某个IO值
u8 PCF8574_ReadBit(u8 bit)
{
	u8 data;
	data=PCF8574_ReadOneByte();
	if(data&(1<<bit))return 1;
	else return 0;
}
