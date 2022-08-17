#include "qspi.h"

//�ȴ�״̬��־
//����ֵ��0-�ɹ�  1-ʧ��
u8 QSPI_Wait_Flag(u32 flag, u8 sta, u32 wtime)
{
	u8 flagsta=0;
	while(wtime)
	{
		flagsta=(QUADSPI->SR&flag)?1:0;
		if(flagsta==sta) break;
		wtime--;
	}
	if(wtime) return 0;
	else return 1;
}

//��ʼ��QSPI
//����ֵ��0-�ɹ�  1-ʧ��
u8 QSPI_Init(void)
{
	u32 tempreg=0;
	RCC->AHB1ENR|=1<<1;
	RCC->AHB1ENR|=1<<5;
	RCC->AHB3ENR|=1<<1;
	
	GPIO_Set(GPIOB,1<<2,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);	//PB2���ù������	
	GPIO_Set(GPIOB,1<<6,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);	//PB6���ù������	
	GPIO_Set(GPIOF,0XF<<6,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);	//PF6~9���ù������	
	
	GPIO_AF_Set(GPIOB,2,9);		//PB2,AF9
 	GPIO_AF_Set(GPIOB,6,10);	//PB6,AF10
 	GPIO_AF_Set(GPIOF,6,9);		//PF6,AF9 
 	GPIO_AF_Set(GPIOF,7,9);		//PF7,AF9
 	GPIO_AF_Set(GPIOF,8,10);	//PF8,AF10
 	GPIO_AF_Set(GPIOF,9,10);	//PF9,AF10
	
	RCC->AHB3RSTR|=1<<1;
	RCC->AHB3RSTR&=~(1<<1);
	if(QSPI_Wait_Flag(1<<5,0,0xffff)==0)	//�ȴ�BUSY����
	{
		//QUADSPI_CR
		tempreg=(3-1)<<24;	//����Ƶ
		tempreg|=(4-1)<<8;	//FIFO��ֵ�ĸ��ֽ�
		tempreg|=0<<7;
		tempreg|=0<<6;
		tempreg|=1<<4;
		QUADSPI->CR=tempreg;
		//QUADSPI_DCR
		tempreg=(25-1)<<16;
		tempreg|=(4-1)<<8;
		tempreg|=1<<0;
		QUADSPI->DCR=tempreg;
		QUADSPI->CR|=1<<0;
	}else return 1;
	return 0;
}

//QSPI��������
//cmd:Ҫ���͵�ָ��
//addr:���͵���Ŀ�ĵ�ַ
//mode:ģʽ,��ϸλ��������:
//	mode[1:0]:ָ��ģʽ;00,��ָ��;01,���ߴ���ָ��;10,˫�ߴ���ָ��;11,���ߴ���ָ��.
//	mode[3:2]:��ַģʽ;00,�޵�ַ;01,���ߴ����ַ;10,˫�ߴ����ַ;11,���ߴ����ַ.
//	mode[5:4]:��ַ����;00,8λ��ַ;01,16λ��ַ;10,24λ��ַ;11,32λ��ַ.
//	mode[7:6]:����ģʽ;00,������;01,���ߴ�������;10,˫�ߴ�������;11,���ߴ�������.
//dmcycle:��ָ��������
void QSPI_Send_CMD(u8 cmd, u32 addr, u8 mode, u8 dmcycle)
{
	u32 tempreg=0;
	u8 status;
	if(QSPI_Wait_Flag(1<<5,0,0xffff)==0)
	{
		//QUADSPI_CCR
		tempreg=0<<31;
		tempreg|=0<<28;
		tempreg|=0<<26;
		tempreg|=((u32)mode>>6)<<24;
		tempreg|=(u32)dmcycle<<18;
		tempreg|=((u32)(mode>>4)&0x03)<<12;
		tempreg|=((u32)(mode>>2)&0x03)<<10;
		tempreg|=((u32)(mode>>0)&0x03)<<8;
		tempreg|=cmd;
		QUADSPI->CCR=tempreg;
		
		if(mode&0x0c)		//��ָ��/��ַҪ����
		{
			QUADSPI->AR=addr;
		}
		if((mode&0xc0)==0)	//�����ݴ���
		{
			status=QSPI_Wait_Flag(1<<1,1,0xffff);		//TCFλ���Ƿ������
			if(status==0)
			{
				QUADSPI->FCR|=1<<1;		//�����־λ
			}
		}
	}
}

//QSPI����ָ�����ȵ�����
//buf:�������ݻ������׵�ַ
//datalen:Ҫ��������ݳ���
//����ֵ:0,����  ����,�������
u8 QSPI_Receive(u8 *buf, u32 datalen)
{
	u32 tempreg=QUADSPI->CCR;
	u32 addreg = QUADSPI->AR;
	u8 status;
	vu32 *data_reg = &QUADSPI->DR;
	QUADSPI->DLR=datalen-1;
	//QUADSPI_CCR
	tempreg&=~(3<<26);
	tempreg|=1<<26;
	QUADSPI->FCR|=1<<1;
	QUADSPI->CCR=tempreg;
	QUADSPI->AR=addreg;
	while(datalen)
	{
		status=QSPI_Wait_Flag(3<<1,1,0xffff);		//���յ�������
		if(status==0)		//�ȴ��ɹ�
		{
			*buf++=*(vu8 *)data_reg;
			datalen--;
		}else break;
	}
	if(status==0)
	{
		QUADSPI->CR|=1<<2;		
		status=QSPI_Wait_Flag(1<<1,1,0xffff);
		if(status==0)		//�����Ѿ����
		{
			QUADSPI->FCR|=1<<1;
			status=QSPI_Wait_Flag(1<<5,0,0xffff);
		}
	}
	return status;
}

//QSPI����ָ�����ȵ�����
//buf:�������ݻ������׵�ַ
//datalen:Ҫ��������ݳ���
//����ֵ:0,����
//    ����,�������
u8 QSPI_Transmit(u8 *buf, u32 datalen)
{
	u32 tempreg=QUADSPI->CCR;
	u32 addrreg=QUADSPI->AR; 
	u8 status;
	vu32 *data_reg=&QUADSPI->DR;
	QUADSPI->DLR=datalen-1;
	tempreg&=~(3<<26);
	tempreg|=0<<26;
	QUADSPI->FCR|=1<<1;
	QUADSPI->CCR=tempreg;
	while(datalen)
	{
		status=QSPI_Wait_Flag(1<<2,1,0xffff);
		if(status!=0) break;
		*(vu8 *)data_reg=*buf++;
		datalen--;
	}
	if(status==0)
	{
		QUADSPI->CR|=1<<2;							//��ֹ���� 
		status=QSPI_Wait_Flag(1<<1,1,0XFFFF);		//�ȴ�TCF,�����ݴ������
		if(status==0)
		{
			QUADSPI->FCR|=1<<1;						//���TCF��־λ 
			status=QSPI_Wait_Flag(1<<5,0,0XFFFF);	//�ȴ�BUSYλ����
		}
	}
	return status;
}

