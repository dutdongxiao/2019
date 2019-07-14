#include "TIM3.h"
#include "led.h"
#include "delay.h"
#include "usart4.h"
/*******ʵ��ʹ�õļ�ʱ��ΪTIM2*********/
uint16_t TimerPeriod = 0;
uint16_t Channel1Pulse = 0, Channel2Pulse = 0;
TIM_OCInitTypeDef  TIM_OCInitStructure;
int wheel_1 = 400;
int wheel_2 = 400; 
int wheel_3 = 400;
int wheel_4 = 400;
	extern int flag_heart;
	extern int flag_one;
	extern int flag_two;
  extern int flag_three;
  extern int flag_four ;
	extern int flag_five ;
	extern int flag_shake;
	extern int flag_node;
void RUN_Init(void)
{
	//ʹ�ܶ�Ӧ��������ת��ͨ����Ӧ�ܽ�
  GPIO_InitTypeDef GPIO_InitStructure;	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9| GPIO_Pin_10| GPIO_Pin_11| GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_PP;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOD,GPIO_Pin_9);
	GPIO_SetBits(GPIOD,GPIO_Pin_10);
	GPIO_ResetBits(GPIOD,GPIO_Pin_11);
	GPIO_ResetBits(GPIOD,GPIO_Pin_12);
	
}
void TIM3_Toggle_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	/* TIM3��ͨ��1��2��Ӧ������PA6��PA7,PB0,PB1����Щ������Ӳ���ֲ����ҵ�*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	/* 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;  //TIM3��ͨ��3��4��Ӧ��������PB0��PB1,���ùܽ�
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);	*/
}
void TIM3_Init(void)
{	  
	/*--------------------ʱ���ṹ���ʼ��-------------------------*/	
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;	
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;	// ����ΪToggleģʽ,��ȽϼĴ������ʱ��ת	
	
	//����ӳ��
//	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);  
//	 GPIO_PinRemapConfig(GPIO_PartialRemap2_TIM2, ENABLE);

	
	// ������ʱ��ʱ��,���ڲ�ʱ��CK_INT=72M	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE); 
	TIM_TimeBaseStructure.TIM_Period= 65535;	//����	
	TIM_TimeBaseStructure.TIM_Prescaler= 35;		//��Ƶ
	TIM_TimeBaseStructure.TIM_ClockDivision=0;		
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;	//	���ϴ���		
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); 
/******************
	//���ñȽϼĴ���
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Toggle;
	TIM_OCInitStructure.TIM_Pulse = 19999;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);
	TIM_Cmd(TIM3, ENABLE);
	//TIM_SetCompare1(TIM3,10000); 	 
	*************/
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Toggle;      //��תģʽ
TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //����ͨ����Ч
TIM_OCInitStructure.TIM_Pulse = 500;        //ռ��ʱ��
TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;

TIM_OC1Init(TIM3, &TIM_OCInitStructure);  //����TIM_OCInitStruct��ָ���Ĳ�����ʼ������TIM3
TIM_OC2Init(TIM3, &TIM_OCInitStructure);  //����TIM_OCInitStruct��ָ���Ĳ�����ʼ������TIM3
TIM_OC3Init(TIM3, &TIM_OCInitStructure);  //����TIM_OCInitStruct��ָ���Ĳ�����ʼ������TIM3
TIM_OC4Init(TIM3, &TIM_OCInitStructure);  //����TIM_OCInitStruct��ָ���Ĳ�����ʼ������TIM3

TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Disable);//ʧ��TIM3��CCR1�ϵ�Ԥװ�ؼĴ���
TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Disable);//ʧ��TIM3��CCR2�ϵ�Ԥװ�ؼĴ���
TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Disable);//ʧ��TIM3��CCR3�ϵ�Ԥװ�ؼĴ���
TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Disable);//ʧ��TIM3��CCR4�ϵ�Ԥװ�ؼĴ���	  

  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//��ռ���ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//�����ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���		
		
	TIM_ITConfig(TIM3, TIM_IT_CC1|TIM_IT_CC2|TIM_IT_CC3|TIM_IT_CC4 , ENABLE);
  TIM_Cmd(TIM3,ENABLE);
	
} 
void LedToggleCtrl(uint8_t Toggle)
{
	uint8_t Capture1;
	Capture1 = TIM_GetCapture1(TIM3);
    TIM_SetCompare1(TIM3, 40000*Toggle/100 );//����TIMx�Զ���װ�ؼĴ���ֵ
}
void Open(void)
{
  LED2_ON;
	//�������ʹ���źţ�ST = 1;
	GPIO_InitTypeDef GPIO_InitStructure;	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_PP;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
	GPIO_SetBits(GPIOE,GPIO_Pin_7);

	 wheel_1 = 400;
	 wheel_2 = 400;
	 wheel_3 = 400;
	 wheel_4 = 400;
}
void Stop(void)
{
	//�������ʧ���ź�,ST = 0;
	GPIO_InitTypeDef GPIO_InitStructure;	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_PP;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOE,GPIO_Pin_7);
	
	flag_heart = 0;
	flag_one = 0;
	flag_two = 0;
	flag_three = 0;
	flag_four = 0;
	flag_five = 0;
	flag_shake = 0;
	flag_node = 0;
}
void State_Rev(void)
{
	if(GPIO_ReadOutputDataBit(GPIOE,GPIO_Pin_7))
		Stop();
	else
		Open();
}
void Round(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9| GPIO_Pin_10| GPIO_Pin_11| GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_PP;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	GPIO_ResetBits(GPIOD,GPIO_Pin_9);
	GPIO_SetBits(GPIOD,GPIO_Pin_10);
	GPIO_ResetBits(GPIOD,GPIO_Pin_11);
	GPIO_SetBits(GPIOD,GPIO_Pin_12);
	
	 Open();
	 wheel_1 = 30000;
	 wheel_2 = 30000;
	 wheel_3 = 400;
	 wheel_4 = 400;
}
void One(void)
{
	Run();
	Delay_ms(7000);
	Stop();
}
void Run(void)
{ 
	//���������ת���ĸ������ź�
  GPIO_InitTypeDef GPIO_InitStructure;	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9| GPIO_Pin_10| GPIO_Pin_11| GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_PP;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOD,GPIO_Pin_9);
	GPIO_SetBits(GPIOD,GPIO_Pin_10);
	GPIO_ResetBits(GPIOD,GPIO_Pin_11);
	GPIO_ResetBits(GPIOD,GPIO_Pin_12);
	LED2_ON;
	 Open();
	 wheel_1 = 400;
	 wheel_2 = 400;
	 wheel_3 = 400;
	 wheel_4 = 400;
}
void Back(void)
{
	//����ʹ���ĸ������ת���ź�
  GPIO_InitTypeDef GPIO_InitStructure;	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9| GPIO_Pin_10| GPIO_Pin_11| GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_PP;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	GPIO_ResetBits(GPIOD,GPIO_Pin_9);
	GPIO_ResetBits(GPIOD,GPIO_Pin_10);
	GPIO_SetBits(GPIOD,GPIO_Pin_11);
	GPIO_SetBits(GPIOD,GPIO_Pin_12);
	
	Open();
	
	wheel_1 = 400;
	wheel_2 = 400;
	wheel_3 = 400;
  wheel_4 = 400;
}
void Left(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9| GPIO_Pin_10| GPIO_Pin_11| GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_PP;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOD,GPIO_Pin_9);
	GPIO_SetBits(GPIOD,GPIO_Pin_10);
	GPIO_ResetBits(GPIOD,GPIO_Pin_11);
	GPIO_ResetBits(GPIOD,GPIO_Pin_12);
	
	Open();
	
	wheel_1 = 300;
	wheel_2 = 700;
	wheel_3 = 700;
	wheel_4 = 300;
}
void Right(void)
{ 
	GPIO_InitTypeDef GPIO_InitStructure;	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9| GPIO_Pin_10| GPIO_Pin_11| GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_PP;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOD,GPIO_Pin_9);
	GPIO_SetBits(GPIOD,GPIO_Pin_10);
	GPIO_ResetBits(GPIOD,GPIO_Pin_11);
	GPIO_ResetBits(GPIOD,GPIO_Pin_12);
	Open();
	
	wheel_1 = 700;
	wheel_2 = 300;
	wheel_3 = 300;
	wheel_4 = 700;
}
void Q_shift(void)
{
		//�����������ƽ�Ƶ��ĸ������źţ������ת���Ҳ෴ת
  GPIO_InitTypeDef GPIO_InitStructure;	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9| GPIO_Pin_10| GPIO_Pin_11| GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_PP;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	GPIO_ResetBits(GPIOD,GPIO_Pin_9);
	GPIO_ResetBits(GPIOD,GPIO_Pin_10);
	GPIO_ResetBits(GPIOD,GPIO_Pin_11);
	GPIO_ResetBits(GPIOD,GPIO_Pin_12);

	 Open();
	 wheel_1 = 500;
	 wheel_2 = 500;
   wheel_3 = 500;
	 wheel_4 = 500;
}
void E_shift(void)
{
		//���������ת���ĸ������ź�
  GPIO_InitTypeDef GPIO_InitStructure;	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9| GPIO_Pin_10| GPIO_Pin_11| GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_PP;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOD,GPIO_Pin_9);
	GPIO_SetBits(GPIOD,GPIO_Pin_10);
	GPIO_SetBits(GPIOD,GPIO_Pin_11);
	GPIO_SetBits(GPIOD,GPIO_Pin_12);
	
	 Open();
	 wheel_1 = 500;
	 wheel_2 = 500;
	 wheel_3 = 500;
	 wheel_4 = 5000;
}
void Round_1 (void)
{
	GPIO_InitTypeDef GPIO_InitStructure;	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9| GPIO_Pin_10| GPIO_Pin_11| GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_PP;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	GPIO_ResetBits(GPIOD,GPIO_Pin_9);
	GPIO_ResetBits(GPIOD,GPIO_Pin_10);
	GPIO_SetBits(GPIOD,GPIO_Pin_11);
	GPIO_ResetBits(GPIOD,GPIO_Pin_12);
	
	 Open();
	 wheel_1 = 30000;
	 wheel_2 = 30000;
	 wheel_3 = 400;
	 wheel_4 = 400;
}
void Two(void)
{
	Round();
	Delay_ms(1970);
	Run();
	Delay_ms(4900);
	Round_1();
	Delay_ms(1900);
	Right();
  Delay_ms(2800);
	Left();
	Delay_ms(8000);
	Stop();
}
void Three(void)
{
	Round_1();
	Delay_ms(2000);
	Run();
	Delay_ms(1000);
	Left();
	Delay_ms(5800);
	Round_1();
	Delay_ms(4000);
	Run();
	Delay_ms(1000);
	Left();
	Delay_ms(5800);
	Stop();
}
void Four(void)
{
	Run();
	Delay_ms(8600);
	Round();
	Delay_ms(3200);
	Run();
	Delay_ms(5000);
	Round();
	Delay_ms(2900);
	Run();
	Delay_ms(4400);
	Stop();
}
void Heart(void)
{
	Left();
	Delay_ms(1200);
	Run();
	Delay_ms(5580);
	Right();
	Delay_ms(6700);
	Round();
	Delay_ms(4000);
	Right();
	Delay_ms(7000);
	Run();
	Delay_ms(5680);
	Stop();
}
u16 capture;
void TIM3_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM3, TIM_IT_CC1) != RESET)
	{
			TIM_ClearITPendingBit(TIM3, TIM_IT_CC1 );
			capture = TIM_GetCapture1(TIM3);
			TIM_SetCompare1(TIM3, capture + wheel_4 );
	}
	if (TIM_GetITStatus(TIM3, TIM_IT_CC2) != RESET)
	{
			TIM_ClearITPendingBit(TIM3, TIM_IT_CC2);
			capture = TIM_GetCapture2(TIM3);
			TIM_SetCompare2(TIM3, capture + wheel_3);	
	}
	if (TIM_GetITStatus(TIM3, TIM_IT_CC3) != RESET)
	{
			TIM_ClearITPendingBit(TIM3, TIM_IT_CC3);
			capture = TIM_GetCapture3(TIM3);
			TIM_SetCompare3(TIM3, capture + wheel_1);
	}
	if (TIM_GetITStatus(TIM3, TIM_IT_CC4) != RESET)
	{
			TIM_ClearITPendingBit(TIM3, TIM_IT_CC4);
			capture = TIM_GetCapture4(TIM3);
			TIM_SetCompare4(TIM3, capture + wheel_2);
	}
}