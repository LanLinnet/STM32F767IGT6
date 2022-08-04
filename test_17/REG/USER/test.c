#include "sys.h" 
#include "delay.h" 
#include "led.h"
#include "mpu.h"
#include "usart.h"
#include "lcd.h"
#include "sdram.h"
#include "wkup.h"

int main(void)
{ 
 	u8 led0sta=1; 		//LEDµÆ
	
	Stm32_Clock_Init(432, 25, 2, 9);
	delay_init(216);
	uart_init(108,115200);
	LED_Init();
	MPU_Memory_Protection();
	WKUP_Init();		//´ý»ú»½ÐÑ³õÊ¼»¯
	SDRAM_Init();
	LCD_Init();
	
	
	POINT_COLOR = RED;		//ºìÉ«»­±Ê
	LCD_ShowString(30,50,200,16,16,(u8 *)"Apollo STM32F4/F7"); 
	LCD_ShowString(30,70,200,16,16,(u8 *)"WKUP TEST");	
	LCD_ShowString(30,90,200,16,16,(u8 *)"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,(u8 *)"2022/8/4");
	LCD_ShowString(30,130,200,16,16,(u8 *)"WK_UP:Standby/WK_UP");	
	
	
	while(1)
	{
		LED0(led0sta^=1);
		delay_ms(250);
	}
}
