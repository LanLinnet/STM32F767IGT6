#include "wkup.h"
#include "led.h"
#include "delay.h"

//ϵͳ�������ģʽ
void Sys_Enter_Standby(void)
{
	u32 tempreg;	//�Ĵ����趨ֵ��ʱ����
	
	//�ر���������
	RCC->AHB1RSTR|=0x01fe;		//��λ��GPIOA���������IO��
	while(WKUP_KD);			//�ȴ�WK_UP�����ɿ�(����RTC�ж�ʱ,�����WK_UP�ɿ��ٽ������)
	RCC->AHB1RSTR|=1<<0;		//��λGPIOA
	
	//��RTC�жϽ��д��������жϣ����������ã�
	RCC->APB1ENR|=1<<28;	//ʹ�ܵ�Դʱ��
	PWR->CR1|=1<<8;			//Disable backup domain write protection ���������ʹ�� RTC+SRAM
	//�ر�RTC�Ĵ���д����
	RTC->WPR=0xca;
	RTC->WPR=0x53;
	tempreg=RTC->CR&(0xf<<12);	//��¼ԭ����RTC�ж����ã�TSIE��WUTIE��ALRBIE��ALRAIE��
	RTC->CR&=~(0xf<<12);		//�ر�RTC�����ж�
	RTC->ISR&=~(0x3f<<8);		//�������RTC�жϱ�־
	PWR->CR2|=1<<0;				//���Wake-up��־
	RTC->CR|=tempreg;			//��������RTC�ж�
	RTC->WPR=0xff;				//ʹ��RTC�Ĵ���д����
	Sys_Standby();				//�������ģʽ
}

//���WKUP�ŵ��ź�
u8 Check_WKUP(void)
{
	u8 t=0;
	u8 tx=0;
	LED0(0);
	while(1)
	{
		if(WKUP_KD)		//�Ѿ�������
		{
			t++;
			tx=0;
		}else{
			tx++;
			if(tx>3)  //����90ms��û��WKUP�ź�
			{
				LED0(1);
				return 0;	//����İ��������´�������
			}
		}
		delay_ms(30);
		if(t>=100)		//���³���3s������
		{
			LED0(0);		//����DS0
			return 1;
		}
	}
}

//�жϣ���⵽PA0���ϵ�һ��������
void EXTI0_IRQHandler(void)
{
	EXTI->PR=1<<0;		//���LINE0�ϵ��жϱ�־λ
	if(Check_WKUP())Sys_Enter_Standby();		//���³���3s���������ģʽ
}

//PA0 WKUP���ѳ�ʼ��
void WKUP_Init(void)
{
	RCC->AHB1ENR|=1<<0;
	GPIO_Set(GPIOA,PIN0,GPIO_MODE_IN,0,0,GPIO_PUPD_PD);		
	
	//����Ƿ�����������
	if(Check_WKUP()==0)Sys_Enter_Standby();		//���ǿ������Ʋ�����ֱ�ӽ������ģʽ
	Ex_NVIC_Config(GPIO_A,0,RTIR);	//PA0�����ش���
	MY_NVIC_Init(2,2,EXTI0_IRQn,2);
}
