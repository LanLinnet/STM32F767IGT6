#include "wdg.h"

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
