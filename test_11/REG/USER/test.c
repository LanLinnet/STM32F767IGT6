#include "sys.h" 
#include "delay.h" 
#include "led.h"
#include "usart.h"
#include "key.h"
#include "mpu.h"

u8 mpudata[128]__attribute__((at(0x20002000)));		//����һ�����飬���׵�ַΪ0x20002000
		
int main(void)
{ 
	u8 i=0, led0sta=1;
	u8 key;
	
	Stm32_Clock_Init(432, 25, 2, 9);
	delay_init(216);
	uart_init(108,115200);
	LED_Init();
	KEY_Init();
	
	printf("\r\n\r\nMPU closed!\r\n");		//��ʾMPU�ر�
	
	while(1)
	{
		key=KEY_Scan(0);
		if(key==WKUP_PRES)		//ʹ��MPU��������
		{
			MPU_Set_Protection(0x20002000, 128, 0, MPU_REGION_PRIV_RO_URO, 0, 0, 1);		//ֻ������ֹ���ã���ֹcache��������
			printf("MPU open!\r\n");		//��ʾMPU��
		}else if(key==KEY0_PRES)		//��������д������
		{
			printf("Start Writing data...\r\n");
			sprintf((char*)mpudata,"MPU test array %d",i);
			printf("Data Write finshed!\r\n");
		}else if(key==KEY1_PRES)		//�������ж�ȡ����
		{
			printf("Array data is:%s\r\n",mpudata);
		}else delay_ms(10);
		i++;
		if((i%50)==0) LED0(led0sta^=1);	//LED0��ת
	}
}
