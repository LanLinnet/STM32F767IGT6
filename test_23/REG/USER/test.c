#include "sys.h" 
#include "delay.h" 
#include "led.h"
#include "mpu.h"
#include "usart.h"
#include "lcd.h"
#include "sdram.h"
#include "key.h"
#include "usmart.h"
#include "24cxx.h"


//要写入到24c02的字符串数组
const u8 TEXT_Buffer[]={"Apollo STM32F7 IIC TEST"};
#define SIZE sizeof(TEXT_Buffer)
	
int main(void)
{ 
 	u8 led0sta=1; 		//LED灯
	u8 key;
	u16 i=0;
	u8 datatemp[SIZE];
	
	Stm32_Clock_Init(432, 25, 2, 9);
	delay_init(216);
	uart_init(108,115200);
	usmart_dev.init(108);
	LED_Init();
	MPU_Memory_Protection();
	SDRAM_Init();
	LCD_Init();
	
	KEY_Init();		//按键初始化
	AT24CXX_Init();	//IIC初始化
		
	POINT_COLOR = RED;		//红色画笔
	LCD_ShowString(30,50,200,16,16,(u8 *)"Apollo STM32F4/F7"); 
	LCD_ShowString(30,70,200,16,16,(u8 *)"IIC TEST");	
	LCD_ShowString(30,90,200,16,16,(u8 *)"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,(u8 *)"2022/8/10");
	LCD_ShowString(30,130,200,16,16,(u8 *)"KEY1: Write  KEY0: Read");
	
	while(AT24CXX_Check())	//未检测到24C02
	{
		LCD_ShowString(30,150,200,16,16,(u8 *)"24C02 Check Failed!");
		delay_ms(500);
		LCD_ShowString(30,150,200,16,16,(u8 *)"Please Check!        ");
		delay_ms(500);
		LED0(led0sta^=1);
	}
	
	LCD_ShowString(30,150,200,16,16,(u8 *)"24C02 Ready!");
	POINT_COLOR = BLUE;		//蓝色画笔
	
	while(1)
	{
		key=KEY_Scan(0);
		if(key==KEY1_PRES)
		{
			LCD_Fill(0,170,239,319,WHITE);
			LCD_ShowString(30,170,200,16,16,(u8 *)"Start write 24C02....");
			AT24CXX_Write(0,(u8*)TEXT_Buffer, SIZE);
			LCD_ShowString(30,170,200,16,16,(u8 *)"24C02 write finished!");
		}
		if(key==KEY0_PRES)
		{
			LCD_ShowString(30,170,200,16,16,(u8 *)"Start read 24C02....");
			AT24CXX_Read(0,datatemp,SIZE);
			LCD_ShowString(30,170,200,16,16,(u8 *)"The data readed is:    ");
			LCD_ShowString(30,190,200,16,16,datatemp);
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
