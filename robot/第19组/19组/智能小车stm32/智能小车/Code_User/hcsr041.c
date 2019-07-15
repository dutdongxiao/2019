#include "hcsr041.h"  
#include "stm32f10x.h"  
#include "delay.h"  
#include "stm32f10x_tim.h" 
#include "stm32f10x_gpio.h" 
#include "stm32f10x_rcc.h" 
#include "misc.h" 
#include "stm32f10x_conf.h" 
#define HCSR041_PORT     GPIOC  
#define HCSR041_CLK      RCC_APB2Periph_GPIOC  
#define HCSR041_TRIG     GPIO_Pin_1  
#define HCSR041_ECHO     GPIO_Pin_3

u16 msHcCount1 = 0;//ms����  
 void Hcsr041Init(void)
{
	//TIM2���ж�NVIC����
	NVIC_InitTypeDef  NVIC_InitStructure; 
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;     //�������ڶ�ʱ�����õĽṹ��  
	GPIO_InitTypeDef GPIO_InitStructure1;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);			// ʹ��TIM2ʱ��
	RCC_APB2PeriphClockCmd(HCSR041_CLK, ENABLE);

	
	
	GPIO_InitStructure1.GPIO_Pin = HCSR041_TRIG;       //���͵�ƽ����  
	GPIO_InitStructure1.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure1.GPIO_Mode = GPIO_Mode_Out_PP;//�������  
	GPIO_Init(HCSR041_PORT, &GPIO_InitStructure1);
	GPIO_ResetBits(HCSR041_PORT, HCSR041_TRIG);
	

	GPIO_InitStructure1.GPIO_Pin = HCSR041_ECHO;        //���ص�ƽ����  
	GPIO_InitStructure1.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure1.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������  
	GPIO_Init(HCSR041_PORT, &GPIO_InitStructure1);
	//��ʱ����ʼ�� ʹ�û�����ʱ��TIM6  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	//��ʱ��TIM3��ʼ��
	
	
	TIM_TimeBaseStructure.TIM_Period = 999; 		// ������һ�������¼���װ���Զ���װ�ؼĴ�����ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler = 71;// ����TIM3ʱ��Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; 	// ����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); 			// ���ݲ�����ʼ��TIM3�ļ�����ʽ
  TIM_ClearFlag(TIM2, TIM_FLAG_Update);  
       
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE ); 		// ����TIM2�����ж�
	//TIM3���ж�NVIC����
	
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  			// TIM2�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  	// ��ռ���ȼ�1��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  		// �����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 			// ʹ��TIM2_IRQnͨ��
	NVIC_Init(&NVIC_InitStructure);  							// ��ʼ��NVIC�Ĵ���


	TIM_Cmd(TIM2, ENABLE);  									// ʹ��TIM2

}
static void OpenTimerForHc1()    //�򿪶�ʱ��  
{
	TIM_SetCounter(TIM2, 0);//�������  
	msHcCount1 = 0;
	TIM_Cmd(TIM2, ENABLE);  //ʹ��TIMx����  
}

static void CloseTimerForHc1()   //�رն�ʱ��  
{
	TIM_Cmd(TIM2, DISABLE);  //ʹ��TIMx����  
}


//��ʱ��2�жϷ������  
void TIM2_IRQHandler1(void)   //TIM2�ж�  
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)  //���TIM3�����жϷ������  
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);  //���TIMx�����жϱ�־   
		msHcCount1++;
	}
}

u32 GetEchoTimer1(void)
{
	u32 t = 0;
	t = msHcCount1 * 1000;//�õ�MS  
	t += TIM_GetCounter(TIM2);//�õ�US 
  TIM2->CNT = 0; 
  Delay_Ms(50);
	return t;
}

//һ�λ�ȡ������������� ���β��֮����Ҫ���һ��ʱ�䣬���ϻ����ź�  
 u32 Hcsr041GetLength1(void)
{
	u32 t = 0;
	int i = 0;
	float lengthTemp = 0;
	float sum = 0;
	while (i != 5)
	{
		GPIO_SetBits(HCSR041_PORT, HCSR041_TRIG);      //���Ϳڸߵ�ƽ���
		delay_nus(20);
		GPIO_ResetBits(HCSR041_PORT, HCSR041_TRIG);
		while (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3 )== 0);      //�ȴ����տڸߵ�ƽ���
		OpenTimerForHc1();        //�򿪶�ʱ��
		i = i + 1;
		while (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3)== 1);
		CloseTimerForHc1();        //�رն�ʱ��
		t = GetEchoTimer1();        //��ȡʱ��,�ֱ���Ϊ1US
		lengthTemp = ((float)t /58.0);//cm
		sum = lengthTemp + sum;

	}
	lengthTemp = sum / 5.0;
	return (u32)lengthTemp;
}

