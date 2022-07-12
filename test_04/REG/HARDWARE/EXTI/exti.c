#include "exti.h"
#include "delay.h"
#include "led.h"
#include "key.h"

//�ⲿ�ж�0�������
void EXTI0_IRQHandler(void)
{
	static u8 led0sta=1, led1sta=1;
	delay_ms(10);		//����
	EXTI->PR=1<<0;		//���LINE0�ϵ��жϱ�־λ
	if(WK_UP==1)		//DS0��DS1����
	{
		led1sta=!led1sta;
		led0sta=!led1sta;
		LED1(led1sta);
		LED0(led0sta);
	}
}

//�ⲿ�ж�2�������
void EXTI2_IRQHandler(void)
{
	static u8 led1sta=1;
	delay_ms(10);		//����
	EXTI->PR=1<<2;		//���LINE2�ϵ��жϱ�־λ
	if(KEY1==0)		//DS1
	{
		led1sta=!led1sta;
		LED1(led1sta);
	}
}

//�ⲿ�ж�3�������
void EXTI3_IRQHandler(void)
{
	static u8 led0sta=1, led1sta=1;
	delay_ms(10);		//����
	EXTI->PR=1<<3;		//���LINE3�ϵ��жϱ�־λ
	if(KEY0==0)		//DS0��DS1ͬʱ
	{
		led1sta=!led1sta;
		led0sta=!led0sta;
		LED1(led1sta);
		LED0(led0sta);
	}
}

//�ⲿ�ж�10-15�������
void EXTI15_10_IRQHandler(void)
{
	static u8 led0sta=1;
	delay_ms(10);		//����
	EXTI->PR=1<<13;		//���LINE13�ϵ��жϱ�־λ
	if(KEY2==0)		//DS0
	{
		led0sta=!led0sta;
		LED0(led0sta);
	}
}

//�ⲿ�жϳ�ʼ������
void EXTIX_Init(void)
{
	KEY_Init();
	Ex_NVIC_Config(GPIO_C, 13, FTIR);		//�½��ش���
	Ex_NVIC_Config(GPIO_H, 2, FTIR);		//�½��ش���
	Ex_NVIC_Config(GPIO_H, 3, FTIR);		//�½��ش���
	Ex_NVIC_Config(GPIO_A, 0, RTIR);		//�����ش���
	MY_NVIC_Init(3, 2, EXTI2_IRQn, 2);		//��ռ3�������ȼ�2����2	KEY1
	MY_NVIC_Init(2, 2, EXTI3_IRQn, 2);		//��ռ2�������ȼ�2����2	KEY0
	MY_NVIC_Init(1, 2, EXTI15_10_IRQn, 2);	//��ռ1�������ȼ�2����2	KEY2
	MY_NVIC_Init(0, 2, EXTI0_IRQn, 2);		//��ռ0�������ȼ�2����2	KEY_UP
	
}
