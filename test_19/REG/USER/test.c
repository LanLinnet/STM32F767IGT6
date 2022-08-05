#include "sys.h" 
#include "delay.h" 
#include "led.h"
#include "mpu.h"
#include "usart.h"
#include "lcd.h"
#include "sdram.h"
#include "adc.h"

int main(void)
{ 
 	u8 led0sta=1; 		//LEDµÆ
	short temp;
	
	Stm32_Clock_Init(432, 25, 2, 9);
	delay_init(216);
	uart_init(108,115200);
	LED_Init();
	MPU_Memory_Protection();
	SDRAM_Init();
	LCD_Init();
	
	Adc_Init();		//³õÊ¼»¯ADC
	
	
	POINT_COLOR = RED;		//ºìÉ«»­±Ê
	LCD_ShowString(30,50,200,16,16,(u8 *)"Apollo STM32F4/F7"); 
	LCD_ShowString(30,70,200,16,16,(u8 *)"Temperature TEST");	
	LCD_ShowString(30,90,200,16,16,(u8 *)"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,(u8 *)"2022/8/5");
	
	POINT_COLOR = BLUE;		//À¶É«»­±Ê
	LCD_ShowString(30,140,200,16,16,(u8 *)"Temperature: 00.00C");
	
	while(1)
	{
		temp=Get_Temperature();
		if(temp<0)
		{
			temp=-temp;
			LCD_ShowString(30+12*8,140,16,16,16,(u8 *)"-");
		}else LCD_ShowString(30+12*8,140,16,16,16,(u8 *)" ");
		
		LCD_ShowxNum(30+13*8,140,temp/100,2,16,0);
		LCD_ShowxNum(30+16*8,140,temp%100,2,16,0x80);
		LED0(led0sta^=1);
		delay_ms(250);
	}
}
