#include "adc.h"
#include "delay.h"

//��ʼ��ADC
void Adc_Init(void)
{
	//��ʼ��IO��
	RCC->APB2ENR|=1<<8;
	RCC->AHB1ENR|=1<<0;
	GPIO_Set(GPIOA,PIN5,GPIO_MODE_AIN,0,0,GPIO_PUPD_PU);
	
	RCC->APB2RSTR|=1<<8;
	RCC->APB2RSTR&=~(1<<8);
	ADC->CCR=1<<16;		//4��Ƶ
	ADC1->CR1=0;
	ADC1->CR2=0;
	ADC1->CR1|=0<<24;		//12λ�ֱ���
	ADC1->CR1|=0<<8;		//��ɨ��ģʽ
	ADC1->CR2&=~(1<<1);		//����ת��ģʽ
	ADC1->CR2&=~(1<<11);	//�Ҷ���
	ADC1->CR2|=0<<28;		//��ʹ���ⲿ���������������
	ADC1->SQR1&=~(0xf<<20);		//1��ת��
	ADC1->SMPR2&=~(7<<(3*5));	//ͨ��5����ʱ�����
	ADC1->SMPR2|=7<<(3*5);		//����ʱ�� 480������
	ADC1->CR2|=1<<0;			//��ADת����
}

//���ADCֵ��������ת�����
u16 Get_Adc(u8 ch)
{
	//����ת������
	ADC1->SQR3&=0xffffffe0;  //��������1 ͨ��ch ������
	ADC1->SQR3|=ch;			//д��ͨ��ֵ
	ADC1->CR2|=1<<30;        //ÿ��ת��ǰ��Ҫ��������ͨ��ת��
	while(!(ADC1->SR&1<<1));		//�ȴ�ת������
	return ADC1->DR;		//����ADCֵ
}

//��ȡͨ��ch��ADCת��ֵ��ȥtimes�Σ�Ȼ�󷵻�ƽ��ֵ
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
