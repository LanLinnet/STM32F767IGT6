 #include "led.h"
 
void LED_Init(void)
{
	RCC->AHB1ENR|=1<<1;	//GPIOBʱ��ʹ��
	//�����峧���ṩ��������������IO�ڵ�ģʽ�����͡��ٶȡ�����λ�ȣ�֧�ֶ��IOͬʱ���á�
	GPIO_Set(GPIOB, PIN0|PIN1, GPIO_MODE_OUT, GPIO_OTYPE_PP, GPIO_SPEED_100M, GPIO_PUPD_PU);
	LED0(1);	//�ر�DS0
	LED1(1);	//�ر�DS1
}
