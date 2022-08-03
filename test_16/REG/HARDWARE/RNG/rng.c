#include "rng.h"
#include "delay.h"
//初始化RNG
//返回值：0-成功  1-失败
u8 RNG_Init(void)
{
	u16 retry=0;
	RCC->AHB2ENR=1<<6;		//Bit 6 RNGEN: Random number generator clock enable
	RNG->CR|=1<<2;			//使能RNG
	while((RNG->SR&0x01)==0 && retry<10000) //等待随机数有效
	{
		retry++;
		delay_us(100);
	}
	if(retry>=10000) return 1;
	return 0;
}

//得到随机数并返回
u32 RNG_Get_RandomNum(void)
{
	while((RNG->SR&0x01)==0);	//等待随机数有效
	return RNG->DR;
}

//得到某个范围内的随机数
int RNG_Get_RandomRange(int min, int max)
{
	return RNG_Get_RandomNum()%(max-min+1)+min;
}
