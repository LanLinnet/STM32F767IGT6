#include "tpad.h"
#include "delay.h"
#include "usart.h"

#define TPAD_ARR_MAX_VAL 0xffffffff		//最大的ARR值（TIM2是32位寄存器）
vu16 tpad_default_val=0;				//空载的时候（没有手按下），计数器需要的时间

//初始化触摸按键，返回0成功，返回1失败
u8 TPAD_Init(u8 psc)
{
	u16 buf[10];	//存放reset后的计数器值（10次）
	u16 temp;		//临时中间变量
	u8 i,j;			//中间循环变量
	
	TIM2_CH1_Cap_Init(TPAD_ARR_MAX_VAL, psc-1);		//设置分频系数
	
	for(i=0; i<10; i++)		//连续读取10次
	{
		buf[i]=TPAD_Get_Val();
		delay_ms(10);
	}
	
	for(i=0;i<9;i++)
	{
		for(j=i+1;j<10;j++)
		{
			if(buf[i]>buf[j])		//升序
			{
				temp=buf[i];
				buf[i]=buf[j];
				buf[j]=temp;
			}
		}
	}
	
	//取中间6个作为平均值
	temp=0;
	for(i=2;i<8;i++) temp+=buf[i];
	tpad_default_val = temp/6;
	printf("tpad_default_val: %d\r\n",tpad_default_val);
	
	//返回初始化结果
	if(tpad_default_val>TPAD_ARR_MAX_VAL/2) return 1;	//初始化遇到超过TPAD_ARR_MAX_VAL/2的数值，不正常！
	return 0;
}

//复位：释放电容，清空计数器
void TPAD_Reset(void)
{
	GPIO_Set(GPIOA, PIN5, GPIO_MODE_OUT, GPIO_OTYPE_PP, GPIO_SPEED_100M, GPIO_PUPD_PD);		//PA5推挽输出，下拉
	GPIO_Pin_Set(GPIOA, PIN5, 0);		//PA5输出0，电容放电
	delay_ms(5);
	TIM2->SR = 0;		//清除标记
	TIM2->CNT = 0;		//计数器清零
	GPIO_Set(GPIOA, PIN5, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_SPEED_100M, GPIO_PUPD_NONE);	//PA5复用功能，不带上下拉	
}

//获取定时器捕获值，返回捕获值/计数值（超时）
u16 TPAD_Get_Val(void)
{
	TPAD_Reset();
	//等待捕获上升沿 [1]-CC1IF: Capture/Compare 1 interrupt flag 
	//0: No input capture occurred  
	//1: The counter value has been captured in TIMx_CCR1 register
	while((TIM2->SR&0x02)==0)	
	{
		if(TIM2->CNT>TPAD_ARR_MAX_VAL-500) return TIM2->CNT;		//超时，返回CNT
	}
	return TIM2->CCR1;
}

//读取n次定时器捕获值，并取最大值返回
u16 TPAD_Get_MaxVal(u8 n)
{
	u16 temp=0;
	u16 res=0;				//result，用于存放最大捕获值
	u8 lcntnum=n*2/3;		//至少2/3*n的有效触摸才算有效
	u8 okcnt=0;
	
	while(n--)
	{
		temp=TPAD_Get_Val();
		if(temp>(tpad_default_val*5/4)) okcnt++;	//捕获值至少要大于默认值的5/4才算有效
		if(temp>res)res=temp;
	}
	if(okcnt>=lcntnum) return res;		//有效触摸次数至少要超过2/3才返回最大值
	else return 0;		//否则认为出错，返回0
}

//扫描触摸按键
//mode： 0-不连续触发  1-连续触发
//返回值： 0-没有按下  1-按下
u8 TPAD_Scan(u8 mode)
{
	//static修饰局部变量,其作用域未改变仍与局部变量相同，但其生命周期改变，程序结束时才销毁
	static u8 keyen=0;		//0-可以开始检测  大于0-还不能开始检测
	u8 res=0;	
	u8 sample=3;		//默认采样次数为3
	u16 rval;			//存放最大捕获值
	
	if(mode)	//连续触发模式
	{
		sample=6;		//设置采样次数为6次
		keyen=0;		//支持连按
	}
	
	rval=TPAD_Get_MaxVal(sample);		//获取最大捕获值
	//大于tpad_default_val+(1/3)*大于tpad_default_val,且小于10倍tpad_default_val,则有效
	if(rval>(tpad_default_val*4/3) && rval<(10*tpad_default_val))
	{
		if(keyen==0) res=1;  //keyen==0可以开始检测，返回1，按键按下
		keyen=3;		//重置keyen值，下次依然需要再过3次之后按键才有效
	}
	
	if(keyen) keyen--;
	return res;
}

//定时器2通道1输入捕获配置
//arr-重载值  psc-预分频系数
void TIM2_CH1_Cap_Init(u32 arr, u16 psc)
{
	RCC->APB1ENR|=1<<0;		//TIM2时钟使能    
	RCC->AHB1ENR|=1<<0;   	//使能PORTA时钟	
	GPIO_Set(GPIOA,PIN5,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_NONE);//PA5,复用功能,不带上下拉
	GPIO_AF_Set(GPIOA,5,1);	//PA5,AF1 
	  
 	TIM2->ARR=arr;  		//设定计数器自动重装值//刚好1ms    
	TIM2->PSC=psc;  		//预分频器,1M的计数频率	 

	TIM2->CCMR1|=1<<0;		//CC1S=01 	选择输入端 IC1映射到TI1上
 	TIM2->CCMR1|=0<<4; 		//IC1F=0000 配置输入滤波器 不滤波 
 
	TIM2->EGR=1<<0;			//软件控制产生更新事件,使写入PSC的值立即生效,否则将会要等到定时器溢出才会生效!
	TIM2->CCER|=0<<1; 		//CC1P=0	上升沿捕获
	TIM2->CCER|=1<<0; 		//CC1E=1 	允许捕获计数器的值到捕获寄存器中
																 
	TIM2->CR1|=0x01;    	//使能定时器2
}
