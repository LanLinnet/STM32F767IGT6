#include "sys.h" 
#include "delay.h" 
#include "led.h"
#include "mpu.h"
#include "usart.h"
#include "lcd.h"
#include "sdram.h"
#include "usmart.h"

#include "rng.h"
#include "key.h"
int main(void)
{ 
 	u8 led0sta=1; 		//LED灯
	u32 random;			//随机数
	u8 t=0, key;		//时间，按键
	
	Stm32_Clock_Init(432, 25, 2, 9);
	delay_init(216);
	uart_init(108,115200);
	usmart_dev.init(108);  //初始化USMART
	LED_Init();
	MPU_Memory_Protection();
	KEY_Init();
	SDRAM_Init();
	LCD_Init();
	
	
	POINT_COLOR = RED;		//红色画笔
	LCD_ShowString(30,50,200,16,16,(u8 *)"Apollo STM32F4/F7"); 
	LCD_ShowString(30,70,200,16,16,(u8 *)"RNG TEST");	
	LCD_ShowString(30,90,200,16,16,(u8 *)"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,(u8 *)"2022/8/3");	
	
  	while(RNG_Init()) 
	{		    					 
		LCD_ShowString(30,130,200,16,16,(u8 *)"  RNG ERROR!");
		delay_ms(200);
		LCD_ShowString(30,130,200,16,16,(u8 *)"RNG Trying...");
	}
	LCD_ShowString(30,130,200,16,16,(u8 *)"RNG Ready!	");
	LCD_ShowString(30,150,200,16,16,(u8 *)"KEY0: Get Random Num");
	LCD_ShowString(30,180,200,16,16,(u8 *)"Random Num: ");
	LCD_ShowString(30,210,200,16,16,(u8 *)"Random Num[0-9]: ");
	POINT_COLOR=BLUE;
	
	while(1)
	{
		key=KEY_Scan(0);
		if(key==KEY0_PRES)
		{
			random=RNG_Get_RandomNum();
			LCD_ShowNum(30+8*12,180,random,10,16);
		}
		if((t%20)==0)		//每200ms
		{
			LED0(led0sta^=1);
			random=RNG_Get_RandomRange(0,9);		//获取[0,9]区间的随机数
			LCD_ShowNum(30+8*17,210,random,1,16);
		}
		delay_ms(10);
		t++;
	}
}
