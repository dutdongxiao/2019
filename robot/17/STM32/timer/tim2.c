#include "tim2.h"
void TIM2_Init(u16 arr,u16 psc)
{	  
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;	
	TIM_OCInitTypeDef  TIM_OCInitStructure;	// ����ΪPWMģʽ	
	NVIC_InitTypeDef NVIC_InitStructure;
	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	/* TIM2��PWM1��PWM2ͨ����Ӧ������PA0123*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
  
	/*--------------------ʱ���ṹ���ʼ��-------------------------*/	
	
	// ������ʱ��ʱ��,���ڲ�ʱ��CK_INT=72M	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE); 
	TIM_TimeBaseStructure.TIM_Period= arr;	//����	
	TIM_TimeBaseStructure.TIM_Prescaler= psc;		//��Ƶ
	TIM_TimeBaseStructure.TIM_ClockDivision=0;		
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;	//	���ϴ���		
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); 
	TIM_Cmd(TIM2,DISABLE);
  
	//���ñȽϼĴ���
	/*TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
	TIM_OCInitStructure.TIM_Pulse = cmp;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//�Ƚ����ʹ��
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC1Init(TIM2, &TIM_OCInitStructure); 
	TIM_OC2Init(TIM2, &TIM_OCInitStructure); 
	TIM_OC3Init(TIM2, &TIM_OCInitStructure); 
	TIM_OC4Init(TIM2, &TIM_OCInitStructure); 
	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);//ʹ��TIMx��CCR1�ϵ�Ԥװ�ؼĴ���
	TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);//ʹ��TIMx��CCR1�ϵ�Ԥװ�ؼĴ���
	TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);//ʹ��TIMx��CCR1�ϵ�Ԥװ�ؼĴ���
	TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);//ʹ��TIMx��CCR1�ϵ�Ԥװ�ؼĴ���
	TIM_Cmd(TIM2,ENABLE)*///�˴��ǿ��ƶ���Ĵ��룬�Ѿ���������
	
	//�����ж�
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//��ռ���ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//�����ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE); //�����ж�
} 
