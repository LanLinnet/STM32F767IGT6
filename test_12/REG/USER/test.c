#include "sys.h"
#include "delay.h" 
#include "led.h"  
#include "usart.h" 
#include "key.h" 
#include "mpu.h"  
#include "sdram.h" 

//������������飬�������ⲿSDRAM��ʼ��ַ
u16 testsram[250000] __attribute__((at(0XC0000000)));
//����SDRAM�ڴ���Ժ���
void fmc_sdram_test(u16 x,u16 y);

int main(void)
{  
	u8 key;		 
 	u8 i=0,led0sta=1;	     
	u32 ts=0; 
	Stm32_Clock_Init(432,25,2,9);//����ʱ��,216Mhz
    delay_init(216);			//��ʱ��ʼ��  
	uart_init(108,115200);		//��ʼ�����ڲ�����Ϊ115200
	LED_Init();		  			//��ʼ����LED���ӵ�Ӳ���ӿ�
	MPU_Memory_Protection();	//������ش洢����
	KEY_Init();					//��ʼ��LED
	SDRAM_Init();				//��ʼ��SDRAM 
	//LCD_Init();					//��ʼ��LCD
 	//POINT_COLOR=RED;			//��������Ϊ��ɫ
	
//	LCD_ShowString(30,50,200,16,16,"APOLLO STM32F4/F7"); 
//	LCD_ShowString(30,70,200,16,16,"SDRAM TEST");	
//	LCD_ShowString(30,110,200,16,16,"2022/7/25");   
//	LCD_ShowString(30,130,200,16,16,"KEY0:Test Sram");
//	LCD_ShowString(30,150,200,16,16,"KEY1:TEST Data");
	printf("APOLLO STM32F4/F7\r\n");
	printf("SDRAM TEST\r\n");
	printf("2022/7/25\r\n");
	printf("KEY0:Test Sram\r\n");
	printf("KEY1:TEST Data\r\n\r\n");
	
 	//POINT_COLOR=BLUE;//��������Ϊ��ɫ  
	for(ts=0;ts<250000;ts++)
	{
		testsram[ts]=ts;//Ԥ���������	 
  	}
	while(1)
	{	
		key=KEY_Scan(0);//��֧������	
		if(key==KEY0_PRES)fmc_sdram_test(40,170);//����SDRAM����
		else if(key==KEY1_PRES)//��ӡԤ���������
		{
			for(ts=0;ts<250000;ts++)
			{
				//LCD_ShowxNum(40,190,testsram[ts],6,16,0);//��ʾ��������	
				printf("testsram[%d]:%d\r\n",ts,testsram[ts]);
			}
		}else delay_ms(10);   
		i++;
		if(i==20)//DS0��˸.
		{
			i=0;
			LED0(led0sta^=1);
 		}
	}
}

//SDRAM�ڴ����	    
void fmc_sdram_test(u16 x,u16 y)
{  
	u32 i=0;  	  
	u32 temp=0;	   
	u32 sval=0;	//�ڵ�ַ0����������	  				   
   //LCD_ShowString(x,y,239,y+16,16,"Ex Memory Test:    0KB"); 
	//ÿ��16K�ֽ�,д��һ������,�ܹ�д��2048������,�պ���32M�ֽ�
	for(i=0;i<32*1024*1024;i+=16*1024)
	{
		*(vu32*)(Bank5_SDRAM_ADDR+i)=temp; 
		temp++;
	}
	//���ζ���֮ǰд�������,����У��		  
 	for(i=0;i<32*1024*1024;i+=16*1024) 
	{	
  		temp=*(vu32*)(Bank5_SDRAM_ADDR+i);
		if(i==0)sval=temp;
 		else if(temp<=sval)break;//�������������һ��Ҫ�ȵ�һ�ζ��������ݴ�.	   		   
		//LCD_ShowxNum(x+15*8,y,(u16)(temp-sval+1)*16,5,16,0);	//��ʾ�ڴ�����  
		printf("SDRAM Capacity:%dKB\r\n",(u16)(temp-sval+1)*16);//��ӡSDRAM����
 	}					 
}

