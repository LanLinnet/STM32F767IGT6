#include "sys.h" 
#include "delay.h" 
#include "led.h"
#include "mpu.h"
#include "usart.h"
#include "lcd.h"
#include "sdram.h"
#include "key.h"
#include "usmart.h"

#include "can.h"
	
int main(void)
{ 
 	u8 led0sta=1; 		//LED��
	u8 key;
	u16 i=0,t=0,cnt=0;
	
	u8 canbuf[8];
	u8 res;
	u8 mode=1;  	//CAN����ģʽ��0-��ͨ��1-����
	
	Stm32_Clock_Init(432, 25, 2, 9);
	delay_init(216);
	uart_init(108,115200);
	usmart_dev.init(108);
	LED_Init();
	MPU_Memory_Protection();
	SDRAM_Init();
	LCD_Init();
	KEY_Init();
	
	CAN1_Mode_Init(1,7,10,6,1);		//CAN��ʼ����������500kbps
		
	POINT_COLOR = RED;		//��ɫ����
	LCD_ShowString(30,50,200,16,16,(u8 *)"Apollo STM32F4/F7"); 
	LCD_ShowString(30,70,200,16,16,(u8 *)"CAN TEST");	
	LCD_ShowString(30,90,200,16,16,(u8 *)"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,(u8 *)"2022/10/11");
	LCD_ShowString(30,130,200,16,16,(u8 *)"LoopBack Mode");
	LCD_ShowString(30,150,200,16,16,(u8 *)"KEY0:Send WK_UP:Mode");	//��ʾ��ʾ��Ϣ
	
	POINT_COLOR = BLUE;		//��ɫ����
	LCD_ShowString(30,170,200,16,16,(u8 *)"Count:");
	LCD_ShowString(30,190,200,16,16,(u8 *)"Send Data:");
	LCD_ShowString(30,250,200,16,16,(u8 *)"Receive Data:");
	
	
	while(1)
	{
		key=KEY_Scan(0);
		if(key==KEY0_PRES)//KEY0���£�����һ������
		{
			for(i=0;i<8;i++)
			{
				canbuf[i]=cnt+i;	//��䷢�ͻ�����
				if(i<4) LCD_ShowxNum(30+i*32,210,canbuf[i],3,16,0X80);	//��ʾ����
				else LCD_ShowxNum(30+(i-4)*32,230,canbuf[i],3,16,0X80);	//��ʾ����
				
			}
 			res=CAN1_Send_Msg(canbuf,8);
			if(res)LCD_ShowString(30+80,190,200,16,16,(u8 *)"Failed");		//��ʾ����ʧ��
			else LCD_ShowString(30+80,190,200,16,16,(u8 *)"OK    ");	 		//��ʾ���ͳɹ�
		}else if(key==WKUP_PRES) 
		{
			mode=!mode;		//�ı�CAN�Ĺ���ģʽ
			CAN1_Mode_Init(1,7,10,6,mode);
			  			POINT_COLOR=RED;//��������Ϊ��ɫ 
			if(mode==0)//��ͨģʽ����Ҫ2��������
			{
				LCD_ShowString(30,130,200,16,16,(u8 *)"Normal   Mode ");	    
			}else //�ػ�ģʽ,һ��������Ϳ��Բ�����.
			{
 				LCD_ShowString(30,130,200,16,16,(u8 *)"LoopBack Mode");
			}
 			POINT_COLOR=BLUE;//��������Ϊ��ɫ 
		}
		key=CAN1_Receive_Msg(canbuf);
		if(key)
		{
			LCD_Fill(30,270,160,310,WHITE);//���֮ǰ����ʾ
 			for(i=0;i<key;i++)
			{									    
				if(i<4)LCD_ShowxNum(30+i*32,270,canbuf[i],3,16,0X80);	//��ʾ����
				else LCD_ShowxNum(30+(i-4)*32,290,canbuf[i],3,16,0X80);	//��ʾ����
 			}
		}
		
		t++;
		delay_ms(10);
		if(t==20)
		{
			LED0(led0sta^=1);	//DS0��˸
			t=0;
			cnt++;
			LCD_ShowxNum(30+48,170,cnt,3,16,0x80);
		}
	}
}
