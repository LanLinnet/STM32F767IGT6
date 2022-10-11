#include "can.h"

//CAN初始化
//返回值：0-成功  其它-失败
u8 CAN1_Mode_Init(u8 tsjw, u8 tbs2, u8 tbs1, u16 brp, u8 mode)
{
	u16 i=0;
	if(tsjw==0||tbs2==0||tbs1==0||brp==0) return 1;
	tsjw-=1;
	tbs1-=1;
	tbs2-=1;
	brp-=1;
	
	RCC->AHB1ENR|=1<<0;
	GPIO_Set(GPIOA, PIN11|PIN12, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_SPEED_50M, GPIO_PUPD_PU);
	GPIO_AF_Set(GPIOA,11,9);
	GPIO_AF_Set(GPIOA,12,9);
	
	RCC->APB1ENR|=1<<25;
	CAN1->MCR=0x0000;
	CAN1->MCR|=1<<0;
	while((CAN1->MSR&1<<0)==0)
	{
		i++;
		if(i>100) return 2;		//进入初始化模式失败
	}
	CAN1->MCR|=0<<7;
	CAN1->MCR|=0<<6;
	CAN1->MCR|=0<<5;
	CAN1->MCR|=1<<4;
	CAN1->MCR|=0<<3;
	CAN1->MCR|=0<<2;
	
	CAN1->BTR=0x00000000;
	CAN1->BTR|=mode<<30;
	CAN1->BTR|=tsjw<<24;
	CAN1->BTR|=tbs2<<20;
	CAN1->BTR|=tbs1<<16;
	CAN1->BTR|=brp<<0;
	
	CAN1->MCR&=~(1<<0);
	while((CAN1->MSR&1<<0)==1)
	{
		i++;
		if(i>0xfff0) return 3;		//退出初始化模式失败
	}
	
	//过滤器初始化
	CAN1->FMR|=1<<0;		//过滤器组工作在初始化模式
	CAN1->FA1R&=~(1<<0);	//过滤器0不激活
	CAN1->FS1R|=1<<0; 		//过滤器位宽为32位.
	CAN1->FM1R|=0<<0;		//过滤器0工作在标识符屏蔽位模式
	CAN1->FFA1R|=0<<0;		//过滤器0关联到FIFO0
	CAN1->sFilterRegister[0].FR1=0X00000000;//32位ID
	CAN1->sFilterRegister[0].FR2=0X00000000;//32位MASK
	CAN1->FA1R|=1<<0;		//激活过滤器0
	CAN1->FMR&=0<<0;		//过滤器组进入正常模式

#if CAN1_RX0_INT_ENABLE
 	//使用中断接收
	CAN1->IER|=1<<1;		//FIFO0消息挂号中断允许.	    
	MY_NVIC_Init(1,0,CAN1_RX0_IRQn,2);//组2
#endif
	return 0;
}

//CAN发送数据
//返回值0-3 邮箱编号
u8 CAN1_Tx_Msg(u32 id, u8 ide, u8 rtr, u8 len, u8 *dat)
{
	u8 mbox;
	if(CAN1->TSR&(1<<26))mbox = 0;		//Transmit mailbox 0 empty
	else if(CAN1->TSR&(1<<27))mbox = 1;
	else if(CAN1->TSR&(1<<28))mbox = 2;
	else return 0xff;
	CAN1->sTxMailBox[mbox].TIR = 0;
	
	if(ide==0)		//标准帧
	{
		id&=0x7ff;
		id<<=21;
	}else{
		id&=0x1fffffff;
		id<<=3;
	}
	CAN1->sTxMailBox[mbox].TIR|=id;
	CAN1->sTxMailBox[mbox].TIR|=ide<<2;
	CAN1->sTxMailBox[mbox].TIR|=rtr<<1;
	len&=0x0f;
	CAN1->sTxMailBox[mbox].TDTR&=~(0x0000000f);
	CAN1->sTxMailBox[mbox].TDTR|=len;
	//将待发数据存入邮箱
	CAN1->sTxMailBox[mbox].TDHR=(((u32)dat[7]<<24)|((u32)dat[6]<<16)|((u32)dat[5]<<8)|((u32)dat[4]));
	CAN1->sTxMailBox[mbox].TDLR=(((u32)dat[3]<<24)|((u32)dat[2]<<16)|((u32)dat[1]<<8)|((u32)dat[0]));
	CAN1->sTxMailBox[mbox].TIR|=1<<0;
	return mbox;
}

//获得发送状态
//返回值：0-挂起；0x05-发送失败;0x07-发送成功
u8 CAN1_Tx_Status(u8 mbox)
{
	u8 sta=0;
	switch(mbox)
	{
		case 0:
			sta|=CAN1->TSR&(1<<0);
			sta|=CAN1->TSR&(1<<1);
			sta|=((CAN1->TSR&(1<<26))>>24);
		case 1:
			sta|=CAN1->TSR&(1<<8)>>8;
			sta|=CAN1->TSR&(1<<9)>>8;
			sta|=((CAN1->TSR&(1<<27))>>25);
		case 2:
			sta|=CAN1->TSR&(1<<16)>>16;
			sta|=CAN1->TSR&(1<<17)>>16;
			sta|=((CAN1->TSR&(1<<28))>>26);
		default:
			sta = 0x05;
		break;
	}
	return sta;
}

//得到在FIFO中接收到的报文个数
u8 CAN1_Msg_Pend(u8 fifox)
{
	if(fifox==0)return CAN1->RF0R&0x03;
	else if(fifox==1) return CAN1->RF1R&0x03;
	else return 0;
}

//接收数据
void CAN1_Rx_Msg(u8 fifox, u32 *id, u8 *ide, u8 *rtr, u8 *len, u8 *dat)
{
	*ide = CAN1->sFIFOMailBox[fifox].RIR&0x04;
	if(*ide==0)	//标准标识符
	{
		*id=CAN1->sFIFOMailBox[fifox].RIR>>21;
	}else{
		*id=CAN1->sFIFOMailBox[fifox].RIR>>3;
	}
	*rtr=CAN1->sFIFOMailBox[fifox].RIR&0x02;
	*len=CAN1->sFIFOMailBox[fifox].RDTR&0x0f;
	//接收数据
	dat[0]=CAN1->sFIFOMailBox[fifox].RDLR&0xff;
	dat[1]=(CAN1->sFIFOMailBox[fifox].RDLR>>8)&0xff;
	dat[2]=(CAN1->sFIFOMailBox[fifox].RDLR>16)&0xff;
	dat[3]=(CAN1->sFIFOMailBox[fifox].RDLR>>24)&0xff;
	dat[4]=CAN1->sFIFOMailBox[fifox].RDHR&0xff;
	dat[5]=(CAN1->sFIFOMailBox[fifox].RDHR>>8)&0xff;
	dat[6]=(CAN1->sFIFOMailBox[fifox].RDHR>>16)&0xff;
	dat[7]=(CAN1->sFIFOMailBox[fifox].RDHR>>24)&0xff;
	if(fifox==0)CAN1->RF0R|=0x20;
	else if(fifox==1)CAN1->RF1R|=0x20;
}

#if CAN1_RX0_INT_ENABLE	//使能RX0中断
//中断服务函数			    
void CAN1_RX0_IRQHandler(void)
{
	u8 rxbuf[8];
	u32 id;
	u8 ide,rtr,len;     
 	CAN1_Rx_Msg(0,&id,&ide,&rtr,&len,rxbuf);
    printf("id:%d\r\n",id);
    printf("ide:%d\r\n",ide);
    printf("rtr:%d\r\n",rtr);
    printf("len:%d\r\n",len);
    printf("rxbuf[0]:%d\r\n",rxbuf[0]);
    printf("rxbuf[1]:%d\r\n",rxbuf[1]);
    printf("rxbuf[2]:%d\r\n",rxbuf[2]);
    printf("rxbuf[3]:%d\r\n",rxbuf[3]);
    printf("rxbuf[4]:%d\r\n",rxbuf[4]);
    printf("rxbuf[5]:%d\r\n",rxbuf[5]);
    printf("rxbuf[6]:%d\r\n",rxbuf[6]);
    printf("rxbuf[7]:%d\r\n",rxbuf[7]);
}
#endif

//CAN发送一组数据
u8 CAN1_Send_Msg(u8* msg, u8 len)
{
	u8 mbox;
	u16 i=0;
	mbox = CAN1_Tx_Msg(0x12,0,0,len,msg);
	while((CAN1_Tx_Status(mbox)!=0X07)&&(i<0XFFF))i++;
	if(i>=0xfff) return 1;
	return 0;
}

//CAN口接收数据查询
u8 CAN1_Receive_Msg(u8 *buf)
{
	u32 id;
	u8 ide,rtr,len;
	if(CAN1_Msg_Pend(0)==0) return 0;
	CAN1_Rx_Msg(0,&id,&ide,&rtr,&len,buf);
	if(id!=0x12||ide!=0||rtr!=0)len=0;
	return len;
}
