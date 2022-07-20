#include "sys.h" 
#include "delay.h" 
#include "led.h"
#include "usart.h"
#include "tpad.h"

extern u8 TIM5CH1_CAPTURE_STA;		//输入捕获状态
extern u32 TIM5CH1_CAPTURE_VAL;		//输入捕获值
		
int main(void)
{ 
	u8 t=0, led0sta=1, led1sta=1;
	
	Stm32_Clock_Init(432, 25, 2, 9);
	delay_init(216);
	uart_init(108,115200);
	LED_Init();
	TPAD_Init(2);		//初始化触摸按键，二分频，以108/2=54MHz频率计数
	
	while(1)
	{
		if(TPAD_Scan(0))		//捕捉到一次上升沿 mode0 不连续触发
		{
			LED1(led1sta^=1);	//LED1翻转
		}
		t++;
		if(t==15)
		{
			t=0;
			LED0(led0sta^=1);	//LED0翻转，提示程序正在运行
		}
		delay_ms(10);
	}
}
