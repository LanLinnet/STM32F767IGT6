#ifndef __AP3216C_H
#define __AP3216C_H

#include "sys.h"

#define AP3216C_ADDR 0x3C	//AP3216C����IIC��ַ��+д������

u8 AP3216C_Init(void);
u8 AP3216C_WriteOneByte(u8 reg, u8 data);
u8 AP3216C_ReadOneByte(u8 reg);
void AP3216C_ReadData(u16 *ir, u16 *ps, u16 *als);

#endif
