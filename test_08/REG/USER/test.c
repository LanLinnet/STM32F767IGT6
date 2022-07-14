#include "sys.h" 
#include "delay.h" 
#include "led.h"
#include "timer.h"

int main(void)
{ 
	u16 led0pwmval=0;
	u8 dir=1;

	Stm32_Clock_Init(432, 25, 2, 9);
	delay_init(216);
	LED_Init();
	
	TIM3_PWM_Init(500-1,108-1);		//1MHz计数频率，2kHz的PWM
	
	while(1)
	{
		delay_ms(10);
		if(dir)led0pwmval++;	//CCR上升， 低电平占比上升（占空比减小），平均电压下降，亮度升高
		else led0pwmval--;
		if(led0pwmval>300)dir=0;
		if(led0pwmval==0)dir=1;
		LED0_PWM_VAL=led0pwmval;
	}
}
