#ifndef __ADC_H
#define __ADC_H

#include "sys.h"

#define ADC_CH5 5		//ͨ��5
#define ADC_CH_TEMP 18

void Adc_Init(void);	//ADC��ʼ��
u16 Get_Adc(u8 ch);		//���ĳ��ͨ��ֵ
u16 Get_Adc_Average(u8 ch, u8 times);	//�õ�ĳ��ͨ����������������ƽ��ֵ
short Get_Temperature(void);

#endif