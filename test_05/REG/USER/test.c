#include "sys.h" 
#include "delay.h" 
#include "led.h"
#include "key.h"
#include "wdg.h"

int main(void)
{ 
	Stm32_Clock_Init(432, 25, 2, 9);
	delay_init(216);
	LED_Init();
	KEY_Init();
	delay_ms(100);		//延时100ms再初始化看门狗，LED0的变化“可见”
	IWDG_Init(4, 500);	//预分频系数为4*2^4 = 64，重载值为500， 溢出时间为64*500/32=1000ms，即1s
	LED0(0);			//先点亮红灯
	while(1)
	{
		if(KEY_Scan(0)==WKUP_PRES)	//如果WK_UP按下，则喂狗，  0-不支持连续按
		{
			IWDG_Feed();	//喂狗
		}
		delay_ms(10);
	}
}
