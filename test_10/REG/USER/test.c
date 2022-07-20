#include "sys.h" 
#include "delay.h" 
#include "led.h"
#include "usart.h"
#include "tpad.h"

extern u8 TIM5CH1_CAPTURE_STA;		//���벶��״̬
extern u32 TIM5CH1_CAPTURE_VAL;		//���벶��ֵ
		
int main(void)
{ 
	u8 t=0, led0sta=1, led1sta=1;
	
	Stm32_Clock_Init(432, 25, 2, 9);
	delay_init(216);
	uart_init(108,115200);
	LED_Init();
	TPAD_Init(2);		//��ʼ����������������Ƶ����108/2=54MHzƵ�ʼ���
	
	while(1)
	{
		if(TPAD_Scan(0))		//��׽��һ�������� mode0 ����������
		{
			LED1(led1sta^=1);	//LED1��ת
		}
		t++;
		if(t==15)
		{
			t=0;
			LED0(led0sta^=1);	//LED0��ת����ʾ������������
		}
		delay_ms(10);
	}
}
