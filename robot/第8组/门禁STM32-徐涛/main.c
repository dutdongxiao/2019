/*-------------------------------------------------------------------------------
�ļ����ƣ�main.c
�ļ���������������LED��˸��S1ȡ��LED2��S2ȡ��LED3��S3ͬʱȡ��LED2��LED3
          S4ʹLED2��LED3��˸5��
Ӳ��ƽ̨����ĪM3S������
��д����shifang
�̼���  ��V3.5
������̳��www.doflye.net
��    ע��ͨ�����޸Ŀ�����ֲ�����������壬����������Դ�����硣
---------------------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "led.h"
#include "delay.h"
#include "key.h"
#include "usart1.h"
float dist;
u16 msHcCount=0;



void cinit(){         //������GPIO���ų�ʼ��
	
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);    //ʹ��PB5 ������������������ź�
	  GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
	  GPIO_ResetBits(GPIOB,GPIO_Pin_5);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
    GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOB,GPIO_Pin_9);
}



void TIM6_Int_Init()    //��ʱ��ʱ�ӳ�ʼ��
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
	TIM_DeInit(TIM6);
	TIM_TimeBaseStructure.TIM_Period=999;
	TIM_TimeBaseStructure.TIM_Prescaler=71;
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM6,&TIM_TimeBaseStructure);
	
	TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE);        //����ʱ��6�����ж�
	
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel=TIM6_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;
  NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	
	
  TIM_Cmd(TIM6,DISABLE);	
}


void TIMEOPEN()
{
	TIM_SetCounter(TIM6,0);
	msHcCount=0;
	TIM_Cmd(TIM6,ENABLE);
}

void TIM6_IRQHandler()
{
	if(TIM_GetITStatus(TIM6,TIM_IT_Update)!=RESET)
	{
		TIM_ClearITPendingBit(TIM6,TIM_IT_Update);
		msHcCount++;
	}
}

void TIMECLOSE()
{
	TIM_Cmd(TIM6,DISABLE);
}


u32 GetEchoTimer(void)
{
	u32 t=0;
	t=msHcCount*1000;
	t+=TIM_GetCounter(TIM6);
	TIM6->CNT=0;
	return t;
}


float alls()
{
	u32 t=0;
	int i=0;
	float lengthTemp=0;
	float sum=0;
	while(i!=5)
	{
		GPIO_ResetBits(GPIOB,GPIO_Pin_9);
		GPIO_SetBits(GPIOB,GPIO_Pin_5);
		Delay_us(20);
		GPIO_ResetBits(GPIOB,GPIO_Pin_5);
		while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9)==0);

		TIMEOPEN();
		i=i+1;
		while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9)==1);
	
		TIMECLOSE();
		t=GetEchoTimer();
		lengthTemp=((float)t/58.0);
		sum=lengthTemp+sum;
		msHcCount=0;
		Delay_ms(100);
	}
	lengthTemp=sum/5.0;
	return lengthTemp;
}

int main(void)
{		
	uint8_t y;//�������
	int  x;
  LED_Init();//LED��ʼ��
  KEY_Init();//������ʼ��
  uart1_init(115200);
  SysTick_Init();//��ʱ��ʼ��
	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);  //ʹ��Pc13 ���ڶ�ȡ��ݮ���ź�
  GPIO_InitTypeDef GPIO_InitStructure2;
  GPIO_InitStructure2.GPIO_Pin = GPIO_Pin_13;
  GPIO_InitStructure2.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure2.GPIO_Mode = GPIO_Mode_IPD;
  GPIO_Init(GPIOC, &GPIO_InitStructure2);

	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  //ʹ��PA6 ���������������ź�
	GPIO_InitTypeDef GPIO_InitStructure1;
  GPIO_InitStructure1.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure1.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure1.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure1);
		

	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure1;          //TIM3����
	TIM_OCInitTypeDef TIM_OCInitStructure1;
	
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	TIM_TimeBaseStructure1.TIM_Period=199;
	TIM_TimeBaseStructure1.TIM_Prescaler=7199;
	TIM_TimeBaseStructure1.TIM_ClockDivision=0;
	TIM_TimeBaseStructure1.TIM_CounterMode=TIM_CounterMode_Up;
	

	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure1);
	
	TIM_OCInitStructure1.TIM_OCMode=TIM_OCMode_PWM2;
	TIM_OCInitStructure1.TIM_Pulse=0;
	TIM_OCInitStructure1.TIM_OutputState=TIM_OutputState_Enable;
	TIM_OCInitStructure1.TIM_OCPolarity=TIM_OCPolarity_Low;
	TIM_OC1Init(TIM3,&TIM_OCInitStructure1);
	TIM_Cmd(TIM3,ENABLE);
	cinit();
	TIM6_Int_Init();
	int flag=0;
	LED_Init();
TIM_SetCompare1(TIM3,24);     //��ת��ˮƽλ��


float ceshi;
  while (1)
  {              //��ȡ��ݮ����Ϣ
		if(flag==-1)
		{
			if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13)==1)
			{
				flag=0;
			}
		}
		if(flag==0){
//			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
				TIM_SetCompare1(TIM3,15);   //ת��90��
//	      Delay_ms(1000);
	//      RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, DISABLE);
	  	dist=alls();
			
			if(dist<20)
				flag=1;
		}
		else if(flag==1){
			dist=alls();
			if(dist>20)
				flag=2;
		}
		else if(flag==2){
			Delay_ms(200);
//		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
			TIM_SetCompare1(TIM3,24);
			Delay_ms(200);
//	    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, DISABLE);
			flag=-1;
		}
}
}
/*----------------------�·��� ������̳��www.doflye.net--------------------------*/

