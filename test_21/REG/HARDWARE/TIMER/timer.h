#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"
//ͨ���ı�TIM3->CCR4��ֵ���ı�ռ�ձȣ��Ӷ�����LED0������
#define LED0_PWM_VAL TIM3->CCR4
#define PWM_DAC_VAL TIM9->CCR2
void TIM3_Int_Init(u16 arr, u16 psc);
void TIM3_PWM_Init(u32 arr, u32 psc);
void TIM9_CH2_PWM_Init(u16 arr, u16 psc);
#endif