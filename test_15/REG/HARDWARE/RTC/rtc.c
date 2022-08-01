#include "rtc.h"
#include "led.h"
#include "delay.h"
#include "usart.h"

//RTC��ʼ��
//Ĭ�ϳ���ʹ��LSE,��LSE����ʧ�ܺ�,�л�ΪLSI.
//ͨ��BKP�Ĵ���0��ֵ,�����ж�RTCʹ�õ���LSE/LSI:
//��BKP0==0X5050ʱ,ʹ�õ���LSE
//��BKP0==0X5051ʱ,ʹ�õ���LSI
//����ֵ:0,��ʼ���ɹ�;
//       1,�����ʼ��ģʽʧ��;
//ע��:�л�LSI/LSE������ʱ��/���ڶ�ʧ,�л�������������.
u8 RTC_Init(void)
{
	u16 ssr;
	u16 bkpflag;
	u16 retry=200;
	u32 tempreg=0;
	
	RCC->APB1ENR|=1<<28;		//ʹ�ܵ�Դ�ӿ�ʱ��
	PWR->CR1|=1<<8;				//���������ʹ��(RTC+SRAM) 
	bkpflag = RTC_Read_BKR(0);		//��ȡBKP0��ֵ
	if(bkpflag!=0x5050)
	{
		RCC->CSR|=1<<0;
		while(!(RCC->CSR&0x02));		//LSI RC oscillator not ready
		RCC->BDCR|=1<<0;
		while(retry&&((RCC->BDCR&0X02)==0))
		{
			retry--;
			delay_ms(5);
		}
		tempreg=RCC->BDCR;			//��ȡBDCR��ֵ
		tempreg&=~(3<<8);			//����8/9λ 
		if(retry==0)tempreg|=1<<9;	//LSE����ʧ��,����LSI. 
		else tempreg|=1<<8;			//ѡ��LSE,��ΪRTCʱ��   
		tempreg|=1<<15;				//ʹ��RTCʱ�� 
		RCC->BDCR=tempreg;			//��������BDCR�Ĵ���
		//�ر�RTC�Ĵ���д����
		RTC->WPR=0xCA;
		RTC->WPR=0x53;
		RTC->CR=0;
		if(RTC_Init_Mode())
		{
			RCC->BDCR=1<<16;
			delay_ms(10);
			RCC->BDCR=0;
			return 2;
		}
		RTC->PRER=0xff;
		RTC->PRER|=0x7f<<16;
		RTC->CR&=~(1<<6);
		RTC->ISR&=~(1<<7);
		RTC->WPR=0xff;
		if(bkpflag!=0x5051)
		{
			RTC_Set_Time(23,59,56,0);
			RTC_Set_Date(15,12,27,7);
		}
		if(retry==0)RTC_Write_BKR(0,0x5051);		//LSI
		else RTC_Write_BKR(0,0x5050);		//LSE
	}else
	{
		retry=10;
		ssr=RTC->SSR;
		while(retry)
		{
			delay_ms(10);
			if(ssr==RTC->SSR)retry--;
			else break;
		}
		if(retry==0)
		{
			RTC_Write_BKR(0,0xffff);
			RCC->BDCR=1<<16;
			delay_ms(10);
			RCC->BDCR=0;
		}
	}
	return 0;
}

//RTCʱ������
//hour,min,sec:Сʱ,����,����
//ampm:AM/PM,0=AM/24H,1=PM.
//����ֵ:0,�ɹ�
//       1,�����ʼ��ģʽʧ�� 
u8 RTC_Set_Time(u8 hour, u8 min, u8 sec, u8 ampm)
{
	u32 temp=0;
	RTC->WPR=0xCA;
	RTC->WPR=0x53;
	if(RTC_Init_Mode()) return 1;
	temp=(((u32)ampm&0x01)<<22)|((u32)RTC_DEC2BCD(hour)<<16)|((u32)RTC_DEC2BCD(min)<<8)|(RTC_DEC2BCD(sec));
	RTC->TR=temp;
	RTC->ISR&=~(1<<7);
	return 0;
}

//RTC��������
//year,month,date:��(0~99),��(1~12),��(0~31)
//week:����(1~7,0,�Ƿ�!)
//����ֵ:0,�ɹ�
//       1,�����ʼ��ģʽʧ��
u8 RTC_Set_Date(u8 year, u8 month, u8 date, u8 week)
{
	u32 temp=0;
	RTC->WPR=0xCA;
	RTC->WPR=0x53;
	if(RTC_Init_Mode()) return 1;
	temp=(((u32)week&0x07)<<13)|((u32)RTC_DEC2BCD(year)<<16)|((u32)RTC_DEC2BCD(month)<<8)|(RTC_DEC2BCD(date));
	RTC->DR=temp;
	RTC->ISR&=~(1<<7);
	return 0;
}

//��ȡRTCʱ��
//*hour,*min,*sec:Сʱ,����,���� 
//*ampm:AM/PM,0=AM/24H,1=PM.
void RTC_Get_Time(u8 *hour, u8 *min, u8 *sec, u8 *ampm)
{
	u32 temp=0;
	while(RTC_Wait_Synchro());
	temp=RTC->TR;
	*hour=RTC_BCD2DEC((temp>>16)&0x3f);
	*min=RTC_BCD2DEC((temp>>8)&0X7F);
	*sec=RTC_BCD2DEC(temp&0X7F);
	*ampm=temp>>22; 	
}

//��ȡRTC����
//*year,*mon,*date:��,��,��
//*week:����
void RTC_Get_Date(u8 *year,u8 *month,u8 *date,u8 *week)
{
	u32 temp=0;
 	while(RTC_Wait_Synchro());	//�ȴ�ͬ��  	 
	temp=RTC->DR;
	*year=RTC_BCD2DEC((temp>>16)&0XFF);
	*month=RTC_BCD2DEC((temp>>8)&0X1F);
	*date=RTC_BCD2DEC(temp&0X3F);
	*week=(temp>>13)&0X07; 
}

//��������ʱ��(����������,24Сʱ��)
//week:���ڼ�(1~7)
//hour,min,sec:Сʱ,����,����
void RTC_Set_AlarmA(u8 week, u8 hour, u8 min, u8 sec)
{
	RTC->WPR=0xCA; 
	RTC->WPR=0x53;
	RTC->CR&=~(1<<8);		//�ر�����A
	
	while((RTC->ISR&0x01)==0);
	
	RTC->ALRMAR=0;
	RTC->ALRMAR|=1<<30;
	RTC->ALRMAR|=0<<22;
	RTC->ALRMAR|=(u32)RTC_DEC2BCD(week)<<24;
	RTC->ALRMAR|=(u32)RTC_DEC2BCD(hour)<<16;
	RTC->ALRMAR|=(u32)RTC_DEC2BCD(min)<<8;
	RTC->ALRMAR|=(u32)RTC_DEC2BCD(sec);
	
	RTC->ALRMASSR=0;
	RTC->CR|=1<<12;
	RTC->CR|=1<<8;
	RTC->WPR=0xff;
	
	RTC->ISR&=~(1<<8);
	EXTI->PR=1<<17;
	EXTI->IMR|=1<<17;
	EXTI->RTSR|=1<<17;
	MY_NVIC_Init(2,2,RTC_Alarm_IRQn,2); //��ռ2�������ȼ�2����2 
}

//�����Ի��Ѷ�ʱ������
//wksel:000,RTC/16;001,RTC/8;010,RTC/4;011,RTC/2;
//      10x,ck_spre,1Hz;11x,1Hz,��cntֵ����2^16(��cnt+2^16)
//ע��:RTC����RTC��ʱ��Ƶ��,��RTCCLK!
//cnt:�Զ���װ��ֵ.����0,�����ж�.
void RTC_Set_WakeUp(u8 wksel, u16 cnt)
{
	RTC->WPR=0xCA; 
	RTC->WPR=0x53;
	RTC->CR&=~(1<<10);		//�ر�WAKE UP
	while((RTC->ISR&0x04)==0);
	RTC->CR&=~(7<<0);
	RTC->CR|=wksel&0x07;
	RTC->WUTR=cnt;
	RTC->ISR&=~(1<<10);
	RTC->CR|=1<<14;
	RTC->CR|=1<<10;
	RTC->WPR=0xff;
	
	EXTI->PR=1<<22;  			//���LINE22�ϵ��жϱ�־λ  
	EXTI->IMR|=1<<22;			//����line22�ϵ��ж� 
	EXTI->RTSR|=1<<22;			//line22���¼��������ش��� 
	MY_NVIC_Init(2,2,RTC_WKUP_IRQn,2); //��ռ2�������ȼ�2����2
}

//RTC�����жϷ�����
void RTC_Alarm_IRQHandler(void)
{
	if(RTC->ISR&(1<<8))
	{
		RTC->ISR&=~(1<<8);
		printf("ALARM A!\r\n");
	}
	EXTI->PR|=1<<17;
}

//RTC WAKE UP�жϷ�����
void RTC_WKUP_IRQHandler(void)
{
	static u8 led1sta;
	if(RTC->ISR&(1<<10))
	{
		RTC->ISR&=~(1<<10);
		LED1(led1sta^=1);
	}
	EXTI->PR|=1<<22;
}	

//�ȴ�RSFͬ��
//���أ�0-�ɹ�  1-ʧ��
u8 RTC_Wait_Synchro(void)
{
	u32 retry=0xfffff;
	RTC->WPR=0xca;
	RTC->WPR=0x53;
	RTC->ISR&=~(1<<5);
	while(retry&&((RTC->ISR & (1<<5))==0x00)) retry--;
	if(retry==0) return 1;
	RTC->WPR=0xff;
	return 0;
}

//RTC�����ʼ��ģʽ
//����ֵ:0,�ɹ�;1,ʧ��;
u8 RTC_Init_Mode(void)
{ 
	u32 retry=0XFFFFF; 
	if(RTC->ISR&(1<<6))return 0; 
	RTC->ISR|=1<<7;	//����RTC��ʼ��ģʽ
	while(retry&&((RTC->ISR&(1<<6))==0x00))retry--;//�ȴ�����RTC��ʼ��ģʽ�ɹ� 
    if(retry==0)return 1;	//ͬ��ʧ��
	else return 0; 			//ͬ���ɹ� 
}
//RTCд�������SRAM
//BKPx:�����Ĵ������,��Χ:0~19
//data:Ҫд�������,32λ����
void RTC_Write_BKR(u32 BKRx,u32 data)
{
	u32 temp=0; 
	temp=RTC_BASE+0x50+BKRx*4;   
	(*(u32*)temp)=data; 
}
//RTC��ȡ������SRAM
//BKPx:�����Ĵ������,��Χ:0~19
//����ֵ:��ȡ��������
u32 RTC_Read_BKR(u32 BKRx)
{
	u32 temp=0; 
	temp=RTC_BASE+0x50+BKRx*4;  
	return (*(u32*)temp);		//���ض�ȡ����ֵ
}

//ʮ����ת��ΪBCD��
//val:Ҫת����ʮ������
//����ֵ:BCD��
u8 RTC_DEC2BCD(u8 val)
{
	u8 bcdhigh = 0; 
	while(val>=10)
	{
		bcdhigh++;
		val-=10;
	} 
	return ((u8)(bcdhigh<<4)|val);
}
//BCD��ת��Ϊʮ��������
//val:Ҫת����BCD��
//����ֵ:ʮ��������
u8 RTC_BCD2DEC(u8 val)
{
	u8 temp=0;
	temp=(val>>4)*10;
	return (temp+(val&0X0F));
}

u8 const table_week[12]={0,3,3,6,1,4,6,2,5,0,3,5}; //���������ݱ�
//������������ڼ�
//��������:���빫�����ڵõ�����(ֻ����1901-2099��)
//year,month,day������������ 
//����ֵ�����ں�(1~7,������1~����)																						 
u8 RTC_Get_Week(u16 year,u8 month,u8 day)
{	
	u16 temp2;
	u8 yearH,yearL;
	yearH=year/100;	yearL=year%100; 
	// ���Ϊ21����,�������100  
	if (yearH>19)yearL+=100;
	// ����������ֻ��1900��֮���  
	temp2=yearL+yearL/4;
	temp2=temp2%7; 
	temp2=temp2+day+table_week[month-1];
	if (yearL%4==0&&month<3)temp2--;
	temp2%=7;
	if(temp2==0)temp2=7;
	return temp2;
}
