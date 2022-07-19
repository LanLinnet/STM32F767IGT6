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

//TIM3 PWM初始化
void TIM3_PWM_Init(u32 arr, u32 psc)
{
	RCC->APB1ENR|=1<<1;		//TIM3时钟使能
	RCC->AHB1ENR|=1<<2;		//使能PORTB时钟
	
	GPIO_Set(GPIOB, PIN1, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_SPEED_100M, GPIO_PUPD_PU);		//复用功能，上拉输出
	GPIO_AF_Set(GPIOB, 1, 2);		//PB1复用AF2（TIM3）
	
	TIM3->ARR=arr;			//设置计数器自动重装值
	TIM3->PSC=psc;			//预分频
	TIM3->CCMR2|=6<<12;		//CH4 PWM1模式  [14:12]--0110
	TIM3->CCMR2|=1<<11;		//CH4 预装载使能
	TIM3->CCER|=1<<12;		//OC4 输出使能
	TIM3->CCER|=1<<13;		//OC4 低电平有效
	TIM3->CR1|=1<<7;		//ARPE使能(Auto-reload preload enable)
	TIM3->CR1|=1<<0;		//使能定时器3
}

//定时器5通道1输入捕获配置
//arr-自动重装值（TIM2 TIM5为32位）  psc-时钟预分频数
void TIM5_CH1_Cap_Init(u32 arr, u16 psc)
{
	RCC->APB1ENR|=1<<3;		//TIM5时钟使能
	RCC->AHB1ENR|=1<<0;		//使能PortA时钟（GPIOAEN）--KEY_UP
	GPIO_Set(GPIOA, PIN0, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_SPEED_100M, GPIO_PUPD_PD);		//PA0复用，管脚下拉
	GPIO_AF_Set(GPIOA, 0, 2);		//PA0映射AF2
	
	TIM5->ARR = arr;	//重装值
	TIM5->PSC = psc;	//预分频
	
	TIM5->CCMR1|=1<<0;	//CC1S=01	选择输入端IC1映射到TI1上
	TIM5->CCMR1|=0<<4;	//IC1F=0000 不滤波
	TIM5->CCMR1|=0<<2;	//IC1PS=00  不分频		//0<<2?*****************************
	
	TIM5->CCER|=0<<1;	//CC1P=0	上升沿捕获
	TIM5->CCER|=1<<0;	//CC1E=1	输入使能
	
	TIM5->EGR=1<<0;		//软件控制产生更新事件，寄存器值更新后立即生效
	
	TIM5->DIER|=1<<1;	//捕获1中断使能
	TIM5->DIER|=1<<0;	//更新中断使能
	
	TIM5->CR1|=0x01;	//使能定时器2
	
	MY_NVIC_Init(2, 0, TIM5_IRQn, 2);		//抢占2，子优先级0，组2
}	

//一些自定义全局变量
//TIM5CH1_CAPTURE_STA  [7]-捕获完成标志  [6]-捕获到高电平标志  [5:0]-捕获高电平后定时器溢出的次数
u8 TIM5CH1_CAPTURE_STA=0;		//输入捕获状态
u32 TIM5CH1_CAPTURE_VAL;		//输入捕获值（TIM2/TIM5是32位）
//定时器5中断服务程序
void TIM5_IRQHandler(void)
{
	u16 tsr;
	tsr=TIM5->SR;		//状态寄存器([0]-Update interrupt flag)
	if((TIM5CH1_CAPTURE_STA&0x80)==0)		//还未成功捕获
	{
		if(tsr&0x01)		//溢出  （2）**************************************
		{
			if(TIM5CH1_CAPTURE_STA&0x40)	//已经捕获到高电平了  0100 0000
			{
				if((TIM5CH1_CAPTURE_STA&0x3F)==0x3F)	//高电平太长了 0011 1111  即超过最大溢出次数时
				{
					TIM5CH1_CAPTURE_STA|=0x80;	//标记成功捕获了一次 1000 0000  强制标记捕获完成
					TIM5CH1_CAPTURE_VAL=0xFFFFFFFF;
				}else TIM5CH1_CAPTURE_STA++;			//计算中间有几次溢出
			}
		}
		if(tsr&0x02)		//捕获1发生捕获事件 ([1]-Capture/compare 1 interrupt flag)
		{
			if(TIM5CH1_CAPTURE_STA&0x40)		//捕获到一个下降沿 0100 0000   （3）****************************
			{
				TIM5CH1_CAPTURE_STA|=0x80;		//标记成功捕获到一次高电平脉宽 1000 0000
				TIM5CH1_CAPTURE_VAL=TIM5->CCR1;	//获取当前的捕获值
				TIM5->CCER&=~(1<<1);			//CC1P=0 设置为上升沿捕获
				
			}else		//还未捕获到新的上升沿  （1）**********************************************
			{	
				TIM5CH1_CAPTURE_STA = 0;		//清空
				TIM5CH1_CAPTURE_VAL=0;
				TIM5CH1_CAPTURE_STA|=0x40;		//标记捕获到了上升沿
				TIM5->CR1&=~(1<<0);				//使能定时器2
				TIM5->CNT=0;					//计数器清空
				TIM5->CCER|=1<<1;		//CC1P=1 设置为下降沿捕获
				TIM5->CR1|=0x01;		//使能定时器2
			}
		}
	}
	TIM5->SR=0;		//清楚中断标志位
}
