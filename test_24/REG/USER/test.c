#include "sys.h" 
#include "delay.h" 
#include "led.h"
#include "mpu.h"
#include "usart.h"
#include "lcd.h"
#include "sdram.h"
#include "key.h"
#include "usmart.h"
#include "pcf8574.h"

	
int main(void)
{ 
 	u8 led0sta=1,led1sta=1; 		//LED��
	u8 key;
	u16 i=0;
	u8 beepsta=1;
	
	Stm32_Clock_Init(432, 25, 2, 9);
	delay_init(216);
	uart_init(108,115200);
	usmart_dev.init(108);
	LED_Init();
	MPU_Memory_Protection();
	SDRAM_Init();
	LCD_Init();
	
	KEY_Init();		//������ʼ��

		
	POINT_COLOR = RED;		//��ɫ����
	LCD_ShowString(30,50,200,16,16,(u8 *)"Apollo STM32F4/F7"); 
	LCD_ShowString(30,70,200,16,16,(u8 *)"PCF8574 TEST");	
	LCD_ShowString(30,90,200,16,16,(u8 *)"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,(u8 *)"2022/8/11");
	LCD_ShowString(30,130,200,16,16,(u8 *)"KEY0:BEEP ON/OFF");
	LCD_ShowString(30,150,200,16,16,(u8 *)"EXIO:DS1 ON/OFF");
	
	while(PCF8574_Init())	//δ��⵽PCF8574
	{
		LCD_ShowString(30,170,200,16,16,(u8 *)"PCF8574 Check Failed!");
		delay_ms(500);
		LCD_ShowString(30,170,200,16,16,(u8 *)"Please Check!        ");
		delay_ms(500);
		LED0(led0sta^=1);
	}
	
	LCD_ShowString(30,170,200,16,16,(u8 *)"PCF8574 Ready!");
	POINT_COLOR = BLUE;		//��ɫ����
	
	while(1)
	{
		key=KEY_Scan(0);
		if(key==KEY0_PRES)
		{
			beepsta=!beepsta;
			PCF8574_WriteBit(BEEP_IO,beepsta);	//��������ת
		}
		if(PCF8574_INT==0)
		{
			key=PCF8574_ReadBit(EX_IO);	//��EXIO״̬�����PCF8574���ж������INT�ָ��ߵ�ƽ������Ϊ�������Ḵλ�ж�
			if(key==0)LED1(led1sta^=1);	//LED1��ת
		}
		i++;
		delay_ms(10);
		if(i==20)
		{
			LED0(led0sta^=1);
			i=0;
		}
	}
}
