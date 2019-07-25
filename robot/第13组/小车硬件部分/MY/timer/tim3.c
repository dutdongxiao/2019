#include "TIM3.h"
#include "led.h"
#include "delay.h"
#include "usart4.h"
/*******实际使用的计时器为TIM2*********/
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
	//使能对应控制正反转四通道对应管脚
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
	/* TIM3的通道1和2对应引脚是PA6和PA7,PB0,PB1，这些可以在硬件手册中找到*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	/* 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;  //TIM3的通道3和4对应的引脚是PB0和PB1,配置管脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);	*/
}
void TIM3_Init(void)
{	  
	/*--------------------时基结构体初始化-------------------------*/	
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;	
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;	// 配置为Toggle模式,与比较寄存器相等时翻转	
	
	//打开重映像
//	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);  
//	 GPIO_PinRemapConfig(GPIO_PartialRemap2_TIM2, ENABLE);

	
	// 开启定时器时钟,即内部时钟CK_INT=72M	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE); 
	TIM_TimeBaseStructure.TIM_Period= 65535;	//周期	
	TIM_TimeBaseStructure.TIM_Prescaler= 35;		//分频
	TIM_TimeBaseStructure.TIM_ClockDivision=0;		
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;	//	向上触发		
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); 
/******************
	//设置比较寄存器
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Toggle;
	TIM_OCInitStructure.TIM_Pulse = 19999;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);
	TIM_Cmd(TIM3, ENABLE);
	//TIM_SetCompare1(TIM3,10000); 	 
	*************/
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Toggle;      //翻转模式
TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //正向通道有效
TIM_OCInitStructure.TIM_Pulse = 500;        //占空时间
TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;

TIM_OC1Init(TIM3, &TIM_OCInitStructure);  //根据TIM_OCInitStruct中指定的参数初始化外设TIM3
TIM_OC2Init(TIM3, &TIM_OCInitStructure);  //根据TIM_OCInitStruct中指定的参数初始化外设TIM3
TIM_OC3Init(TIM3, &TIM_OCInitStructure);  //根据TIM_OCInitStruct中指定的参数初始化外设TIM3
TIM_OC4Init(TIM3, &TIM_OCInitStructure);  //根据TIM_OCInitStruct中指定的参数初始化外设TIM3

TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Disable);//失能TIM3在CCR1上的预装载寄存器
TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Disable);//失能TIM3在CCR2上的预装载寄存器
TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Disable);//失能TIM3在CCR3上的预装载寄存器
TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Disable);//失能TIM3在CCR4上的预装载寄存器	  

  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//子优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器		
		
	TIM_ITConfig(TIM3, TIM_IT_CC1|TIM_IT_CC2|TIM_IT_CC3|TIM_IT_CC4 , ENABLE);
  TIM_Cmd(TIM3,ENABLE);
	
} 
void LedToggleCtrl(uint8_t Toggle)
{
	uint8_t Capture1;
	Capture1 = TIM_GetCapture1(TIM3);
    TIM_SetCompare1(TIM3, 40000*Toggle/100 );//设置TIMx自动重装载寄存器值
}
void Open(void)
{
  LED2_ON;
	//给出电机使能信号，ST = 1;
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
	//给出电机失能信号,ST = 0;
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
	//给出电机正转的四个引脚信号
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
	//给出使得四个电机反转的信号
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
		//给出电机向左平移的四个引脚信号，左侧正转，右侧反转
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
		//给出电机正转的四个引脚信号
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