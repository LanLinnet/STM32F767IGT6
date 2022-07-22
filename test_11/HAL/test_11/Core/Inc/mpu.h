#ifndef __MPU_H
#define __MPU_H
#include "sys.h"

u8 MPU_Set_Protection(u32 baseaddr,u32 size,u32 rnum,u32 ap);
void MPU_Memory_Protection(void);
#endif
