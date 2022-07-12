#include "sys.h" 
#include "delay.h" 
#include "led.h"
#include "exti.h"

int main(void)
{ 
	Stm32_Clock_Init(432, 25, 2, 9);
	delay_init(216);
	LED_Init();
	EXTIX_Init();		//初始化外部中断输入
	LED0(0);			//先点亮红灯
	while(1)
	{
		delay_ms(1000);
	}
}
