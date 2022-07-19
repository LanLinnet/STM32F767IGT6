#include "sys.h" 
#include "delay.h" 
#include "led.h"
#include "timer.h"
#include "usart.h"

extern u8 TIM5CH1_CAPTURE_STA;		//输入捕获状态
extern u32 TIM5CH1_CAPTURE_VAL;		//输入捕获值
		
int main(void)
{ 
	long long temp=0;	//中间变量，用于计算脉冲宽度
	Stm32_Clock_Init(432, 25, 2, 9);
	delay_init(216);
	uart_init(108,115200);
	LED_Init();
	TIM3_PWM_Init(500-1,108-1);		//1MHz计数频率，2kHz的PWM
	TIM5_CH1_Cap_Init(0xffffffff, 108-1);		//以1MHz的频率计数

	while(1)
	{
		delay_ms(10);
		LED0_PWM_VAL++;
		if(LED0_PWM_VAL==300)LED0_PWM_VAL=0;
		if(TIM5CH1_CAPTURE_STA&0x80)		//成功捕获到了一次高电平
		{
			temp=TIM5CH1_CAPTURE_STA&0x3f;		//有temp个溢出
			temp*=0xffffffff;		//溢出时间总和
			temp+=TIM5CH1_CAPTURE_VAL;		//得到总的高电平时间  temp*ARR+CRRx2
			printf("HIGH: %lld us\r\n",temp);		//打印计算出的高电平时间
			TIM5CH1_CAPTURE_STA=0;		//开启下一次捕获
		}
	}	
}
