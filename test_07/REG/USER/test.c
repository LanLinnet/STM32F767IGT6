#include "sys.h" 
#include "delay.h" 
#include "led.h"
#include "timer.h"

int main(void)
{ 
	u8 led0sta=1;		//初始化熄灭
	Stm32_Clock_Init(432, 25, 2, 9);
	delay_init(216);
	LED_Init();
	TIM3_Int_Init(5000-1,10800-1);		//10khz计数频率，计数5k次为500ms
	
	while(1)
	{
		LED0(led0sta^=1);		//LED0翻转
		delay_ms(200);
	}
}
