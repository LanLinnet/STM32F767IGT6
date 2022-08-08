#include "sys.h" 
#include "delay.h" 
#include "led.h"
#include "mpu.h"
#include "usart.h"
#include "lcd.h"
#include "sdram.h"
#include "key.h"
#include "adc.h"
#include "dac.h"
#include "usmart.h"

int main(void)
{ 
 	u8 led0sta=1; 		//LED��
	u16 adcx;
	float temp;
	u8 t=0;
	u16 dacval=0;
	u8 key;
	
	Stm32_Clock_Init(432, 25, 2, 9);
	delay_init(216);
	uart_init(108,115200);
	usmart_dev.init(108);		//��ʼ��USMART
	LED_Init();
	MPU_Memory_Protection();
	SDRAM_Init();
	LCD_Init();
	
	Adc_Init();		//��ʼ��ADC
	KEY_Init();		//������ʼ��
	Dac1_Init();	//DACͨ��1��ʼ��
	
	
	POINT_COLOR = RED;		//��ɫ����
	LCD_ShowString(30,50,200,16,16,(u8 *)"Apollo STM32F4/F7"); 
	LCD_ShowString(30,70,200,16,16,(u8 *)"DAC TEST");	
	LCD_ShowString(30,90,200,16,16,(u8 *)"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,(u8 *)"2022/8/8");
	LCD_ShowString(30,130,200,16,16,(u8 *)"WK_UP: +  KEY1: -");
	
	POINT_COLOR = BLUE;		//��ɫ����
	LCD_ShowString(30,150,200,16,16,(u8 *)"DAC VAL:");
	LCD_ShowString(30,170,200,16,16,(u8 *)"DAC VOL:0.000V");
	LCD_ShowString(30,190,200,16,16,(u8 *)"ADC VOL:0.000V");
	
	DAC->DHR12R1=dacval;		//��ʼֵΪ0
	
	while(1)
	{
		t++;
		key=KEY_Scan(0);
		if(key==WKUP_PRES)
		{
			if(dacval<4000)dacval+=200;
			DAC->DHR12R1=dacval;
		}else if(key==KEY1_PRES)
		{
			if(dacval>200)dacval-=200;
			else dacval=0;
			DAC->DHR12R1=dacval;
		}
		if(t==10||key==KEY1_PRES||key==WKUP_PRES)
		{
			adcx=DAC->DHR12R1;
			LCD_ShowxNum(94,150,adcx,4,16,0);
			temp=(float)adcx*(3.3/4096);
			adcx=temp;
			LCD_ShowxNum(94,170,temp,1,16,0);		//��������
			temp-=adcx;
			temp*=1000;
			LCD_ShowxNum(110,170,temp,3,16,0x80);	//С������
			adcx=Get_Adc_Average(ADC_CH5,20);		//�õ�ADCת��ֵ
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