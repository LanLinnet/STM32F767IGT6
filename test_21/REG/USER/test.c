#include "sys.h" 
#include "delay.h" 
#include "led.h"
#include "mpu.h"
#include "usart.h"
#include "lcd.h"
#include "sdram.h"
#include "key.h"
#include "adc.h"
#include "timer.h"
#include "usmart.h"


//设置输出电压
void PWM_DAC_Set(u16 vol)
{
	double temp=vol;
	temp/=100;
	temp=temp*256/3.3;
	PWM_DAC_VAL=temp;
}

int main(void)
{ 
 	u8 led0sta=1; 		//LED灯
	u16 adcx;
	float temp;
	u8 t=0;
	u8 key;
	u16 pwmval=0;
	
	Stm32_Clock_Init(432, 25, 2, 9);
	delay_init(216);
	uart_init(108,115200);
	usmart_dev.init(108);		//初始化USMART
	LED_Init();
	MPU_Memory_Protection();
	SDRAM_Init();
	LCD_Init();
	
	Adc_Init();		//初始化ADC
	KEY_Init();		//按键初始化
	TIM9_CH2_PWM_Init(255,1);	//TIM9 PWM初始化 Fpwm=108M/256=421.9kHz
	
	
	POINT_COLOR = RED;		//红色画笔
	LCD_ShowString(30,50,200,16,16,(u8 *)"Apollo STM32F4/F7"); 
	LCD_ShowString(30,70,200,16,16,(u8 *)"PWM DAC TEST");	
	LCD_ShowString(30,90,200,16,16,(u8 *)"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,(u8 *)"2022/8/8");
	LCD_ShowString(30,130,200,16,16,(u8 *)"WK_UP: +  KEY1: -");
	
	POINT_COLOR = BLUE;		//蓝色画笔
	LCD_ShowString(30,150,200,16,16,(u8 *)"DAC VAL:");
	LCD_ShowString(30,170,200,16,16,(u8 *)"DAC VOL:0.000V");
	LCD_ShowString(30,190,200,16,16,(u8 *)"ADC VOL:0.000V");

	
	while(1)
	{
		t++;
		key=KEY_Scan(0);
		if(key==WKUP_PRES)
		{
			if(pwmval<250)pwmval+=10;
			PWM_DAC_VAL=pwmval;
		}else if(key==KEY1_PRES)
		{
			if(pwmval>10)pwmval-=10;
			else pwmval=0;
			PWM_DAC_VAL=pwmval;
		}
		if(t==10||key==KEY1_PRES||key==WKUP_PRES)
		{
			adcx=PWM_DAC_VAL;
			LCD_ShowxNum(94,150,adcx,4,16,0);
			temp=(float)adcx*(3.3/256);
			adcx=temp;
			LCD_ShowxNum(94,170,temp,1,16,0);		//整数部分
			temp-=adcx;
			temp*=1000;
			LCD_ShowxNum(110,170,temp,3,16,0x80);	//小数部分
			adcx=Get_Adc_Average(ADC_CH5,20);		//得到ADC转换值
			temp=(float)adcx*(3.3/4096);
			adcx=temp;
			LCD_ShowxNum(94,190,temp,1,16,0);
			temp-=adcx;
			temp*=1000;
			LCD_ShowxNum(110,190,temp,3,16,0x80);
			LED0(led0sta^=1);
			t=0;
		}
		delay_ms(10);
	}	
}
