#include "sdram.h"
#include "delay.h"

//��SDRAM��������
//bankx-BANK5/BANK6  cmd-ָ��  refresh-��ˢ�´���  regval-ģʽ�Ĵ����Ķ���
//����ֵ��0-����  1-ʧ��
u8 SDRAM_Send_Cmd(u8 bankx, u8 cmd, u8 refresh, u16 regval)
{
	u32 retry=0;		//ָ����ظ����Դ���
	u32 tempreg=0;		//���������ʱ����
	tempreg|=cmd<<0;		//����ָ��
	tempreg|=1<<(4-bankx);	//���÷��͵�BANK5/BANK6
	tempreg|=refresh<<5;	//������ˢ�´���
	tempreg|=regval<<9;		//����SDRAMģʽ�Ĵ���������
	FMC_Bank5_6->SDCMR=tempreg;
	
	while((FMC_Bank5_6->SDSR&(1<<5)))		//�ȴ�ָ������
	{
		retry++;
		if(retry>0x1fffff)
			return 1;
	}
	return 0;
}

//SDRAM��ʼ��
void SDRAM_Init(void)
{
	u32 sdctrlreg=0;		//FMC_SDCRx ���ƼĴ��� ��ʱ����
	u32 sdtimereg=0;		//FMC_SDTRx ʱ��Ĵ��� ��ʱ����
	u16 mregval=0;			//SDRAM��ģʽ�Ĵ��� ��ʱ����
	
	RCC->AHB3ENR|=1<<0;		//ʹ��FMCʱ��
	RCC->AHB1ENR|=0x1f<<2;		//ʹ��GPIOC/D/E/F/Gʱ��
	
	GPIO_Set(GPIOC,PIN0|PIN2|PIN3,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);			//PC0/2/3			
	GPIO_Set(GPIOD,3<<0|7<<8|3<<14,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);		//PD0/1/8/9/10/14/15		
	GPIO_Set(GPIOE,3<<0|0X1FF<<7,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);			//PE0/1/7~15				
	GPIO_Set(GPIOF,0X3F<<0|0X1F<<11,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);		//PG0~5/11~15					
	GPIO_Set(GPIOG,7<<0|3<<4|PIN8|PIN15,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);	//PF0~2/4/5/8/15
	
	GPIO_AF_Set(GPIOC,0,12);	//PC0,AF12
 	GPIO_AF_Set(GPIOC,2,12);	//PC2,AF12
 	GPIO_AF_Set(GPIOC,3,12);	//PC3,AF12
	
 	GPIO_AF_Set(GPIOD,0,12);	//PD0,AF12 
 	GPIO_AF_Set(GPIOD,1,12);	//PD1,AF12 
 	GPIO_AF_Set(GPIOD,8,12);	//PD8,AF12
 	GPIO_AF_Set(GPIOD,9,12);	//PD9,AF12
 	GPIO_AF_Set(GPIOD,10,12);	//PD10,AF12  
 	GPIO_AF_Set(GPIOD,14,12);	//PD14,AF12
 	GPIO_AF_Set(GPIOD,15,12);	//PD15,AF12
	
 	GPIO_AF_Set(GPIOE,0,12);	//PE0,AF12 
 	GPIO_AF_Set(GPIOE,1,12);	//PE1,AF12 
 	GPIO_AF_Set(GPIOE,7,12);	//PE7,AF12
 	GPIO_AF_Set(GPIOE,8,12);	//PE8,AF12
 	GPIO_AF_Set(GPIOE,9,12);	//PE9,AF12
 	GPIO_AF_Set(GPIOE,10,12);	//PE10,AF12
 	GPIO_AF_Set(GPIOE,11,12);	//PE11,AF12
 	GPIO_AF_Set(GPIOE,12,12);	//PE12,AF12
 	GPIO_AF_Set(GPIOE,13,12);	//PE13,AF12
 	GPIO_AF_Set(GPIOE,14,12);	//PE14,AF12
 	GPIO_AF_Set(GPIOE,15,12);	//PE15,AF12

 	GPIO_AF_Set(GPIOF,0,12);	//PF0,AF12 
 	GPIO_AF_Set(GPIOF,1,12);	//PF1,AF12 
 	GPIO_AF_Set(GPIOF,2,12);	//PF2,AF12
 	GPIO_AF_Set(GPIOF,3,12);	//PF3,AF12
 	GPIO_AF_Set(GPIOF,4,12);	//PF4,AF12
 	GPIO_AF_Set(GPIOF,5,12);	//PF5,AF12
 	GPIO_AF_Set(GPIOF,11,12);	//PF11,AF12
 	GPIO_AF_Set(GPIOF,12,12);	//PF12,AF12
 	GPIO_AF_Set(GPIOF,13,12);	//PF13,AF12
 	GPIO_AF_Set(GPIOF,14,12);	//PF14,AF12
 	GPIO_AF_Set(GPIOF,15,12);	//PF15,AF12
	
 	GPIO_AF_Set(GPIOG,0,12);	//PG0,AF12 
 	GPIO_AF_Set(GPIOG,1,12);	//PG1,AF12 
 	GPIO_AF_Set(GPIOG,2,12);	//PG2,AF12
 	GPIO_AF_Set(GPIOG,4,12);	//PG4,AF12
 	GPIO_AF_Set(GPIOG,5,12);	//PG5,AF12  
 	GPIO_AF_Set(GPIOG,8,12);	//PG8,AF12
 	GPIO_AF_Set(GPIOG,15,12);	//PG15,AF12	
	
 	sdctrlreg|=1<<0;				//9λ�е�ַ
	sdctrlreg|=2<<2;				//13λ�е�ַ
	sdctrlreg|=1<<4;				//16λ����λ��
	sdctrlreg|=1<<6;				//4���ڲ�����(4 BANKS)
	sdctrlreg|=3<<7;				//3��CAS�ӳ�
	sdctrlreg|=0<<9;				//����д����
	sdctrlreg|=2<<10;				//SDRAMʱ��=HCLK/2=216M/2=108M=9.3ns
	sdctrlreg|=1<<12;				//ʹ��ͻ������ 
	sdctrlreg|=0<<13;				//��ͨ���ӳ�0��HCLK
 	FMC_Bank5_6->SDCR[0]=sdctrlreg;	//����FMC BANK5 SDRAM���ƼĴ���(BANK5��6���ڹ���SDRAM).
	
	sdtimereg|=1<<0;				//����ģʽ�Ĵ���������ʱ����ӳ�Ϊ2��ʱ������
	sdtimereg|=7<<4;				//�˳���ˢ���ӳ�Ϊ8��ʱ������
	sdtimereg|=6<<8;				//��ˢ��ʱ��Ϊ7��ʱ������
	sdtimereg|=6<<12;				//��ѭ���ӳ�Ϊ7��ʱ������
	sdtimereg|=1<<16;				//�ָ��ӳ�Ϊ2��ʱ������
	sdtimereg|=1<<20;				//��Ԥ����ӳ�Ϊ2��ʱ������
	sdtimereg|=1<<24;				//�е����ӳ�Ϊ2��ʱ������
 	FMC_Bank5_6->SDTR[0]=sdtimereg;	//����FMC BANK5 SDRAMʱ��Ĵ��� 
	
	SDRAM_Send_Cmd(0,1,0,0);		//ʱ������ʹ��
	delay_us(500);					//�����ӳ�200us.
	SDRAM_Send_Cmd(0,2,0,0);		//�����д洢��Ԥ���
	SDRAM_Send_Cmd(0,3,8,0);		//������ˢ�´��� 
	mregval|=3<<0;					//����ͻ������:8(������1/2/4/8)
	mregval|=0<<3;					//����ͻ������:����(����������/����)
	mregval|=3<<4;					//����CASֵ:3(������2/3)
	mregval|=0<<7;					//���ò���ģʽ:0,��׼ģʽ
	mregval|=1<<9;					//����ͻ��дģʽ:1,�������
	SDRAM_Send_Cmd(0,4,0,mregval);	//����SDRAM��ģʽ�Ĵ���
	
	//����ʹ�õ�SDRAMˢ������Ϊ64ms,SDCLK=216/2=108Mhz,����Ϊ8192(2^13).
	
	//ˢ��Ƶ�ʼ�����COUNTֵ���㷽��:
	//ˢ�����ʣ�us�� = ˢ������/���� = ��ʵ��COUNT+1��*SDRAMƵ��ʱ��
	//���SDRAM�ڽ�������󣬳����ڲ�ˢ����������뽫ˢ��Ƶ������20��SDRAMʱ�����ڣ��Ի�ȡ���������
	//���� ��Ƶ�COUNT=�������ʵ��COUNT-20
	
	//ˢ������=64ms/8192=7.81us
	//SDRAMʱ��Ƶ��=216/2=108Mhz
	//ʵ��COUNT=ˢ������*SDRAMʱ��Ƶ��+1=7.81us*108Mhz+1=844.48��844
	//���Count=844-20=824

	FMC_Bank5_6->SDRTR=824<<1;		//����ˢ��Ƶ�ʼ�����
}

//��ָ����ַ(WriteAddr+Bank5_SDRAM_ADDR)��ʼд��n���ֽ�
void FMC_SDRAM_WriteBuffer(u8 *pBuffer, u32 WriteAddr, u32 n)
{
	for(;n!=0;n--)
	{
		*(vu8*)(Bank5_SDRAM_ADDR+WriteAddr) = *pBuffer;
		WriteAddr++;
		pBuffer++;
	}
}

//��ָ����ַ(WriteAddr+Bank5_SDRAM_ADDR)��ʼ����n���ֽ�
void FMC_SDRAM_ReadBuffer(u8 *pBuffer, u32 ReadAddr, u32 n)
{
	for(;n!=0;n--)
	{
		*pBuffer++ = *(vu8*)(Bank5_SDRAM_ADDR+ReadAddr);
		ReadAddr++;
	}
}

//���ʹ��RGB��ʱ������
//����MCU��,��������CAS=3,����SDRAM�����쳣
//����RGB��,��������CAS=2,�����SDRAM�����ٶ�
void SDRAM_RGBLCD_Set(void)
{
	u32 mregval = 0;
	FMC_Bank5_6->SDCR[0]&=~(3<<7);		//���ԭ����CAS�ӳ�����
	FMC_Bank5_6->SDCR[0]&= 2<<7;		//����CAS�ӳ�Ϊ2
	
	mregval|=3<<0;					//����ͻ������:8(������1/2/4/8)
	mregval|=0<<3;					//����ͻ������:����(����������/����)
	mregval|=2<<4;					//����CASֵ:2(������2/3)
	mregval|=0<<7;					//���ò���ģʽ:0,��׼ģʽ
	mregval|=1<<9;					//����ͻ��дģʽ:1,�������
	SDRAM_Send_Cmd(0,4,0,mregval);	//����SDRAM��ģʽ�Ĵ���
}
