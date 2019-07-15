#include "hcsr04.h"  
#include "stm32f10x.h"  
#include "delay.h"  
#include "stm32f10x_tim.h" 
#include "stm32f10x_gpio.h" 
#include "stm32f10x_rcc.h" 
#include "misc.h" 
#include "stm32f10x_conf.h" 
#define HCSR04_PORT     GPIOC  
#define HCSR04_CLK      RCC_APB2Periph_GPIOC  
#define HCSR04_TRIG     GPIO_Pin_5  
#define HCSR04_ECHO     GPIO_Pin_6  



u16 msHcCount = 0;//ms����  

void Hcsr04Init()
{
	//TIM2���ж�NVIC����
	NVIC_InitTypeDef  NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;     //�������ڶ�ʱ�����õĽṹ��  
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);			// ʹ��TIM2ʱ��
	RCC_APB2PeriphClockCmd(HCSR04_CLK, ENABLE);

	
	
	GPIO_InitStructure.GPIO_Pin = HCSR04_TRIG;       //���͵�ƽ����  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//�������  
	GPIO_Init(HCSR04_PORT, &GPIO_InitStructure);
	GPIO_ResetBits(HCSR04_PORT, HCSR04_TRIG);
	

	GPIO_InitStructure.GPIO_Pin = HCSR04_ECHO;        //���ص�ƽ����  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������  
	GPIO_Init(HCSR04_PORT, &GPIO_InitStructure);
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
static void OpenTimerForHc()    //�򿪶�ʱ��  
{
	TIM_SetCounter(TIM2, 0);//�������  
	msHcCount = 0;
	TIM_Cmd(TIM2, ENABLE);  //ʹ��TIMx����  
}

static void CloseTimerForHc()   //�رն�ʱ��  
{
	TIM_Cmd(TIM2, DISABLE);  //ʹ��TIMx����  
}


//��ʱ��2�жϷ������  
void TIM2_IRQHandler(void)   //TIM2�ж�  
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)  //���TIM3�����жϷ������  
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);  //���TIMx�����жϱ�־   
		msHcCount++;
	}
}

u32 GetEchoTimer(void)
{
	u32 t = 0;
	t = msHcCount * 1000;//�õ�MS  
	t += TIM_GetCounter(TIM2);//�õ�US 
  TIM2->CNT = 0; 
  Delay_Ms(50);
	return t;
}

//һ�λ�ȡ������������� ���β��֮����Ҫ���һ��ʱ�䣬���ϻ����ź�  
u32 Hcsr04GetLength1()
{
	u32 t = 0;
	int i = 0;
	float lengthTemp = 0;
	float sum = 0;
	while (i != 5)
	{
		GPIO_SetBits(HCSR04_PORT, HCSR04_TRIG);      //���Ϳڸߵ�ƽ���
		delay_nus(20);
		GPIO_ResetBits(HCSR04_PORT, HCSR04_TRIG);
		while (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_6 )== 0);      //�ȴ����տڸߵ�ƽ���
		OpenTimerForHc();        //�򿪶�ʱ��
		i = i + 1;
		while (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_6 )== 1);
		CloseTimerForHc();        //�رն�ʱ��
		t = GetEchoTimer();        //��ȡʱ��,�ֱ���Ϊ1US
		lengthTemp = ((float)t /58.0);//cm
		sum = lengthTemp + sum;

	}
	lengthTemp = sum / 5.0;
	return (u32)lengthTemp;
}

