#include "dac.h"

void Dac1_Init(void)
{
	RCC->APB1ENR|=1<<29;
	RCC->AHB1ENR|=1<<0;
	GPIO_Set(GPIOA,PIN4,GPIO_MODE_AIN,0,0,GPIO_PUPD_PU);
	DAC->CR|=1<<0;
	DAC->CR|=1<<1;
	DAC->CR|=0<<2;
	DAC->CR|=0<<3;
	DAC->CR|=0<<6;
	DAC->CR|=0<<8;
	DAC->CR|=0<<12;
	DAC->DHR12R1=0;
}

//设置通道1输出电压（计算DORx的值）
//vol-扩大了1000倍的电压
void Dac1_Set_Vol(u16 vol)
{
	double temp=vol;
	temp/=1000;
	temp=temp*4096/3.3;
	DAC->DHR12R1=temp;
}
