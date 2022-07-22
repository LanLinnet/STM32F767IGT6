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

//��ʱ��5ͨ��1���벶������
//arr-�Զ���װֵ��TIM2 TIM5Ϊ32λ��  psc-ʱ��Ԥ��Ƶ��
void TIM5_CH1_Cap_Init(u32 arr, u16 psc)
{
	RCC->APB1ENR|=1<<3;		//TIM5ʱ��ʹ��
	RCC->AHB1ENR|=1<<0;		//ʹ��PortAʱ�ӣ�GPIOAEN��--KEY_UP
	GPIO_Set(GPIOA, PIN0, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_SPEED_100M, GPIO_PUPD_PD);		//PA0���ã��ܽ�����
	GPIO_AF_Set(GPIOA, 0, 2);		//PA0ӳ��AF2
	
	TIM5->ARR = arr;	//��װֵ
	TIM5->PSC = psc;	//Ԥ��Ƶ
	
	TIM5->CCMR1|=1<<0;	//CC1S=01	ѡ�������IC1ӳ�䵽TI1��
	TIM5->CCMR1|=0<<4;	//IC1F=0000 ���˲�
	TIM5->CCMR1|=0<<2;	//IC1PS=00  ����Ƶ		//0<<2?*****************************
	
	TIM5->CCER|=0<<1;	//CC1P=0	�����ز���
	TIM5->CCER|=1<<0;	//CC1E=1	����ʹ��
	
	TIM5->EGR=1<<0;		//������Ʋ��������¼����Ĵ���ֵ���º�������Ч
	
	TIM5->DIER|=1<<1;	//����1�ж�ʹ��
	TIM5->DIER|=1<<0;	//�����ж�ʹ��
	
	TIM5->CR1|=0x01;	//ʹ�ܶ�ʱ��2
	
	MY_NVIC_Init(2, 0, TIM5_IRQn, 2);		//��ռ2�������ȼ�0����2
}	

//һЩ�Զ���ȫ�ֱ���
//TIM5CH1_CAPTURE_STA  [7]-������ɱ�־  [6]-���񵽸ߵ�ƽ��־  [5:0]-����ߵ�ƽ��ʱ������Ĵ���
u8 TIM5CH1_CAPTURE_STA=0;		//���벶��״̬
u32 TIM5CH1_CAPTURE_VAL;		//���벶��ֵ��TIM2/TIM5��32λ��
//��ʱ��5�жϷ������
void TIM5_IRQHandler(void)
{
	u16 tsr;
	tsr=TIM5->SR;		//״̬�Ĵ���([0]-Update interrupt flag)
	if((TIM5CH1_CAPTURE_STA&0x80)==0)		//��δ�ɹ�����
	{
		if(tsr&0x01)		//���  ��2��**************************************
		{
			if(TIM5CH1_CAPTURE_STA&0x40)	//�Ѿ����񵽸ߵ�ƽ��  0100 0000
			{
				if((TIM5CH1_CAPTURE_STA&0x3F)==0x3F)	//�ߵ�ƽ̫���� 0011 1111  ����������������ʱ
				{
					TIM5CH1_CAPTURE_STA|=0x80;	//��ǳɹ�������һ�� 1000 0000  ǿ�Ʊ�ǲ������
					TIM5CH1_CAPTURE_VAL=0xFFFFFFFF;
				}else TIM5CH1_CAPTURE_STA++;			//�����м��м������
			}
		}
		if(tsr&0x02)		//����1���������¼� ([1]-Capture/compare 1 interrupt flag)
		{
			if(TIM5CH1_CAPTURE_STA&0x40)		//����һ���½��� 0100 0000   ��3��****************************
			{
				TIM5CH1_CAPTURE_STA|=0x80;		//��ǳɹ�����һ�θߵ�ƽ���� 1000 0000
				TIM5CH1_CAPTURE_VAL=TIM5->CCR1;	//��ȡ��ǰ�Ĳ���ֵ
				TIM5->CCER&=~(1<<1);			//CC1P=0 ����Ϊ�����ز���
				
			}else		//��δ�����µ�������  ��1��**********************************************
			{	
				TIM5CH1_CAPTURE_STA = 0;		//���
				TIM5CH1_CAPTURE_VAL=0;
				TIM5CH1_CAPTURE_STA|=0x40;		//��ǲ�����������
				TIM5->CR1&=~(1<<0);				//ʹ�ܶ�ʱ��2
				TIM5->CNT=0;					//���������
				TIM5->CCER|=1<<1;		//CC1P=1 ����Ϊ�½��ز���
				TIM5->CR1|=0x01;		//ʹ�ܶ�ʱ��2
			}
		}
	}
	TIM5->SR=0;		//����жϱ�־λ
}
