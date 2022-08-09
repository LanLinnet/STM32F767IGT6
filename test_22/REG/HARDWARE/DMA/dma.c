#include "dma.h"

//DMAx的各通道配置
//DMA_Streamx：DMA数据流 0-7
//chx-DMA通道选择  par-外设地址  mar-存储器地址  ndtr-数据传输量
void MYDMA_Config(DMA_Stream_TypeDef *DMA_Streamx,u8 chx,u32 par,u32 mar,u16 ndtr)
{
	DMA_TypeDef *DMAx;
	u8 streamx;
	if((u32)DMA_Streamx>(u32)DMA2)		//判断是DMA1还是DMA2
	{
		DMAx=DMA2;
		RCC->AHB1ENR|=1<<22;
	}else
	{
		DMAx=DMA1;
		RCC->AHB1ENR|=1<<21;
	}
	
	while(DMA_Streamx->CR&0x01);		//等待DMA可配置
	streamx=(((u32)DMA_Streamx-(u32)DMAx)-0x10)/0x18;		//得到stream通道号（通过地址计算）
	
	//清空之前所有的中断标志
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

//开启一次DMA传输
void MYDMA_Enable(DMA_Stream_TypeDef *DMA_Streamx, u16 ndtr)
{
	DMA_Streamx->CR&=~(1<<0);		//关闭DMA传输
	while(DMA_Streamx->CR&0x1);
	DMA_Streamx->NDTR=ndtr;
	DMA_Streamx->CR|=1<<0;
}
