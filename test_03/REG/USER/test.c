#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"

int main(void)
{
	u8 t;
	u8 len;
	u16 times=0;
	u8 led0sta=1;   //LED0初始熄灭
	Stm32_Clock_Init(432,25,2,9);
	delay_init(216);
	uart_init(108, 115200);	//串口初始化为115200
	LED_Init();
	while(1)
	{
		if(USART_RX_STA & 0X8000)  //接收完成
		{
			len=USART_RX_STA & 0x3fff;		//得到此次接收的数据长度（去除前两位）
			printf("\r\n您发送的消息为：\r\n");
			for(t=0; t<len; t++)
			{
				USART1->TDR=USART_RX_BUF[t];
				while((USART1->ISR&0X40)==0);	//等待发送结束
			}
			printf("\r\n\r\n");		//插入换行
			USART_RX_STA=0;		//接收状态标记位清零
		}else{
			times++;
			if(times%5000==0)
			{
				printf("\r\n STM32F7开发板 串口实验 \r\n\r\n");
			}
			if(times%200==0) printf("请输入数据，以回车键结束\r\n");
			if(times%30==0) LED0(led0sta^=1);	//闪烁LED，提示系统正在运行
			delay_ms(10);
		}
	}
}
