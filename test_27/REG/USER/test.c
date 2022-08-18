#include "sys.h" 
#include "delay.h" 
#include "led.h"
#include "mpu.h"
#include "usart.h"
#include "lcd.h"
#include "sdram.h"
#include "key.h"
#include "usmart.h"
#include "rs485.h"

const u8 TEXT_Buffer[]={"Apollo STM32F7 QSPI TEST"};
#define SIZE sizeof(TEXT_Buffer)
	
int main(void)
{ 
 	u8 led0sta=1; 		//LED灯
	u8 key;
	u16 i=0,t=0,cnt=0;
	u8 rs485buf[5];
	
	Stm32_Clock_Init(432, 25, 2, 9);
	delay_init(216);
	uart_init(108,115200);
	usmart_dev.init(108);
	LED_Init();
	MPU_Memory_Protection();
	SDRAM_Init();
	LCD_Init();
	KEY_Init();
	
	RS485_Init(54,9600);	//初始化485
		
	POINT_COLOR = RED;		//红色画笔
	LCD_ShowString(30,50,200,16,16,(u8 *)"Apollo STM32F4/F7"); 
	LCD_ShowString(30,70,200,16,16,(u8 *)"RS485 TEST");	
	LCD_ShowString(30,90,200,16,16,(u8 *)"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,(u8 *)"2022/8/18");
	LCD_ShowString(30,130,200,16,16,(u8 *)"KEY0:Send");	//显示提示信息
	
	POINT_COLOR = BLUE;		//蓝色画笔
	LCD_ShowString(30,150,200,16,16,(u8 *)"Count:");
	LCD_ShowString(30,170,200,16,16,(u8 *)"Send Data:");
	LCD_ShowString(30,210,200,16,16,(u8 *)"Receive Data:");
	
	
	while(1)
	{
		key=KEY_Scan(0);
		if(key==KEY0_PRES)//KEY0按下,读取字符串并显示
		{
			for(i=0;i<5;i++)
			{
				rs485buf[i]=cnt+i;	//填充发送缓冲区
				LCD_ShowxNum(30+i*32,190,rs485buf[i],3,16,0x80);
			}
 			RS485_Send_Data(rs485buf,5);
		} 
		RS485_Receive_Data(rs485buf, &key);
		if(key)
		{
			if(key>5) key=5;		//最大5个数据
			for(i=0;i<key;i++)
			{
				LCD_ShowxNum(30+i*32,230,rs485buf[i],3,16,0x80);
			}
		}
		t++;
		delay_ms(10);
		if(t==20)
		{
			LED0(led0sta^=1);	//DS0闪烁
			t=0;
			cnt++;
			LCD_ShowxNum(30+48,150,cnt,3,16,0x80);
		}
	}
}
