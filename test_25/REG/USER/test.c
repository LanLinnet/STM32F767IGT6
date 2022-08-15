#include "sys.h" 
#include "delay.h" 
#include "led.h"
#include "mpu.h"
#include "usart.h"
#include "lcd.h"
#include "sdram.h"
#include "key.h"
#include "usmart.h"
#include "ap3216c.h"


	
int main(void)
{ 
 	u8 led0sta=1; 		//LEDµÆ
	u16 ir,als,ps;
	
	Stm32_Clock_Init(432, 25, 2, 9);
	delay_init(216);
	uart_init(108,115200);
	usmart_dev.init(108);
	LED_Init();
	MPU_Memory_Protection();
	SDRAM_Init();
	LCD_Init();
		
	POINT_COLOR = RED;		//ºìÉ«»­±Ê
	LCD_ShowString(30,50,200,16,16,(u8 *)"Apollo STM32F4/F7"); 
	LCD_ShowString(30,70,200,16,16,(u8 *)"AP3216C TEST");	
	LCD_ShowString(30,90,200,16,16,(u8 *)"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,(u8 *)"2022/8/15");
	
	while(AP3216C_Init())	//Î´¼ì²âµ½AP3216C
	{
		LCD_ShowString(30,130,200,16,16,(u8 *)"AP3216C Check Failed!");
		delay_ms(500);
		LCD_ShowString(30,130,200,16,16,(u8 *)"Please Check!        ");
		delay_ms(500);
		LED0(led0sta^=1);
	}
	
	LCD_ShowString(30,130,200,16,16,(u8 *)"AP3216C Ready!");
	LCD_ShowString(30,160,200,16,16,(u8 *)"IR:");
	LCD_ShowString(30,180,200,16,16,(u8 *)"PS:");
	LCD_ShowString(30,200,200,16,16,(u8 *)"ALS:");
	POINT_COLOR = BLUE;		//À¶É«»­±Ê
	
	while(1)
	{
		AP3216C_ReadData(&ir, &ps, &als);
		LCD_ShowNum(30+32,160,ir,5,16);		//IR
		LCD_ShowNum(30+32,180,ps,5,16);		//PS
		LCD_ShowNum(30+32,200,als,5,16);	//ALS
		LED0(led0sta^=1);
		delay_ms(120);
	}
}
