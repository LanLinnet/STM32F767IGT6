 #include "led.h"
 
void LED_Init(void)
{
	RCC->AHB1ENR|=1<<1;	//GPIOB时钟使能
	//开发板厂商提供函数：用于配置IO口的模式、类型、速度、上下位等，支持多个IO同时配置。
	GPIO_Set(GPIOB, PIN0|PIN1, GPIO_MODE_OUT, GPIO_OTYPE_PP, GPIO_SPEED_100M, GPIO_PUPD_PU);
	LED0(1);	//关闭DS0
	LED1(1);	//关闭DS1
}
