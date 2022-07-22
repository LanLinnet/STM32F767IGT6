#ifndef __KEY_H
#define __KEY_H

#include "sys.h"

#define KEY0 GPIO_Pin_Get(GPIOH, PIN3)
#define KEY1 GPIO_Pin_Get(GPIOH, PIN2)
#define KEY2 GPIO_Pin_Get(GPIOC, PIN13)
#define WK_UP GPIO_Pin_Get(GPIOA, PIN0)

#define KEY0_PRES 1
#define KEY1_PRES 2
#define KEY2_PRES 3
#define WKUP_PRES 4

void KEY_Init(void);	
u8 KEY_Scan(u8);	

#endif
