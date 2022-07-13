#include "wdg.h"

//WWDG计数器设置值[6:0]，最大值0x111 1111(7F)
u8 WWDG_CNT=0x7f;	//默认最大值

//初始化独立看门狗
//prer：分频数  rlr：重装载寄存器值
void IWDG_Init(u8 prer, u16 rlr)
{
	IWDG->KR = 0x5555;		//使能对IWDG->PR和IWDG->RLR的写
	IWDG->PR = prer;		//设置分频系数
	IWDG->RLR = rlr;		//设置重装载寄存器值
	IWDG->KR=0xaaaa;		//重装载
	IWDG->KR=0xcccc;		//使能看门狗
}

//喂独立看门狗
void IWDG_Feed(void)
{
	IWDG->KR=0xaaaa;		//重装载
}

//初始化窗口看门狗
//tr-计数器值  wr-窗口值  fprer-分频系数
void WWDG_Init(u8 tr, u8 wr, u8 fprer)
{
	RCC->APB1ENR|=1<<11;		//使能wwdg时钟
	WWDG_CNT=tr&WWDG_CNT;		//初始化WWDG_CNT
	WWDG->CFR|=fprer<<7;		//设置分频系数
	WWDG->CFR&=0xFF80;			//将上窗口值清0
	WWDG->CFR|=wr;				//设置上窗口值
	
	WWDG->CR|=WWDG_CNT;			//设置计数器值
	WWDG->CR|=1<<7;				//开启看门狗
	MY_NVIC_Init(2, 3, WWDG_IRQn, 2);		//中断优先级设置：抢占2，子优先级3，组2
	WWDG->SR=0x00;				//清除EWIF 提前中断标志位
	WWDG->CFR|=1<<9;			//使能提前唤醒中断
}

//重设WWDG计数器的值（喂狗）
void WWDG_Set_Counter(u8 cnt)
{
	WWDG->CR=(cnt&0x7f);
}

//窗口看门狗中断服务程序
void WWDG_IRQHandler(void)
{
	static u8 led1sta=1;
	if(WWDG->SR&0x01)		//提前唤醒中断标志位是否置1
	{
		WWDG->SR=0x00;		//清除提前中断标志位
		WWDG_Set_Counter(WWDG_CNT);		//喂狗
		LED1(led1sta^=1);		//LED1翻转
	}
}
