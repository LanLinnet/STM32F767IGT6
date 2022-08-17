#ifndef __QSPI_H
#define __QSPI_H

#include "sys.h"

u8 QSPI_Wait_Flag(u32 flag,u8 sta,u32 wtime);
u8 QSPI_Init(void);
void QSPI_Send_CMD(u8 cmd,u32 addr,u8 mode,u8 dmcycle);
u8 QSPI_Receive(u8 *buf, u32 datalen);
u8 QSPI_Transmit(u8 *buf, u32 datalen);

#endif
