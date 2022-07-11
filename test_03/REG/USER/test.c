#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"

int main(void)
{
	u8 t;
	u8 len;
	u16 times=0;
	u8 led0sta=1;   //LED0��ʼϨ��
	Stm32_Clock_Init(432,25,2,9);
	delay_init(216);
	uart_init(108, 115200);	//���ڳ�ʼ��Ϊ115200
	LED_Init();
	while(1)
	{
		if(USART_RX_STA & 0X8000)  //�������
		{
			len=USART_RX_STA & 0x3fff;		//�õ��˴ν��յ����ݳ��ȣ�ȥ��ǰ��λ��
			printf("\r\n�����͵���ϢΪ��\r\n");
			for(t=0; t<len; t++)
			{
				USART1->TDR=USART_RX_BUF[t];
				while((USART1->ISR&0X40)==0);	//�ȴ����ͽ���
			}
			printf("\r\n\r\n");		//���뻻��
			USART_RX_STA=0;		//����״̬���λ����
		}else{
			times++;
			if(times%5000==0)
			{
				printf("\r\n STM32F7������ ����ʵ�� \r\n\r\n");
			}
			if(times%200==0) printf("���������ݣ��Իس�������\r\n");
			if(times%30==0) LED0(led0sta^=1);	//��˸LED����ʾϵͳ��������
			delay_ms(10);
		}
	}
}
