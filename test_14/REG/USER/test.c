#include "sys.h" 
#include "delay.h" 
#include "led.h"
#include "usart.h"

#include "sdram.h"
#include "mpu.h"
#include "lcd.h"
#include "usmart.h"

int main(void)
{ 
	u8 led0sta=1;
	
	Stm32_Clock_Init(432, 25, 2, 9);
	delay_init(216);
	uart_init(108,115200);
	
	usmart_dev.init(108);  //��ʼ��USMART
	
	LED_Init();
	MPU_Memory_Protection();
	SDRAM_Init();
	LCD_Init();
	
	POINT_COLOR = RED;		//��ɫ����
	LCD_ShowString(30,50,200,16,16,(u8 *)"Apollo STM32F4/F7"); 	
	LCD_ShowString(30,70,200,16,16,(u8 *)"USMART TEST");      					 
	LCD_ShowString(30,110,200,16,16,(u8 *)"2022/07/28");
	
  	while(1) 
	{		    					 
		LED0(led0sta^=1);	//LED0��˸	 
		delay_ms(500);	
	}
}

//LED״̬���ú���
void led_set(u8 sta)
{
	LED1(sta);
}

//�����������ò��Ժ���
void test_fun(void (*ledset)(u8), u8 sta)
{
	ledset(sta);
}
