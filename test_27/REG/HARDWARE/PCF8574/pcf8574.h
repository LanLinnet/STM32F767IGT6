#ifndef __PCF8574_H
#define __PCF8574_H

#include "sys.h"
#include "myiic.h"

#define PCF8574_INT GPIO_Pin_Get(GPIOB,PIN12)
#define PCF8574_ADDR 0x40

//PCF8574各个IO的功能
#define BEEP_IO 0
#define AP_INT_IO 1
#define DCMI_PWDN_IO 2
#define USB_PWR_IO 3
#define EX_IO 4
#define MPU_INT_IO 5
#define RS485_RE_IO 6
#define ETH_RESET_IO 7

u8 PCF8574_Init(void);
u8 PCF8574_ReadOneByte(void);
void PCF8574_WriteOneByte(u8 DataToWrite);
void PCF8574_WriteBit(u8 bit,u8 sta);
u8 PCF8574_ReadBit(u8 bit);

#endif
