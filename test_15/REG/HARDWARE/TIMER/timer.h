#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"
//ͨ���ı�TIM3->CCR4��ֵ���ı�ռ�ձȣ��Ӷ�����LED0������
#define LED0_PWM_VAL TIM3->CCR4
void TIM3_Int_Init(u16 arr, u16 psc);
void TIM3_PWM_Init(u32 arr, u32 psc);
void TIM5_CH1_Cap_Init(u32 arr, u16 psc);
#endif
