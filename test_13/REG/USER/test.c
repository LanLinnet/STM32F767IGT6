#include "sys.h" 
#include "delay.h" 
#include "led.h"
#include "usart.h"

#include "sdram.h"
#include "mpu.h"
#include "lcd.h"
		
int main(void)
{ 
	u8 x=0, led0sta=1;
	u8 lcd_id[12];
	
	Stm32_Clock_Init(432, 25, 2, 9);
	delay_init(216);
	uart_init(108,115200);
	LED_Init();
	MPU_Memory_Protection();
	SDRAM_Init();
	LCD_Init();
	
	POINT_COLOR = RED;
	sprintf((char*)lcd_id,"LCD ID:%04X",lcddev.id);//将LCD ID打印到lcd_id数组。
	
  	while(1) 
	{		 
		switch(x)
		{
			case 0:LCD_Clear(WHITE);break;
			case 1:LCD_Clear(BLACK);break;
			case 2:LCD_Clear(BLUE);break;
			case 3:LCD_Clear(RED);break;
			case 4:LCD_Clear(MAGENTA);break;
			case 5:LCD_Clear(GREEN);break;
			case 6:LCD_Clear(CYAN);break; 
			case 7:LCD_Clear(YELLOW);break;
			case 8:LCD_Clear(BRRED);break;
			case 9:LCD_Clear(GRAY);break;
			case 10:LCD_Clear(LGRAY);break;
			case 11:LCD_Clear(BROWN);break;
		}
		POINT_COLOR=RED;	  
		LCD_ShowString(10,40,240,32,32,(u8 *)"Apollo STM32"); 	
		LCD_ShowString(10,80,240,24,24,(u8 *)"LTDC LCD TEST");
 		LCD_ShowString(10,130,240,16,16,lcd_id);		//显示LCD ID	      					 
		LCD_ShowString(10,150,240,12,12,(u8 *)"2022/07/27");	      					 
	    x++;
		if(x==12)x=0;
		LED0(led0sta^=1);	//LED0闪烁	 
		delay_ms(1000);	
	}
}
