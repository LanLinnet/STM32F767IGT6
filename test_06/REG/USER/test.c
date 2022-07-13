#include "sys.h" 
#include "delay.h" 
#include "led.h"
#include "wdg.h"

int main(void)
{ 
	Stm32_Clock_Init(432, 25, 2, 9);
	delay_init(216);
	LED_Init();
	LED0(0);			//先点亮红灯
	delay_ms(300);		//延时300ms再初始化看门狗，LED0的变化“可见”
	WWDG_Init(0x7f, 0x5f, 3);		//计数器值为7f，上窗口值为5f， 分频系数为8（2^3）
	
	while(1)
	{
		LED0(1);		//关闭红灯
	}
}
