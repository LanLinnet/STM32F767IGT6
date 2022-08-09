#include "sys.h" 
#include "delay.h" 
#include "led.h"
#include "mpu.h"
#include "usart.h"
#include "lcd.h"
#include "sdram.h"
#include "key.h"
#include "dma.h"

#define SEND_BUF_SIZE 7800
u8 SendBuff[SEND_BUF_SIZE];	//�������ݻ�����
const u8 TEXT_TO_SEND[]={"ALIENTEK Apollo STM32F7 DMA ����ʵ��"};


int main(void)
{ 
 	u8 led0sta=1; 		//LED��
	u16 i;
	u8 t=0;
	u8 j,mask=0;
	float pro=0;		//����
	
	Stm32_Clock_Init(432, 25, 2, 9);
	delay_init(216);
	uart_init(108,115200);

	LED_Init();
	MPU_Memory_Protection();
	SDRAM_Init();
	LCD_Init();
	

	KEY_Init();		//������ʼ��
	MYDMA_Config(DMA2_Stream7,4,(u32)&USART1->TDR,(u32)SendBuff,SEND_BUF_SIZE);		//��ʼ��DMA
	
	
	POINT_COLOR = RED;		//��ɫ����
	LCD_ShowString(30,50,200,16,16,(u8 *)"Apollo STM32F4/F7"); 
	LCD_ShowString(30,70,200,16,16,(u8 *)"DMA TEST");	
	LCD_ShowString(30,90,200,16,16,(u8 *)"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,(u8 *)"2022/8/9");
	LCD_ShowString(30,130,200,16,16,(u8 *)"KEY0: Start");
	
	POINT_COLOR = BLUE;		//��ɫ����
	j=sizeof(TEXT_TO_SEND);
	for(i=0;i<SEND_BUF_SIZE;i++)
	{
		if(t>=j)	//���뻻�з�
		{
			if(mask){SendBuff[i]=0x0a;t=0;}
			else{SendBuff[i]=0x0d;mask++;}
		}else //����TEXT_TO_SEND���
		{
			mask=0;
			SendBuff[i]=TEXT_TO_SEND[t];
			t++;
		}
	}
	POINT_COLOR=BLUE;
	i=0;
	while(1)
	{
		t=KEY_Scan(0);
		if(t==KEY0_PRES)
		{
			printf("\r\nDMA DATA:\r\n");
			LCD_ShowString(30,150,200,16,16,(u8 *)"Start Transimit....");
			LCD_ShowString(30,170,200,16,16,(u8 *)"  %");
			USART1->CR3=1<<7;		//ʹ�ܴ���1��DMA����
			MYDMA_Enable(DMA2_Stream7,SEND_BUF_SIZE);		//��ʼһ��DMA����
			
			//�ȴ�DMA�������
			while(1)
			{
				if(DMA2->HISR&(1<<27))	//�ȴ�DMA2_Stream7�������
				{
					DMA2->HIFCR|=1<<27;		//���������ɱ�־
					break;		//����ѭ��
				}
				pro=DMA2_Stream7->NDTR;
				pro=1-pro/SEND_BUF_SIZE;
				pro*=100;
				LCD_ShowNum(30,170,pro,3,16);	//��ʾ�ٷֱ�
			}
			LCD_ShowNum(30,170,100,3,16);//��ʾ100%	  
			LCD_ShowString(30,150,200,16,16,(u8 *)"Transimit Finished!");//��ʾ�������
		}
		i++;
		delay_ms(10);
		if(i==20)
		{
			LED0(led0sta^=1);//��ʾϵͳ��������	
			i=0;
		}
	}
}
