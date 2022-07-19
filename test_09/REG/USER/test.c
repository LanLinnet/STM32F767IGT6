#include "sys.h" 
#include "delay.h" 
#include "led.h"
#include "timer.h"
#include "usart.h"

extern u8 TIM5CH1_CAPTURE_STA;		//���벶��״̬
extern u32 TIM5CH1_CAPTURE_VAL;		//���벶��ֵ
		
int main(void)
{ 
	long long temp=0;	//�м���������ڼ���������
	Stm32_Clock_Init(432, 25, 2, 9);
	delay_init(216);
	uart_init(108,115200);
	LED_Init();
	TIM3_PWM_Init(500-1,108-1);		//1MHz����Ƶ�ʣ�2kHz��PWM
	TIM5_CH1_Cap_Init(0xffffffff, 108-1);		//��1MHz��Ƶ�ʼ���

	while(1)
	{
		delay_ms(10);
		LED0_PWM_VAL++;
		if(LED0_PWM_VAL==300)LED0_PWM_VAL=0;
		if(TIM5CH1_CAPTURE_STA&0x80)		//�ɹ�������һ�θߵ�ƽ
		{
			temp=TIM5CH1_CAPTURE_STA&0x3f;		//��temp�����
			temp*=0xffffffff;		//���ʱ���ܺ�
			temp+=TIM5CH1_CAPTURE_VAL;		//�õ��ܵĸߵ�ƽʱ��  temp*ARR+CRRx2
			printf("HIGH: %lld us\r\n",temp);		//��ӡ������ĸߵ�ƽʱ��
			TIM5CH1_CAPTURE_STA=0;		//������һ�β���
		}
	}	
}
