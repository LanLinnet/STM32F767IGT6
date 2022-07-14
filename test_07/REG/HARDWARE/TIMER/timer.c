#include "timer.h"
#include "led.h"

//通用定时器3中断初始化
//arr-自动重装载值  psc-时钟预分频数
void TIM3_Int_Init(u16 arr, u16 psc)
{
	RCC->APB1ENR|=1<<1;		//TIM3时钟使能  Bit1-TIM3EN
	TIM3->ARR=arr;			//设置计数器自动重装值
	TIM3->PSC=psc;			//预分频
	TIM3->DIER|=1<<0;		//更新中断使能
	TIM3->CR1|=0x01;		//计数器使能，即使能定时器3
	MY_NVIC_Init(1, 3, TIM3_IRQn, 2);	//抢占1，子优先级3， 组2
}

//定时器3中断服务程序
void TIM3_IRQHandler(void)
{
	static u8 led1sta=1;
	if(TIM3->SR&0x0001)		//溢出中断
	{
		LED1(led1sta^=1);		//LED1翻转
	}
	TIM3->SR&=~(1<<0);		//清除中断标志位
}
