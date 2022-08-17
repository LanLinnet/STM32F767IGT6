#include "qspi.h"

//等待状态标志
//返回值：0-成功  1-失败
u8 QSPI_Wait_Flag(u32 flag, u8 sta, u32 wtime)
{
	u8 flagsta=0;
	while(wtime)
	{
		flagsta=(QUADSPI->SR&flag)?1:0;
		if(flagsta==sta) break;
		wtime--;
	}
	if(wtime) return 0;
	else return 1;
}

//初始化QSPI
//返回值：0-成功  1-失败
u8 QSPI_Init(void)
{
	u32 tempreg=0;
	RCC->AHB1ENR|=1<<1;
	RCC->AHB1ENR|=1<<5;
	RCC->AHB3ENR|=1<<1;
	
	GPIO_Set(GPIOB,1<<2,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);	//PB2复用功能输出	
	GPIO_Set(GPIOB,1<<6,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);	//PB6复用功能输出	
	GPIO_Set(GPIOF,0XF<<6,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);	//PF6~9复用功能输出	
	
	GPIO_AF_Set(GPIOB,2,9);		//PB2,AF9
 	GPIO_AF_Set(GPIOB,6,10);	//PB6,AF10
 	GPIO_AF_Set(GPIOF,6,9);		//PF6,AF9 
 	GPIO_AF_Set(GPIOF,7,9);		//PF7,AF9
 	GPIO_AF_Set(GPIOF,8,10);	//PF8,AF10
 	GPIO_AF_Set(GPIOF,9,10);	//PF9,AF10
	
	RCC->AHB3RSTR|=1<<1;
	RCC->AHB3RSTR&=~(1<<1);
	if(QSPI_Wait_Flag(1<<5,0,0xffff)==0)	//等待BUSY空闲
	{
		//QUADSPI_CR
		tempreg=(3-1)<<24;	//三分频
		tempreg|=(4-1)<<8;	//FIFO阈值四个字节
		tempreg|=0<<7;
		tempreg|=0<<6;
		tempreg|=1<<4;
		QUADSPI->CR=tempreg;
		//QUADSPI_DCR
		tempreg=(25-1)<<16;
		tempreg|=(4-1)<<8;
		tempreg|=1<<0;
		QUADSPI->DCR=tempreg;
		QUADSPI->CR|=1<<0;
	}else return 1;
	return 0;
}

//QSPI发送命令
//cmd:要发送的指令
//addr:发送到的目的地址
//mode:模式,详细位定义如下:
//	mode[1:0]:指令模式;00,无指令;01,单线传输指令;10,双线传输指令;11,四线传输指令.
//	mode[3:2]:地址模式;00,无地址;01,单线传输地址;10,双线传输地址;11,四线传输地址.
//	mode[5:4]:地址长度;00,8位地址;01,16位地址;10,24位地址;11,32位地址.
//	mode[7:6]:数据模式;00,无数据;01,单线传输数据;10,双线传输数据;11,四线传输数据.
//dmcycle:空指令周期数
void QSPI_Send_CMD(u8 cmd, u32 addr, u8 mode, u8 dmcycle)
{
	u32 tempreg=0;
	u8 status;
	if(QSPI_Wait_Flag(1<<5,0,0xffff)==0)
	{
		//QUADSPI_CCR
		tempreg=0<<31;
		tempreg|=0<<28;
		tempreg|=0<<26;
		tempreg|=((u32)mode>>6)<<24;
		tempreg|=(u32)dmcycle<<18;
		tempreg|=((u32)(mode>>4)&0x03)<<12;
		tempreg|=((u32)(mode>>2)&0x03)<<10;
		tempreg|=((u32)(mode>>0)&0x03)<<8;
		tempreg|=cmd;
		QUADSPI->CCR=tempreg;
		
		if(mode&0x0c)		//有指令/地址要发送
		{
			QUADSPI->AR=addr;
		}
		if((mode&0xc0)==0)	//无数据传输
		{
			status=QSPI_Wait_Flag(1<<1,1,0xffff);		//TCF位，是否传输完成
			if(status==0)
			{
				QUADSPI->FCR|=1<<1;		//清除标志位
			}
		}
	}
}

//QSPI接收指定长度的数据
//buf:接收数据缓冲区首地址
//datalen:要传输的数据长度
//返回值:0,正常  其他,错误代码
u8 QSPI_Receive(u8 *buf, u32 datalen)
{
	u32 tempreg=QUADSPI->CCR;
	u32 addreg = QUADSPI->AR;
	u8 status;
	vu32 *data_reg = &QUADSPI->DR;
	QUADSPI->DLR=datalen-1;
	//QUADSPI_CCR
	tempreg&=~(3<<26);
	tempreg|=1<<26;
	QUADSPI->FCR|=1<<1;
	QUADSPI->CCR=tempreg;
	QUADSPI->AR=addreg;
	while(datalen)
	{
		status=QSPI_Wait_Flag(3<<1,1,0xffff);		//接收到了数据
		if(status==0)		//等待成功
		{
			*buf++=*(vu8 *)data_reg;
			datalen--;
		}else break;
	}
	if(status==0)
	{
		QUADSPI->CR|=1<<2;		
		status=QSPI_Wait_Flag(1<<1,1,0xffff);
		if(status==0)		//传输已经完成
		{
			QUADSPI->FCR|=1<<1;
			status=QSPI_Wait_Flag(1<<5,0,0xffff);
		}
	}
	return status;
}

//QSPI发送指定长度的数据
//buf:发送数据缓冲区首地址
//datalen:要传输的数据长度
//返回值:0,正常
//    其他,错误代码
u8 QSPI_Transmit(u8 *buf, u32 datalen)
{
	u32 tempreg=QUADSPI->CCR;
	u32 addrreg=QUADSPI->AR; 
	u8 status;
	vu32 *data_reg=&QUADSPI->DR;
	QUADSPI->DLR=datalen-1;
	tempreg&=~(3<<26);
	tempreg|=0<<26;
	QUADSPI->FCR|=1<<1;
	QUADSPI->CCR=tempreg;
	while(datalen)
	{
		status=QSPI_Wait_Flag(1<<2,1,0xffff);
		if(status!=0) break;
		*(vu8 *)data_reg=*buf++;
		datalen--;
	}
	if(status==0)
	{
		QUADSPI->CR|=1<<2;							//终止传输 
		status=QSPI_Wait_Flag(1<<1,1,0XFFFF);		//等待TCF,即数据传输完成
		if(status==0)
		{
			QUADSPI->FCR|=1<<1;						//清除TCF标志位 
			status=QSPI_Wait_Flag(1<<5,0,0XFFFF);	//等待BUSY位清零
		}
	}
	return status;
}

