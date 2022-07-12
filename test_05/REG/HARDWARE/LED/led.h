#ifndef __LED_H
#define __LED_H
#include "sys.h"

#define LED0(x)	GPIO_Pin_Set(GPIOB, PIN1, x)
#define LED1(x)	GPIO_Pin_Set(GPIOB, PIN0, x)
void LED_Init(void);

#endif
