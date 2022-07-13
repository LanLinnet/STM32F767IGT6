#include "wdg.h"

//WWDG����������ֵ[6:0]�����ֵ0x111 1111(7F)
u8 WWDG_CNT=0x7f;	//Ĭ�����ֵ

//��ʼ���������Ź�
//prer����Ƶ��  rlr����װ�ؼĴ���ֵ
void IWDG_Init(u8 prer, u16 rlr)
{
	IWDG->KR = 0x5555;		//ʹ�ܶ�IWDG->PR��IWDG->RLR��д
	IWDG->PR = prer;		//���÷�Ƶϵ��
	IWDG->RLR = rlr;		//������װ�ؼĴ���ֵ
	IWDG->KR=0xaaaa;		//��װ��
	IWDG->KR=0xcccc;		//ʹ�ܿ��Ź�
}

//ι�������Ź�
void IWDG_Feed(void)
{
	IWDG->KR=0xaaaa;		//��װ��
}

//��ʼ�����ڿ��Ź�
//tr-������ֵ  wr-����ֵ  fprer-��Ƶϵ��
void WWDG_Init(u8 tr, u8 wr, u8 fprer)
{
	RCC->APB1ENR|=1<<11;		//ʹ��wwdgʱ��
	WWDG_CNT=tr&WWDG_CNT;		//��ʼ��WWDG_CNT
	WWDG->CFR|=fprer<<7;		//���÷�Ƶϵ��
	WWDG->CFR&=0xFF80;			//���ϴ���ֵ��0
	WWDG->CFR|=wr;				//�����ϴ���ֵ
	
	WWDG->CR|=WWDG_CNT;			//���ü�����ֵ
	WWDG->CR|=1<<7;				//�������Ź�
	MY_NVIC_Init(2, 3, WWDG_IRQn, 2);		//�ж����ȼ����ã���ռ2�������ȼ�3����2
	WWDG->SR=0x00;				//���EWIF ��ǰ�жϱ�־λ
	WWDG->CFR|=1<<9;			//ʹ����ǰ�����ж�
}

//����WWDG��������ֵ��ι����
void WWDG_Set_Counter(u8 cnt)
{
	WWDG->CR=(cnt&0x7f);
}

//���ڿ��Ź��жϷ������
void WWDG_IRQHandler(void)
{
	static u8 led1sta=1;
	if(WWDG->SR&0x01)		//��ǰ�����жϱ�־λ�Ƿ���1
	{
		WWDG->SR=0x00;		//�����ǰ�жϱ�־λ
		WWDG_Set_Counter(WWDG_CNT);		//ι��
		LED1(led1sta^=1);		//LED1��ת
	}
}
