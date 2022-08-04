#include "wkup.h"
#include "usart.h"


#define LED0(n)		(n?HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_RESET))

//系统进入待机模式
void Sys_Enter_Standby(void)
{
    __HAL_RCC_AHB1_FORCE_RESET();       //复位所有IO口  
	while(WKUP_KD);                     //等待WK_UP按键松开(在有RTC中断时,必须等WK_UP松开再进入待机)
    __HAL_RCC_PWR_CLK_ENABLE();         //使能PWR时钟
        
    __HAL_RCC_BACKUPRESET_FORCE();      //复位备份区域
    HAL_PWR_EnableBkUpAccess();         //后备区域访问使能  
	
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);
  
    __HAL_RCC_BACKUPRESET_RELEASE();                    //备份区域复位结束
    __HAL_PWR_CLEAR_WAKEUP_FLAG(PWR_WAKEUP_PIN_FLAG1);  //清除Wake_UP标志
	
	
    HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);           //设置WKUP用于唤醒
    HAL_PWR_EnterSTANDBYMode();                         //进入待机模式     
}

//检测WKUP脚的信号
//返回值1:连续按下3s以上
//      0:错误的触发	
uint8_t Check_WKUP(void) 
{
	uint8_t t=0;
	uint8_t tx=0;//记录松开的次数
	LED0(0); //亮灯DS0 
	while(1)
	{
		if(WKUP_KD)//已经按下了
		{
			t++;
			tx=0;
		}else 
		{
			tx++; 
			if(tx>3)//超过90ms内没有WKUP信号
			{
				LED0(1);
				return 0;//错误的按键,按下次数不够
			}
		}
		HAL_Delay(30);
		if(t>=100)//按下超过3秒钟
		{
			LED0(0);    //点亮DS0 
			return 1;   //按下3s以上了
		}
	}
}  

//中断线0中断处理过程
//此函数会被HAL_GPIO_EXTI_IRQHandler()调用
//GPIO_Pin:引脚
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if(GPIO_Pin==GPIO_PIN_0)//PA0
    {
        if(Check_WKUP())//关机
        {
            Sys_Enter_Standby();//进入待机模式
        }
    }    
}

//PA0 WKUP唤醒初始化
void WKUP_Init(void)
{   
    //检查是否是正常开机
    if(Check_WKUP()==0)
    {
        Sys_Enter_Standby();//不是开机，进入待机模式
    }
}
