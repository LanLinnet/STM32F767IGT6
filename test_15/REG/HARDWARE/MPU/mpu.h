#ifndef __MPU_H
#define __MPU_H

#include "sys.h"

//����MPU->RASR�Ĵ���AP[26:24]λ������ֵ�����ڿ������ݵķ���Ȩ��
#define MPU_REGION_NO_ACCESS 	0x00	//�޷��ʣ���Ȩ&�û������ɷ��ʣ�
#define MPU_REGION_PRIV_RW   	0x01	//��֧����Ȩ��д����
#define MPU_REGION_PRIV_RW_URO  0x02	//��ֹ�û�д���ʣ���Ȩ�ɶ�д��
#define MPU_REGION_FULL_ACCESS	0x03	//ȫ���ʣ���Ȩ&�û����ɷ��ʣ�
#define MPU_REGION_PRIV_RO		0x05	//��֧����Ȩ������
#define MPU_REGION_PRIV_RO_URO	0x06	//ֻ������Ȩ&�û���������д��

void MPU_Disable(void);
void MPU_Enable(void);
u8 MPU_Convert_Bytes_To_POT(u32 nbytes);
u8 MPU_Set_Protection(u32 baseaddr, u32 size, u32 rnum, u8 ap, u8 sen, u8 cen, u8 ben);
void MPU_Memory_Protection(void);

#endif
