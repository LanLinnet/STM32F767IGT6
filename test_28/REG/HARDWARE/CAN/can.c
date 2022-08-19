#include "can.h"

//CAN��ʼ��
//����ֵ��0-�ɹ�  ����-ʧ��
u8 CAN1_Mode_Init(u8 tsjw, u8 tbs2, u8 tbs1, u16 brp, u8 mode)
{
	u16 i=0;
	if(tsjw==0||tbs2==0||tbs1==0||brp==0) return 1;
	tsjw-=1;
	tbs1-=1;
	tbs2-=1;
	brp-=1;
	
	RCC->AHB1ENR|=1<<0;
	GPIO_Set(GPIOA, PIN11|PIN12, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_SPEED_50M, GPIO_PUPD_PU);
	GPIO_AF_Set(GPIOA,11,9);
	GPIO_AF_Set(GPIOA,12,9);
	
	RCC->APB1ENR|=1<<25;
	CAN1->MCR=0x0000;
	CAN1->MCR|=1<<0;
	while((CAN1->MSR&1<<0)==0)
	{
		i++;
		if(i>100) return 2;		//�����ʼ��ģʽʧ��
	}
	CAN1->MCR|=0<<7;
	CAN1->MCR|=0<<6;
	CAN1->MCR|=0<<5;
	CAN1->MCR|=1<<4;
	CAN1->MCR|=0<<3;
	CAN1->MCR|=0<<2;
	
	CAN1->BTR=0x00000000;
	CAN1->BTR|=mode<<30;
	CAN1->BTR|=tsjw<<24;
	CAN1->BTR|=tbs2<<20;
	CAN1->BTR|=tbs1<<16;
	CAN1->BTR|=brp<<0;
	
	CAN1->MCR&=~(1<<0);
	while((CAN1->MSR&1<<0)==1)
	{
		i++;
		if(i>0xfff0) return 3;		//�˳���ʼ��ģʽʧ��
	}
	
	//��������ʼ��
	CAN1->FMR|=1<<0;		//�������鹤���ڳ�ʼ��ģʽ
	CAN1->FA1R&=~(1<<0);	//������0������
	CAN1->FS1R|=1<<0; 		//������λ��Ϊ32λ.
	CAN1->FM1R|=0<<0;		//������0�����ڱ�ʶ������λģʽ
	CAN1->FFA1R|=0<<0;		//������0������FIFO0
	CAN1->sFilterRegister[0].FR1=0X00000000;//32λID
	CAN1->sFilterRegister[0].FR2=0X00000000;//32λMASK
	CAN1->FA1R|=1<<0;		//���������0
	CAN1->FMR&=0<<0;		//���������������ģʽ

#if CAN1_RX0_INT_ENABLE
 	//ʹ���жϽ���
	CAN1->IER|=1<<1;		//FIFO0��Ϣ�Һ��ж�����.	    
	MY_NVIC_Init(1,0,CAN1_RX0_IRQn,2);//��2
#endif
	return 0;
}

