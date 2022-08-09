#include "dma.h"

//DMAx�ĸ�ͨ������
//DMA_Streamx��DMA������ 0-7
//chx-DMAͨ��ѡ��  par-�����ַ  mar-�洢����ַ  ndtr-���ݴ�����
void MYDMA_Config(DMA_Stream_TypeDef *DMA_Streamx,u8 chx,u32 par,u32 mar,u16 ndtr)
{
	DMA_TypeDef *DMAx;
	u8 streamx;
	if((u32)DMA_Streamx>(u32)DMA2)		//�ж���DMA1����DMA2
	{
		DMAx=DMA2;
		RCC->AHB1ENR|=1<<22;
	}else
	{
		DMAx=DMA1;
		RCC->AHB1ENR|=1<<21;
	}
	
	while(DMA_Streamx->CR&0x01);		//�ȴ�DMA������
	streamx=(((u32)DMA_Streamx-(u32)DMAx)-0x10)/0x18;		//�õ�streamͨ���ţ�ͨ����ַ���㣩
	
	//���֮ǰ���е��жϱ�־
	if(streamx>=6)DMAx->HIFCR|=0x3d<<(6*(streamx-6)+16);	
	else if(streamx>=4)DMAx->HIFCR|=0x3d<<6*(streamx-4);
	else if(streamx>=2)DMAx->LIFCR|=0x3d<<(6*(streamx-2)+16);
	else DMAx->LIFCR|=0x3d<<6*streamx;
	
	DMA_Streamx->PAR=par;
	DMA_Streamx->M0AR=mar;
	DMA_Streamx->NDTR=ndtr;
	DMA_Streamx->CR=0;
	DMA_Streamx->CR|=1<<6;
	DMA_Streamx->CR|=0<<8;
	DMA_Streamx->CR|=0<<9;
	DMA_Streamx->CR|=1<<10;
	DMA_Streamx->CR|=0<<11;
	DMA_Streamx->CR|=0<<13;
	DMA_Streamx->CR|=1<<16;
	DMA_Streamx->CR|=0<<21;
	DMA_Streamx->CR|=0<<23;
	DMA_Streamx->CR|=(u32)chx<<25;
}

//����һ��DMA����
void MYDMA_Enable(DMA_Stream_TypeDef *DMA_Streamx, u16 ndtr)
{
	DMA_Streamx->CR&=~(1<<0);		//�ر�DMA����
	while(DMA_Streamx->CR&0x1);
	DMA_Streamx->NDTR=ndtr;
	DMA_Streamx->CR|=1<<0;
}
