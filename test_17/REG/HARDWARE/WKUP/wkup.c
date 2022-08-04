#include "wkup.h"
#include "led.h"
#include "delay.h"

//系统进入待机模式
void Sys_Enter_Standby(void)
{
	u32 tempreg;	//寄存器设定值临时变量
	
	//关闭所有外设
	RCC->AHB1RSTR|=0x01fe;		//复位除GPIOA以外的所有IO口
	while(WKUP_KD);			//等待WK_UP按键松开(在有RTC中断时,必须等WK_UP松开再进入待机)
	RCC->AHB1RSTR|=1<<0;		//复位GPIOA
	
	//对RTC中断进行处理（先清中断，再重新设置）
	RCC->APB1ENR|=1<<28;	//使能电源时钟
	PWR->CR1|=1<<8;			//Disable backup domain write protection 后备区域访问使能 RTC+SRAM
	//关闭RTC寄存器写保护
	RTC->WPR=0xca;
	RTC->WPR=0x53;
	tempreg=RTC->CR&(0xf<<12);	//记录原来的RTC中断设置（TSIE、WUTIE、ALRBIE、ALRAIE）
	RTC->CR&=~(0xf<<12);		//关闭RTC所有中断
	RTC->ISR&=~(0x3f<<8);		//清除所有RTC中断标志
	PWR->CR2|=1<<0;				//清除Wake-up标志
	RTC->CR|=tempreg;			//重新设置RTC中断
	RTC->WPR=0xff;				//使能RTC寄存器写保护
	Sys_Standby();				//进入待机模式
}

//检测WKUP脚的信号
u8 Check_WKUP(void)
{
	u8 t=0;
	u8 tx=0;
	LED0(0);
	while(1)
	{
		if(WKUP_KD)		//已经按下了
		{
			t++;
			tx=0;
		}else{
			tx++;
			if(tx>3)  //超过90ms内没有WKUP信号
			{
				LED0(1);
				return 0;	//错误的按键，按下次数不够
			}
		}
		delay_ms(30);
		if(t>=100)		//按下超过3s，开机
		{
			LED0(0);		//点亮DS0
			return 1;
		}
	}
}

//中断，检测到PA0脚上的一个上升沿
void EXTI0_IRQHandler(void)
{
	EXTI->PR=1<<0;		//清除LINE0上的中断标志位
	if(Check_WKUP())Sys_Enter_Standby();		//按下超过3s，进入待机模式
}

//PA0 WKUP唤醒初始化
void WKUP_Init(void)
{
	RCC->AHB1ENR|=1<<0;
	GPIO_Set(GPIOA,PIN0,GPIO_MODE_IN,0,0,GPIO_PUPD_PD);		
	
	//检查是否能正常开机
	if(Check_WKUP()==0)Sys_Enter_Standby();		//不是开机，灯不亮，直接进入待机模式
	Ex_NVIC_Config(GPIO_A,0,RTIR);	//PA0上升沿触发
	MY_NVIC_Init(2,2,EXTI0_IRQn,2);
}
