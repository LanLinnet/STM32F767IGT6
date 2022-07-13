#include "sys.h" 
#include "delay.h" 
#include "led.h"
#include "wdg.h"

int main(void)
{ 
	Stm32_Clock_Init(432, 25, 2, 9);
	delay_init(216);
	LED_Init();
	LED0(0);			//�ȵ������
	delay_ms(300);		//��ʱ300ms�ٳ�ʼ�����Ź���LED0�ı仯���ɼ���
	WWDG_Init(0x7f, 0x5f, 3);		//������ֵΪ7f���ϴ���ֵΪ5f�� ��Ƶϵ��Ϊ8��2^3��
	
	while(1)
	{
		LED0(1);		//�رպ��
	}
}
