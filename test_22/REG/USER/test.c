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
u8 SendBuff[SEND_BUF_SIZE];	//发送数据缓冲区
const u8 TEXT_TO_SEND[]={"ALIENTEK Apollo STM32F7 DMA 串口实验"};


int main(void)
{ 
 	u8 led0sta=1; 		//LED灯
	u16 i;
	u8 t=0;
	u8 j,mask=0;
	float pro=0;		//进度
	
	Stm32_Clock_Init(432, 25, 2, 9);
	delay_init(216);
	uart_init(108,115200);

	LED_Init();
	MPU_Memory_Protection();
	SDRAM_Init();
	LCD_Init();
	

	KEY_Init();		//按键初始化
	MYDMA_Config(DMA2_Stream7,4,(u32)&USART1->TDR,(u32)SendBuff,SEND_BUF_SIZE);		//初始化DMA
	
	
	POINT_COLOR = RED;		//红色画笔
	LCD_ShowString(30,50,200,16,16,(u8 *)"Apollo STM32F4/F7"); 
	LCD_ShowString(30,70,200,16,16,(u8 *)"DMA TEST");	
	LCD_ShowString(30,90,200,16,16,(u8 *)"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,(u8 *)"2022/8/9");
	LCD_ShowString(30,130,200,16,16,(u8 *)"KEY0: Start");
	
	POINT_COLOR = BLUE;		//蓝色画笔
	j=sizeof(TEXT_TO_SEND);
	for(i=0;i<SEND_BUF_SIZE;i++)
	{
		if(t>=j)	//加入换行符
		{
			if(mask){SendBuff[i]=0x0a;t=0;}
			else{SendBuff[i]=0x0d;mask++;}
		}else //复制TEXT_TO_SEND语句
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
			USART1->CR3=1<<7;		//使能串口1的DMA发送
			MYDMA_Enable(DMA2_Stream7,SEND_BUF_SIZE);		//开始一次DMA传输
			
			//等待DMA传输完成
			while(1)
			{
				if(DMA2->HISR&(1<<27))	//等待DMA2_Stream7传输完成
				{
					DMA2->HIFCR|=1<<27;		//清除传输完成标志
					break;		//跳出循环
				}
				pro=DMA2_Stream7->NDTR;
				pro=1-pro/SEND_BUF_SIZE;
				pro*=100;
				LCD_ShowNum(30,170,pro,3,16);	//显示百分比
			}
			LCD_ShowNum(30,170,100,3,16);//显示100%	  
			LCD_ShowString(30,150,200,16,16,(u8 *)"Transimit Finished!");//提示传送完成
		}
		i++;
		delay_ms(10);
		if(i==20)
		{
			LED0(led0sta^=1);//提示系统正在运行	
			i=0;
		}
	}
}
