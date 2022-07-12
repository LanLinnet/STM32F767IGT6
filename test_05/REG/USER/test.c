#include "sys.h" 
#include "delay.h" 
#include "led.h"
#include "key.h"
#include "wdg.h"

int main(void)
{ 
	Stm32_Clock_Init(432, 25, 2, 9);
	delay_init(216);
	LED_Init();
	KEY_Init();
	delay_ms(100);		//��ʱ100ms�ٳ�ʼ�����Ź���LED0�ı仯���ɼ���
	IWDG_Init(4, 500);	//Ԥ��Ƶϵ��Ϊ4*2^4 = 64������ֵΪ500�� ���ʱ��Ϊ64*500/32=1000ms����1s
	LED0(0);			//�ȵ������
	while(1)
	{
		if(KEY_Scan(0)==WKUP_PRES)	//���WK_UP���£���ι����  0-��֧��������
		{
			IWDG_Feed();	//ι��
		}
		delay_ms(10);
	}
}
