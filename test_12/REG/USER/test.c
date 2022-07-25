#include "sys.h"
#include "delay.h" 
#include "led.h"  
#include "usart.h" 
#include "key.h" 
#include "mpu.h"  
#include "sdram.h" 

//定义测试用数组，定义在外部SDRAM起始地址
u16 testsram[250000] __attribute__((at(0XC0000000)));
//声明SDRAM内存测试函数
void fmc_sdram_test(u16 x,u16 y);

int main(void)
{  
	u8 key;		 
 	u8 i=0,led0sta=1;	     
	u32 ts=0; 
	Stm32_Clock_Init(432,25,2,9);//设置时钟,216Mhz
    delay_init(216);			//延时初始化  
	uart_init(108,115200);		//初始化串口波特率为115200
	LED_Init();		  			//初始化与LED连接的硬件接口
	MPU_Memory_Protection();	//保护相关存储区域
	KEY_Init();					//初始化LED
	SDRAM_Init();				//初始化SDRAM 
	//LCD_Init();					//初始化LCD
 	//POINT_COLOR=RED;			//设置字体为红色
	
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
	
 	//POINT_COLOR=BLUE;//设置字体为蓝色  
	for(ts=0;ts<250000;ts++)
	{
		testsram[ts]=ts;//预存测试数据	 
  	}
	while(1)
	{	
		key=KEY_Scan(0);//不支持连按	
		if(key==KEY0_PRES)fmc_sdram_test(40,170);//测试SDRAM容量
		else if(key==KEY1_PRES)//打印预存测试数据
		{
			for(ts=0;ts<250000;ts++)
			{
				//LCD_ShowxNum(40,190,testsram[ts],6,16,0);//显示测试数据	
				printf("testsram[%d]:%d\r\n",ts,testsram[ts]);
			}
		}else delay_ms(10);   
		i++;
		if(i==20)//DS0闪烁.
		{
			i=0;
			LED0(led0sta^=1);
 		}
	}
}

//SDRAM内存测试	    
void fmc_sdram_test(u16 x,u16 y)
{  
	u32 i=0;  	  
	u32 temp=0;	   
	u32 sval=0;	//在地址0读到的数据	  				   
   //LCD_ShowString(x,y,239,y+16,16,"Ex Memory Test:    0KB"); 
	//每隔16K字节,写入一个数据,总共写入2048个数据,刚好是32M字节
	for(i=0;i<32*1024*1024;i+=16*1024)
	{
		*(vu32*)(Bank5_SDRAM_ADDR+i)=temp; 
		temp++;
	}
	//依次读出之前写入的数据,进行校验		  
 	for(i=0;i<32*1024*1024;i+=16*1024) 
	{	
  		temp=*(vu32*)(Bank5_SDRAM_ADDR+i);
		if(i==0)sval=temp;
 		else if(temp<=sval)break;//后面读出的数据一定要比第一次读到的数据大.	   		   
		//LCD_ShowxNum(x+15*8,y,(u16)(temp-sval+1)*16,5,16,0);	//显示内存容量  
		printf("SDRAM Capacity:%dKB\r\n",(u16)(temp-sval+1)*16);//打印SDRAM容量
 	}					 
}

