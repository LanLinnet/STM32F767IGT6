#include "sdram.h"
#include "delay.h"

//向SDRAM发送命令
//bankx-BANK5/BANK6  cmd-指令  refresh-自刷新次数  regval-模式寄存器的定义
//返回值：0-正常  1-失败
u8 SDRAM_Send_Cmd(u8 bankx, u8 cmd, u8 refresh, u16 regval)
{
	u32 retry=0;		//指令发送重复尝试次数
	u32 tempreg=0;		//存放命令临时变量
	tempreg|=cmd<<0;		//设置指令
	tempreg|=1<<(4-bankx);	//设置发送到BANK5/BANK6
	tempreg|=refresh<<5;	//设置自刷新次数
	tempreg|=regval<<9;		//设置SDRAM模式寄存器的内容
	FMC_Bank5_6->SDCMR=tempreg;
	
	while((FMC_Bank5_6->SDSR&(1<<5)))		//等待指令发送完成
	{
		retry++;
		if(retry>0x1fffff)
			return 1;
	}
	return 0;
}

//SDRAM初始化
void SDRAM_Init(void)
{
	u32 sdctrlreg=0;		//FMC_SDCRx 控制寄存器 临时变量
	u32 sdtimereg=0;		//FMC_SDTRx 时序寄存器 临时变量
	u16 mregval=0;			//SDRAM的模式寄存器 临时变量
	
	RCC->AHB3ENR|=1<<0;		//使能FMC时钟
	RCC->AHB1ENR|=0x1f<<2;		//使能GPIOC/D/E/F/G时钟
	
	GPIO_Set(GPIOC,PIN0|PIN2|PIN3,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);			//PC0/2/3			
	GPIO_Set(GPIOD,3<<0|7<<8|3<<14,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);		//PD0/1/8/9/10/14/15		
	GPIO_Set(GPIOE,3<<0|0X1FF<<7,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);			//PE0/1/7~15				
	GPIO_Set(GPIOF,0X3F<<0|0X1F<<11,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);		//PG0~5/11~15					
	GPIO_Set(GPIOG,7<<0|3<<4|PIN8|PIN15,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);	//PF0~2/4/5/8/15
	
	GPIO_AF_Set(GPIOC,0,12);	//PC0,AF12
 	GPIO_AF_Set(GPIOC,2,12);	//PC2,AF12
 	GPIO_AF_Set(GPIOC,3,12);	//PC3,AF12
	
 	GPIO_AF_Set(GPIOD,0,12);	//PD0,AF12 
 	GPIO_AF_Set(GPIOD,1,12);	//PD1,AF12 
 	GPIO_AF_Set(GPIOD,8,12);	//PD8,AF12
 	GPIO_AF_Set(GPIOD,9,12);	//PD9,AF12
 	GPIO_AF_Set(GPIOD,10,12);	//PD10,AF12  
 	GPIO_AF_Set(GPIOD,14,12);	//PD14,AF12
 	GPIO_AF_Set(GPIOD,15,12);	//PD15,AF12
	
 	GPIO_AF_Set(GPIOE,0,12);	//PE0,AF12 
 	GPIO_AF_Set(GPIOE,1,12);	//PE1,AF12 
 	GPIO_AF_Set(GPIOE,7,12);	//PE7,AF12
 	GPIO_AF_Set(GPIOE,8,12);	//PE8,AF12
 	GPIO_AF_Set(GPIOE,9,12);	//PE9,AF12
 	GPIO_AF_Set(GPIOE,10,12);	//PE10,AF12
 	GPIO_AF_Set(GPIOE,11,12);	//PE11,AF12
 	GPIO_AF_Set(GPIOE,12,12);	//PE12,AF12
 	GPIO_AF_Set(GPIOE,13,12);	//PE13,AF12
 	GPIO_AF_Set(GPIOE,14,12);	//PE14,AF12
 	GPIO_AF_Set(GPIOE,15,12);	//PE15,AF12

 	GPIO_AF_Set(GPIOF,0,12);	//PF0,AF12 
 	GPIO_AF_Set(GPIOF,1,12);	//PF1,AF12 
 	GPIO_AF_Set(GPIOF,2,12);	//PF2,AF12
 	GPIO_AF_Set(GPIOF,3,12);	//PF3,AF12
 	GPIO_AF_Set(GPIOF,4,12);	//PF4,AF12
 	GPIO_AF_Set(GPIOF,5,12);	//PF5,AF12
 	GPIO_AF_Set(GPIOF,11,12);	//PF11,AF12
 	GPIO_AF_Set(GPIOF,12,12);	//PF12,AF12
 	GPIO_AF_Set(GPIOF,13,12);	//PF13,AF12
 	GPIO_AF_Set(GPIOF,14,12);	//PF14,AF12
 	GPIO_AF_Set(GPIOF,15,12);	//PF15,AF12
	
 	GPIO_AF_Set(GPIOG,0,12);	//PG0,AF12 
 	GPIO_AF_Set(GPIOG,1,12);	//PG1,AF12 
 	GPIO_AF_Set(GPIOG,2,12);	//PG2,AF12
 	GPIO_AF_Set(GPIOG,4,12);	//PG4,AF12
 	GPIO_AF_Set(GPIOG,5,12);	//PG5,AF12  
 	GPIO_AF_Set(GPIOG,8,12);	//PG8,AF12
 	GPIO_AF_Set(GPIOG,15,12);	//PG15,AF12	
	
 	sdctrlreg|=1<<0;				//9位列地址
	sdctrlreg|=2<<2;				//13位行地址
	sdctrlreg|=1<<4;				//16位数据位宽
	sdctrlreg|=1<<6;				//4个内部存区(4 BANKS)
	sdctrlreg|=3<<7;				//3个CAS延迟
	sdctrlreg|=0<<9;				//允许写访问
	sdctrlreg|=2<<10;				//SDRAM时钟=HCLK/2=216M/2=108M=9.3ns
	sdctrlreg|=1<<12;				//使能突发访问 
	sdctrlreg|=0<<13;				//读通道延迟0个HCLK
 	FMC_Bank5_6->SDCR[0]=sdctrlreg;	//设置FMC BANK5 SDRAM控制寄存器(BANK5和6用于管理SDRAM).
	
	sdtimereg|=1<<0;				//加载模式寄存器到激活时间的延迟为2个时钟周期
	sdtimereg|=7<<4;				//退出自刷新延迟为8个时钟周期
	sdtimereg|=6<<8;				//自刷新时间为7个时钟周期
	sdtimereg|=6<<12;				//行循环延迟为7个时钟周期
	sdtimereg|=1<<16;				//恢复延迟为2个时钟周期
	sdtimereg|=1<<20;				//行预充电延迟为2个时钟周期
	sdtimereg|=1<<24;				//行到列延迟为2个时钟周期
 	FMC_Bank5_6->SDTR[0]=sdtimereg;	//设置FMC BANK5 SDRAM时序寄存器 
	
	SDRAM_Send_Cmd(0,1,0,0);		//时钟配置使能
	delay_us(500);					//至少延迟200us.
	SDRAM_Send_Cmd(0,2,0,0);		//对所有存储区预充电
	SDRAM_Send_Cmd(0,3,8,0);		//设置自刷新次数 
	mregval|=3<<0;					//设置突发长度:8(可以是1/2/4/8)
	mregval|=0<<3;					//设置突发类型:连续(可以是连续/交错)
	mregval|=3<<4;					//设置CAS值:3(可以是2/3)
	mregval|=0<<7;					//设置操作模式:0,标准模式
	mregval|=1<<9;					//设置突发写模式:1,单点访问
	SDRAM_Send_Cmd(0,4,0,mregval);	//设置SDRAM的模式寄存器
	
	//我们使用的SDRAM刷新周期为64ms,SDCLK=216/2=108Mhz,行数为8192(2^13).
	
	//刷新频率计数器COUNT值计算方法:
	//刷新速率（us） = 刷新周期/行数 = （实际COUNT+1）*SDRAM频率时钟
	//如果SDRAM在接受请求后，出现内部刷新请求，则必须将刷新频率增加20个SDRAM时钟周期，以获取充足的余量
	//所以 设计的COUNT=计算出的实际COUNT-20
	
	//刷新速率=64ms/8192=7.81us
	//SDRAM时钟频率=216/2=108Mhz
	//实际COUNT=刷新速率*SDRAM时钟频率+1=7.81us*108Mhz+1=844.48≈844
	//设计Count=844-20=824

	FMC_Bank5_6->SDRTR=824<<1;		//设置刷新频率计数器
}

//在指定地址(WriteAddr+Bank5_SDRAM_ADDR)开始写入n个字节
void FMC_SDRAM_WriteBuffer(u8 *pBuffer, u32 WriteAddr, u32 n)
{
	for(;n!=0;n--)
	{
		*(vu8*)(Bank5_SDRAM_ADDR+WriteAddr) = *pBuffer;
		WriteAddr++;
		pBuffer++;
	}
}

//在指定地址(WriteAddr+Bank5_SDRAM_ADDR)开始读出n个字节
void FMC_SDRAM_ReadBuffer(u8 *pBuffer, u32 ReadAddr, u32 n)
{
	for(;n!=0;n--)
	{
		*pBuffer++ = *(vu8*)(Bank5_SDRAM_ADDR+ReadAddr);
		ReadAddr++;
	}
}

//针对使用RGB屏时的设置
//对于MCU屏,必须设置CAS=3,否则SDRAM驱动异常
//对于RGB屏,可以设置CAS=2,以提高SDRAM访问速度
void SDRAM_RGBLCD_Set(void)
{
	u32 mregval = 0;
	FMC_Bank5_6->SDCR[0]&=~(3<<7);		//清除原来的CAS延迟设置
	FMC_Bank5_6->SDCR[0]&= 2<<7;		//设置CAS延迟为2
	
	mregval|=3<<0;					//设置突发长度:8(可以是1/2/4/8)
	mregval|=0<<3;					//设置突发类型:连续(可以是连续/交错)
	mregval|=2<<4;					//设置CAS值:2(可以是2/3)
	mregval|=0<<7;					//设置操作模式:0,标准模式
	mregval|=1<<9;					//设置突发写模式:1,单点访问
	SDRAM_Send_Cmd(0,4,0,mregval);	//设置SDRAM的模式寄存器
}
