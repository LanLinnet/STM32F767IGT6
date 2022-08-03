#include "rng.h"
#include "delay.h"
//��ʼ��RNG
//����ֵ��0-�ɹ�  1-ʧ��
u8 RNG_Init(void)
{
	u16 retry=0;
	RCC->AHB2ENR=1<<6;		//Bit 6 RNGEN: Random number generator clock enable
	RNG->CR|=1<<2;			//ʹ��RNG
	while((RNG->SR&0x01)==0 && retry<10000) //�ȴ��������Ч
	{
		retry++;
		delay_us(100);
	}
	if(retry>=10000) return 1;
	return 0;
}

//�õ������������
u32 RNG_Get_RandomNum(void)
{
	while((RNG->SR&0x01)==0);	//�ȴ��������Ч
	return RNG->DR;
}

//�õ�ĳ����Χ�ڵ������
int RNG_Get_RandomRange(int min, int max)
{
	return RNG_Get_RandomNum()%(max-min+1)+min;
}
