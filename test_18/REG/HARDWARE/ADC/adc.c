#include "adc.h"
#include "delay.h"

//初始化ADC
void Adc_Init(void)
{
	//初始化IO口
	RCC->APB2ENR|=1<<8;
	RCC->AHB1ENR|=1<<0;
	GPIO_Set(GPIOA,PIN5,GPIO_MODE_AIN,0,0,GPIO_PUPD_PU);
	
	RCC->APB2RSTR|=1<<8;
	RCC->APB2RSTR&=~(1<<8);
	ADC->CCR=1<<16;		//4分频
	ADC1->CR1=0;
	ADC1->CR2=0;
	ADC1->CR1|=0<<24;		//12位分辨率
	ADC1->CR1|=0<<8;		//非扫描模式
	ADC1->CR2&=~(1<<1);		//单次转换模式
	ADC1->CR2&=~(1<<11);	//右对齐
	ADC1->CR2|=0<<28;		//不使用外部触发（软件触发）
	ADC1->SQR1&=~(0xf<<20);		//1次转换
	ADC1->SMPR2&=~(7<<(3*5));	//通道5采样时间清空
	ADC1->SMPR2|=7<<(3*5);		//采样时间 480个周期
	ADC1->CR2|=1<<0;			//开AD转换器
}

//获得ADC值，并返回转换结果
u16 Get_Adc(u8 ch)
{
	//设置转换序列
	ADC1->SQR3&=0xffffffe0;  //规则序列1 通道ch 先清零
	ADC1->SQR3|=ch;			//写入通道值
	ADC1->CR2|=1<<30;        //每次转换前都要开启规则通道转换
	while(!(ADC1->SR&1<<1));		//等待转换结束
	return ADC1->DR;		//返回ADC值
}

//获取通道ch的ADC转换值，去times次，然后返回平均值
u16 Get_Adc_Average(u8 ch, u8 times)
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<times;t++)
	{
		temp_val+=Get_Adc(ch);
		delay_ms(5);
	}
	return temp_val/times;		
}
