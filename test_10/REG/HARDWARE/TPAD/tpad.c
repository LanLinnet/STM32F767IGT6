#include "tpad.h"
#include "delay.h"
#include "usart.h"

#define TPAD_ARR_MAX_VAL 0xffffffff		//����ARRֵ��TIM2��32λ�Ĵ�����
vu16 tpad_default_val=0;				//���ص�ʱ��û���ְ��£�����������Ҫ��ʱ��

//��ʼ����������������0�ɹ�������1ʧ��
u8 TPAD_Init(u8 psc)
{
	u16 buf[10];	//���reset��ļ�����ֵ��10�Σ�
	u16 temp;		//��ʱ�м����
	u8 i,j;			//�м�ѭ������
	
	TIM2_CH1_Cap_Init(TPAD_ARR_MAX_VAL, psc-1);		//���÷�Ƶϵ��
	
	for(i=0; i<10; i++)		//������ȡ10��
	{
		buf[i]=TPAD_Get_Val();
		delay_ms(10);
	}
	
	for(i=0;i<9;i++)
	{
		for(j=i+1;j<10;j++)
		{
			if(buf[i]>buf[j])		//����
			{
				temp=buf[i];
				buf[i]=buf[j];
				buf[j]=temp;
			}
		}
	}
	
	//ȡ�м�6����Ϊƽ��ֵ
	temp=0;
	for(i=2;i<8;i++) temp+=buf[i];
	tpad_default_val = temp/6;
	printf("tpad_default_val: %d\r\n",tpad_default_val);
	
	//���س�ʼ�����
	if(tpad_default_val>TPAD_ARR_MAX_VAL/2) return 1;	//��ʼ����������TPAD_ARR_MAX_VAL/2����ֵ����������
	return 0;
}

//��λ���ͷŵ��ݣ���ռ�����
void TPAD_Reset(void)
{
	GPIO_Set(GPIOA, PIN5, GPIO_MODE_OUT, GPIO_OTYPE_PP, GPIO_SPEED_100M, GPIO_PUPD_PD);		//PA5�������������
	GPIO_Pin_Set(GPIOA, PIN5, 0);		//PA5���0�����ݷŵ�
	delay_ms(5);
	TIM2->SR = 0;		//������
	TIM2->CNT = 0;		//����������
	GPIO_Set(GPIOA, PIN5, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_SPEED_100M, GPIO_PUPD_NONE);	//PA5���ù��ܣ�����������	
}

//��ȡ��ʱ������ֵ�����ز���ֵ/����ֵ����ʱ��
u16 TPAD_Get_Val(void)
{
	TPAD_Reset();
	//�ȴ����������� [1]-CC1IF: Capture/Compare 1 interrupt flag 
	//0: No input capture occurred  
	//1: The counter value has been captured in TIMx_CCR1 register
	while((TIM2->SR&0x02)==0)	
	{
		if(TIM2->CNT>TPAD_ARR_MAX_VAL-500) return TIM2->CNT;		//��ʱ������CNT
	}
	return TIM2->CCR1;
}

//��ȡn�ζ�ʱ������ֵ����ȡ���ֵ����
u16 TPAD_Get_MaxVal(u8 n)
{
	u16 temp=0;
	u16 res=0;				//result�����ڴ����󲶻�ֵ
	u8 lcntnum=n*2/3;		//����2/3*n����Ч����������Ч
	u8 okcnt=0;
	
	while(n--)
	{
		temp=TPAD_Get_Val();
		if(temp>(tpad_default_val*5/4)) okcnt++;	//����ֵ����Ҫ����Ĭ��ֵ��5/4������Ч
		if(temp>res)res=temp;
	}
	if(okcnt>=lcntnum) return res;		//��Ч������������Ҫ����2/3�ŷ������ֵ
	else return 0;		//������Ϊ��������0
}

//ɨ�败������
//mode�� 0-����������  1-��������
//����ֵ�� 0-û�а���  1-����
u8 TPAD_Scan(u8 mode)
{
	//static���ξֲ�����,��������δ�ı�����ֲ�������ͬ�������������ڸı䣬�������ʱ������
	static u8 keyen=0;		//0-���Կ�ʼ���  ����0-�����ܿ�ʼ���
	u8 res=0;	
	u8 sample=3;		//Ĭ�ϲ�������Ϊ3
	u16 rval;			//�����󲶻�ֵ
	
	if(mode)	//��������ģʽ
	{
		sample=6;		//���ò�������Ϊ6��
		keyen=0;		//֧������
	}
	
	rval=TPAD_Get_MaxVal(sample);		//��ȡ��󲶻�ֵ
	//����tpad_default_val+(1/3)*����tpad_default_val,��С��10��tpad_default_val,����Ч
	if(rval>(tpad_default_val*4/3) && rval<(10*tpad_default_val))
	{
		if(keyen==0) res=1;  //keyen==0���Կ�ʼ��⣬����1����������
		keyen=3;		//����keyenֵ���´���Ȼ��Ҫ�ٹ�3��֮�󰴼�����Ч
	}
	
	if(keyen) keyen--;
	return res;
}

//��ʱ��2ͨ��1���벶������
//arr-����ֵ  psc-Ԥ��Ƶϵ��
void TIM2_CH1_Cap_Init(u32 arr, u16 psc)
{
	RCC->APB1ENR|=1<<0;		//TIM2ʱ��ʹ��    
	RCC->AHB1ENR|=1<<0;   	//ʹ��PORTAʱ��	
	GPIO_Set(GPIOA,PIN5,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_NONE);//PA5,���ù���,����������
	GPIO_AF_Set(GPIOA,5,1);	//PA5,AF1 
	  
 	TIM2->ARR=arr;  		//�趨�������Զ���װֵ//�պ�1ms    
	TIM2->PSC=psc;  		//Ԥ��Ƶ��,1M�ļ���Ƶ��	 

	TIM2->CCMR1|=1<<0;		//CC1S=01 	ѡ������� IC1ӳ�䵽TI1��
 	TIM2->CCMR1|=0<<4; 		//IC1F=0000 ���������˲��� ���˲� 
 
	TIM2->EGR=1<<0;			//������Ʋ��������¼�,ʹд��PSC��ֵ������Ч,���򽫻�Ҫ�ȵ���ʱ������Ż���Ч!
	TIM2->CCER|=0<<1; 		//CC1P=0	�����ز���
	TIM2->CCER|=1<<0; 		//CC1E=1 	�������������ֵ������Ĵ�����
																 
	TIM2->CR1|=0x01;    	//ʹ�ܶ�ʱ��2
}
