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
 	u8 led0sta=1; 		//LED灯
	u16 adcx;
	float temp;
	
	Stm32_Clock_Init(432, 25, 2, 9);
	delay_init(216);
	uart_init(108,115200);
	LED_Init();
	MPU_Memory_Protection();
	SDRAM_Init();
	LCD_Init();
	
	Adc_Init();		//初始化ADC
	
	
	POINT_COLOR = RED;		//红色画笔
	LCD_ShowString(30,50,200,16,16,(u8 *)"Apollo STM32F4/F7"); 
	LCD_ShowString(30,70,200,16,16,(u8 *)"ADC TEST");	
	LCD_ShowString(30,90,200,16,16,(u8 *)"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,(u8 *)"2022/8/5");
	
	POINT_COLOR = BLUE;		//蓝色画笔
	LCD_ShowString(30,130,200,16,16,(u8 *)"ADC1_CH5_VAL:");
	LCD_ShowString(30,150,200,16,16,(u8 *)"ADC1_CH5_VOL:0.000V");
	
	while(1)
	{
		adcx=Get_Adc_Average(ADC_CH5,20);
		LCD_ShowxNum(134,130,adcx,4,16,0);
		temp=(float)adcx*(3.3/4096);			//12位分辨率，量程是4096
		adcx=temp;
		LCD_ShowxNum(134,150,adcx,1,16,0);		//显示电压值整数部分
		temp-=adcx;
		temp*=1000;
		LCD_ShowxNum(150,150,temp,3,16,0x80);	//显示电压值小数部分
		
		LED0(led0sta^=1);
		delay_ms(250);
	}
}
