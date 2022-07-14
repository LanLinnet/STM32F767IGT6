#include "timer.h"
#include "led.h"

//ͨ�ö�ʱ��3�жϳ�ʼ��
//arr-�Զ���װ��ֵ  psc-ʱ��Ԥ��Ƶ��
void TIM3_Int_Init(u16 arr, u16 psc)
{
	RCC->APB1ENR|=1<<1;		//TIM3ʱ��ʹ��  Bit1-TIM3EN
	TIM3->ARR=arr;			//���ü������Զ���װֵ
	TIM3->PSC=psc;			//Ԥ��Ƶ
	TIM3->DIER|=1<<0;		//�����ж�ʹ��
	TIM3->CR1|=0x01;		//������ʹ�ܣ���ʹ�ܶ�ʱ��3
	MY_NVIC_Init(1, 3, TIM3_IRQn, 2);	//��ռ1�������ȼ�3�� ��2
}

//��ʱ��3�жϷ������
void TIM3_IRQHandler(void)
{
	static u8 led1sta=1;
	if(TIM3->SR&0x0001)		//����ж�
	{
		LED1(led1sta^=1);		//LED1��ת
	}
	TIM3->SR&=~(1<<0);		//����жϱ�־λ
}

//TIM3 PWM��ʼ��
void TIM3_PWM_Init(u32 arr, u32 psc)
{
	RCC->APB1ENR|=1<<1;		//TIM3ʱ��ʹ��
	RCC->AHB1ENR|=1<<2;		//ʹ��PORTBʱ��
	
	GPIO_Set(GPIOB, PIN1, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_SPEED_100M, GPIO_PUPD_PU);		//���ù��ܣ��������
	GPIO_AF_Set(GPIOB, 1, 2);		//PB1����AF2��TIM3��
	
	TIM3->ARR=arr;			//���ü������Զ���װֵ
	TIM3->PSC=psc;			//Ԥ��Ƶ
	TIM3->CCMR2|=6<<12;		//CH4 PWM1ģʽ  [14:12]--0110
	TIM3->CCMR2|=1<<11;		//CH4 Ԥװ��ʹ��
	TIM3->CCER|=1<<12;		//OC4 ���ʹ��
	TIM3->CCER|=1<<13;		//OC4 �͵�ƽ��Ч
	TIM3->CR1|=1<<7;		//ARPEʹ��(Auto-reload preload enable)
	TIM3->CR1|=1<<0;		//ʹ�ܶ�ʱ��3
}
