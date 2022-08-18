#include "rs485.h"
#include "pcf8574.h"
#include "delay.h"

#if EN_USART2_RX  	//���ʹ�����жϽ���
u8 RS485_RX_BUF[64];	//���ջ�����
u8 RS485_RX_CNT=0;		//���յ������ݳ���

void USART2_IRQHandler(void)
{
	u8 res;
	if(USART2->ISR&(1<<5))	//���յ�����
	{
		res=USART2->RDR;
		if(RS485_RX_CNT<64)
		{
			RS485_RX_BUF[RS485_RX_CNT]=res;
			RS485_RX_CNT++;
		}
	}
}
#endif

//��ʼ��IO����2
void RS485_Init(u32 pclk1, u32 bound)
{
	u32 temp;
	temp=(pclk1*1000000+bound/2)/bound;
	PCF8574_Init();
	RCC->AHB1ENR|=1<<0;
	GPIO_Set(GPIOA,PIN2|PIN3,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PU);
	GPIO_AF_Set(GPIOA,2,7);
	GPIO_AF_Set(GPIOA,3,7);
	RCC->APB1ENR|=1<<17;
	RCC->APB1RSTR|=1<<17;
	RCC->APB1RSTR&=~(1<<17);
	
	//����������
 	USART2->BRR=temp; 			//����������	
	USART2->CR1=0;		 		//����CR1�Ĵ���
	USART2->CR1|=0<<28;	 		//����M1=0
	USART2->CR1|=0<<12;	 		//����M0=0&M1=0,ѡ��8λ�ֳ� 
	USART2->CR1|=0<<15; 		//����OVER8=0,16�������� 
	USART2->CR1|=1<<3;  		//���ڷ���ʹ�� 
#if EN_USART2_RX		  		//���ʹ���˽���
	//ʹ�ܽ����ж� 
	USART2->CR1|=1<<2;  		//���ڽ���ʹ��
	USART2->CR1|=1<<5;    		//���ջ������ǿ��ж�ʹ��	    	
	MY_NVIC_Init(3,3,USART2_IRQn,2);//��2��������ȼ� 
#endif
	USART2->CR1|=1<<0;  		//����ʹ��
	RS485_TX_Set(0);			//Ĭ������Ϊ����ģʽ	
}

//RS485����len���ֽ�.
//buf:�������׵�ַ
//len:���͵��ֽ���(Ϊ�˺ͱ�����Ľ���ƥ��,���ｨ�鲻Ҫ����64���ֽ�)
void RS485_Send_Data(u8 *buf, u8 len)
{
	u8 t;
	RS485_TX_Set(1);		//����Ϊ����ģʽ
	for(t=0;t<len;t++)
	{
		while((USART2->ISR&0x40)==0);		//�ȴ��������
		USART2->TDR=buf[t];
	}
	while((USART2->ISR&0x40)==0);	//�ȴ��������
	RS485_RX_CNT=0;
	RS485_TX_Set(0);		//����Ϊ����ģʽ
}

//RS485��ѯ���յ�������
//buf:���ջ����׵�ַ
//len:���������ݳ���
void RS485_Receive_Data(u8 *buf, u8 *len)
{
	u8 rxlen=RS485_RX_CNT;
	u8 i=0;
	*len=0;
	delay_ms(10);
	if(rxlen==RS485_RX_CNT&&rxlen)	//���յ������ݣ��ҽ��������
	{
		for(i=0;i<rxlen;i++)
		{
			buf[i]=RS485_RX_BUF[i];
		}
		*len=RS485_RX_CNT;		//��¼�������ݳ���
		RS485_RX_CNT=0;
	}
}

//RS485ģʽ����.
//en:0,����;1,����.
void RS485_TX_Set(u8 en)
{
	PCF8574_WriteBit(RS485_RE_IO,en);
}
