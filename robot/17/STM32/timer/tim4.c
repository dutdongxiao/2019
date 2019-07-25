#include <tim4.h>

void TIM4_Init(u16 arr,u16 psc)
{	  
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;	
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	/*--------------------ʱ���ṹ���ʼ��-------------------------*/	
	
	
	
	// ������ʱ��ʱ��,���ڲ�ʱ��CK_INT=72M	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE); 
	TIM_TimeBaseStructure.TIM_Period= arr;	//����
	TIM_TimeBaseStructure.TIM_Prescaler= psc;		//��Ƶ
	TIM_TimeBaseStructure.TIM_ClockDivision=0;		
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;	//	���ϴ���		
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); 
	TIM_Cmd(TIM4,DISABLE);
	
	//�����ж�
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//��ռ���ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//�����ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
	TIM_OCInitStructure.TIM_Pulse = 65535;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//�Ƚ����ʹ��
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);//ʹ��TIMx��CCR1�ϵ�Ԥװ�ؼĴ���
	
	TIM_ITConfig(TIM4,TIM_IT_Update,DISABLE); //ʧ��ָ����TIM4�ж�
} 

