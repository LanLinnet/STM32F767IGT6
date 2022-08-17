#include "sys.h" 
#include "delay.h" 
#include "led.h"
#include "mpu.h"
#include "usart.h"
#include "lcd.h"
#include "sdram.h"
#include "key.h"
#include "usmart.h"
#include "w25qxx.h"

const u8 TEXT_Buffer[]={"Apollo STM32F7 QSPI TEST"};
#define SIZE sizeof(TEXT_Buffer)
	
int main(void)
{ 
 	u8 led0sta=1; 		//LED��
	u8 key;
	u16 i=0;
	u8 datatemp[SIZE];
	u32 FLASH_SIZE;	
	
	Stm32_Clock_Init(432, 25, 2, 9);
	delay_init(216);
	uart_init(108,115200);
	usmart_dev.init(108);
	LED_Init();
	MPU_Memory_Protection();
	SDRAM_Init();
	LCD_Init();
	KEY_Init();
	
	W25QXX_Init();
		
	POINT_COLOR = RED;		//��ɫ����
	LCD_ShowString(30,50,200,16,16,(u8 *)"Apollo STM32F4/F7"); 
	LCD_ShowString(30,70,200,16,16,(u8 *)"QSPI TEST");	
	LCD_ShowString(30,90,200,16,16,(u8 *)"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,(u8 *)"2022/8/16");
	LCD_ShowString(30,130,200,16,16,(u8 *)"KEY1:Write  KEY0:Read");	//��ʾ��ʾ��Ϣ
	
	while(W25QXX_ReadID()!=W25Q256)	//δ��⵽AP3216C
	{
		LCD_ShowString(30,150,200,16,16,(u8 *)"W25Q256 Check Failed!");
		delay_ms(500);
		LCD_ShowString(30,150,200,16,16,(u8 *)"Please Check!        ");
		delay_ms(500);
		LED0(led0sta^=1);
	}
	
	LCD_ShowString(30,150,200,16,16,(u8 *)"W25Q256 Ready!");
	FLASH_SIZE=32*1024*1024;
	POINT_COLOR = BLUE;		//��ɫ����
	
	while(1)
	{
		key=KEY_Scan(0);
		if(key==KEY1_PRES)//KEY1����,д��W25Q256
		{
			LCD_Fill(0,170,239,319,WHITE);//�������    
 			LCD_ShowString(30,170,200,16,16,(u8 *)"Start Write W25Q256....");
			W25QXX_Write((u8*)TEXT_Buffer,FLASH_SIZE-100,SIZE);		//�ӵ�����100����ַ����ʼ,д��SIZE���ȵ�����
			LCD_ShowString(30,170,200,16,16,(u8 *)"W25Q256 Write Finished!");	//��ʾ�������
		}
		if(key==KEY0_PRES)//KEY0����,��ȡ�ַ�������ʾ
		{
 			LCD_ShowString(30,170,200,16,16,(u8 *)"Start Read W25Q256.... ");
			W25QXX_Read(datatemp,FLASH_SIZE-100,SIZE);					//�ӵ�����100����ַ����ʼ,����SIZE���ֽ�
			LCD_ShowString(30,170,200,16,16,(u8 *)"The Data Readed Is:   ");	//��ʾ�������
			LCD_ShowString(30,190,200,16,16,datatemp);					//��ʾ�������ַ���
		} 
		i++;
		delay_ms(10);
		if(i==20)
		{
			LED0(led0sta^=1);	//DS0��˸
			i=0;
		}
	}
}
