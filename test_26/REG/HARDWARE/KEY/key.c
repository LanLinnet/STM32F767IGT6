#include "key.h"
#include "delay.h"

void KEY_Init(void)
{
	//ʹ��GPIOA GPIOC GPIOHʱ��
	RCC->AHB1ENR|=1<<0;
	RCC->AHB1ENR|=1<<2;
	RCC->AHB1ENR|=1<<7;
	//����GPIOģʽ��������
	GPIO_Set(GPIOA, PIN0, GPIO_MODE_IN, 0, 0, GPIO_PUPD_PD);
	GPIO_Set(GPIOC, PIN13, GPIO_MODE_IN, 0, 0, GPIO_PUPD_PU);
	GPIO_Set(GPIOH, PIN2|PIN3, GPIO_MODE_IN, 0, 0, GPIO_PUPD_PU);
}

//����������
//mode��0-��֧��������  1-֧��������
//����ֵ��1-KEY0  2-KEY1  3-KEY2  4-KEY_UP 0-�ް�������
u8 KEY_Scan(u8 mode)
{
	static u8 key_up=1;	//�����ɿ���־λ
	if(mode)key_up=1;
	if(key_up&&(KEY0==0||KEY1==0||KEY2==0||WK_UP==1))
	{
		delay_ms(10);	//��������
		key_up=0;
		if(KEY0==0) return 1;
		else if(KEY1==0) return 2;
		else if(KEY2==0) return 3;
		else if(WK_UP==1) return 4;
	}else if(KEY0==1&&KEY1==1&&KEY2==1&&WK_UP==0) key_up=1;
	return 0;
}
