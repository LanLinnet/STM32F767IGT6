#include "sys.h" 
#include "delay.h" 
#include "led.h"
#include "mpu.h"
#include "usart.h"
#include "lcd.h"
#include "sdram.h"
#include "key.h"
#include "usmart.h"

#include "can.h"
	
int main(void)
{ 
 	u8 led0sta=1; 		//LED灯
	u8 key;
	u16 i=0,t=0,cnt=0;
	
	u8 canbuf[8];
	u8 res;
	u8 mode=1;  	//CAN工作模式：0-普通；1-环回
	
	Stm32_Clock_Init(432, 25, 2, 9);
	delay_init(216);
	uart_init(108,115200);
	usmart_dev.init(108);
	LED_Init();
	MPU_Memory_Protection();
	SDRAM_Init();
	LCD_Init();
	KEY_Init();
	
	CAN1_Mode_Init(1,7,10,6,1);		//CAN初始化，波特率500kbps
		
	POINT_COLOR = RED;		//红色画笔
	LCD_ShowString(30,50,200,16,16,(u8 *)"Apollo STM32F4/F7"); 
	LCD_ShowString(30,70,200,16,16,(u8 *)"CAN TEST");	
	LCD_ShowString(30,90,200,16,16,(u8 *)"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,(u8 *)"2022/10/11");
	LCD_ShowString(30,130,200,16,16,(u8 *)"LoopBack Mode");
	LCD_ShowString(30,150,200,16,16,(u8 *)"KEY0:Send WK_UP:Mode");	//显示提示信息
	
	POINT_COLOR = BLUE;		//蓝色画笔
	LCD_ShowString(30,170,200,16,16,(u8 *)"Count:");
	LCD_ShowString(30,190,200,16,16,(u8 *)"Send Data:");
	LCD_ShowString(30,250,200,16,16,(u8 *)"Receive Data:");
	
	
	while(1)
	{
		key=KEY_Scan(0);
		if(key==KEY0_PRES)//KEY0按下，发送一次数据
		{
			for(i=0;i<8;i++)
			{
				canbuf[i]=cnt+i;	//填充发送缓冲区
				if(i<4) LCD_ShowxNum(30+i*32,210,canbuf[i],3,16,0X80);	//显示数据
				else LCD_ShowxNum(30+(i-4)*32,230,canbuf[i],3,16,0X80);	//显示数据
				
			}
 			res=CAN1_Send_Msg(canbuf,8);
			if(res)LCD_ShowString(30+80,190,200,16,16,(u8 *)"Failed");		//提示发送失败
			else LCD_ShowString(30+80,190,200,16,16,(u8 *)"OK    ");	 		//提示发送成功
		}else if(key==WKUP_PRES) 
		{
			mode=!mode;		//改变CAN的工作模式
			CAN1_Mode_Init(1,7,10,6,mode);
			  			POINT_COLOR=RED;//设置字体为红色 
			if(mode==0)//普通模式，需要2个开发板
			{
				LCD_ShowString(30,130,200,16,16,(u8 *)"Normal   Mode ");	    
			}else //回环模式,一个开发板就可以测试了.
			{
 				LCD_ShowString(30,130,200,16,16,(u8 *)"LoopBack Mode");
			}
 			POINT_COLOR=BLUE;//设置字体为蓝色 
		}
		key=CAN1_Receive_Msg(canbuf);
		if(key)
		{
			LCD_Fill(30,270,160,310,WHITE);//清除之前的显示
 			for(i=0;i<key;i++)
			{									    
				if(i<4)LCD_ShowxNum(30+i*32,270,canbuf[i],3,16,0X80);	//显示数据
				else LCD_ShowxNum(30+(i-4)*32,290,canbuf[i],3,16,0X80);	//显示数据
 			}
		}
		
		t++;
		delay_ms(10);
		if(t==20)
		{
			LED0(led0sta^=1);	//DS0闪烁
			t=0;
			cnt++;
			LCD_ShowxNum(30+48,170,cnt,3,16,0x80);
		}
	}
}
