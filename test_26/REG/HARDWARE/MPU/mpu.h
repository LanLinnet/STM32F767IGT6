#ifndef __MPU_H
#define __MPU_H

#include "sys.h"

//定义MPU->RASR寄存器AP[26:24]位的设置值，用于控制数据的访问权限
#define MPU_REGION_NO_ACCESS 	0x00	//无访问（特权&用户都不可访问）
#define MPU_REGION_PRIV_RW   	0x01	//仅支持特权读写访问
#define MPU_REGION_PRIV_RW_URO  0x02	//禁止用户写访问（特权可读写）
#define MPU_REGION_FULL_ACCESS	0x03	//全访问（特权&用户都可访问）
#define MPU_REGION_PRIV_RO		0x05	//仅支持特权读访问
#define MPU_REGION_PRIV_RO_URO	0x06	//只读（特权&用户都不可以写）

void MPU_Disable(void);
void MPU_Enable(void);
u8 MPU_Convert_Bytes_To_POT(u32 nbytes);
u8 MPU_Set_Protection(u32 baseaddr, u32 size, u32 rnum, u8 ap, u8 sen, u8 cen, u8 ben);
void MPU_Memory_Protection(void);

#endif
