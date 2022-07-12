#include "wdg.h"

//初始化独立看门狗
//prer：分频数  rlr：重装载寄存器值
void IWDG_Init(u8 prer, u16 rlr)
{
	IWDG->KR = 0x5555;		//使能对IWDG->PR和IWDG->RLR的写
	IWDG->PR = prer;		//设置分频系数
	IWDG->RLR = rlr;		//设置重装载寄存器值
	IWDG->KR=0xaaaa;		//重装载
	IWDG->KR=0xcccc;		//使能看门狗
}

//喂独立看门狗
void IWDG_Feed(void)
{
	IWDG->KR=0xaaaa;		//重装载
}
