#include "sys.h"
#include "delay.h"
#include "led.h"
#include "key.h"

int main(void)
{
	u8 key;
	u8 led0sta=1, led1sta=1;
	Stm32_Clock_Init(432,25,2,9);
	delay_init(216);
	LED_Init();
	KEY_Init();
	LED0(0);	//DS0亮
	while(1)
	{
		key=KEY_Scan(0);	//扫描键值
		if(key)	//有按键按下
		{
			switch(key)
			{
				case WKUP_PRES:
					led1sta = !led1sta;
					led0sta = !led1sta;
					break;
				case KEY2_PRES:
					led0sta = !led0sta;
					break;
				case KEY1_PRES:
					led1sta = !led1sta;
					break;
				case KEY0_PRES:
					led0sta=!led0sta;
					led1sta=!led1sta;
					break;
			}
			LED0(led0sta);
			LED1(led1sta);
		}else delay_ms(10);
	}
	
}
