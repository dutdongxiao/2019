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

u16 msHcCount1 = 0;//ms计数  
 void Hcsr041Init(void)
{
	//TIM2的中断NVIC设置
	NVIC_InitTypeDef  NVIC_InitStructure; 
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;     //生成用于定时器设置的结构体  
	GPIO_InitTypeDef GPIO_InitStructure1;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);			// 使能TIM2时钟
	RCC_APB2PeriphClockCmd(HCSR041_CLK, ENABLE);

	
	
	GPIO_InitStructure1.GPIO_Pin = HCSR041_TRIG;       //发送电平引脚  
	GPIO_InitStructure1.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure1.GPIO_Mode = GPIO_Mode_Out_PP;//推挽输出  
	GPIO_Init(HCSR041_PORT, &GPIO_InitStructure1);
	GPIO_ResetBits(HCSR041_PORT, HCSR041_TRIG);
	

	GPIO_InitStructure1.GPIO_Pin = HCSR041_ECHO;        //返回电平引脚  
	GPIO_InitStructure1.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure1.GPIO_Mode = GPIO_Mode_IN_FLOATING;//下拉输入  
	GPIO_Init(HCSR041_PORT, &GPIO_InitStructure1);
	//定时器初始化 使用基本定时器TIM6  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	//定时器TIM3初始化
	
	
	TIM_TimeBaseStructure.TIM_Period = 999; 		// 设置下一个更新事件后，装入自动重装载寄存器的值	
	TIM_TimeBaseStructure.TIM_Prescaler = 71;// 设置TIM3时钟预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; 	// 设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // TIM向上计数模式
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); 			// 根据参数初始化TIM3的计数方式
  TIM_ClearFlag(TIM2, TIM_FLAG_Update);  
       
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE ); 		// 允许TIM2更新中断
	//TIM3的中断NVIC设置
	
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  			// TIM2中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  	// 抢占优先级1级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  		// 子优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 			// 使能TIM2_IRQn通道
	NVIC_Init(&NVIC_InitStructure);  							// 初始化NVIC寄存器


	TIM_Cmd(TIM2, ENABLE);  									// 使能TIM2

}
static void OpenTimerForHc1()    //打开定时器  
{
	TIM_SetCounter(TIM2, 0);//清除计数  
	msHcCount1 = 0;
	TIM_Cmd(TIM2, ENABLE);  //使能TIMx外设  
}

static void CloseTimerForHc1()   //关闭定时器  
{
	TIM_Cmd(TIM2, DISABLE);  //使能TIMx外设  
}


//定时器2中断服务程序  
void TIM2_IRQHandler1(void)   //TIM2中断  
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)  //检查TIM3更新中断发生与否  
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);  //清除TIMx更新中断标志   
		msHcCount1++;
	}
}

u32 GetEchoTimer1(void)
{
	u32 t = 0;
	t = msHcCount1 * 1000;//得到MS  
	t += TIM_GetCounter(TIM2);//得到US 
  TIM2->CNT = 0; 
  Delay_Ms(50);
	return t;
}

//一次获取超声波测距数据 两次测距之间需要相隔一段时间，隔断回响信号  
 u32 Hcsr041GetLength1(void)
{
	u32 t = 0;
	int i = 0;
	float lengthTemp = 0;
	float sum = 0;
	while (i != 5)
	{
		GPIO_SetBits(HCSR041_PORT, HCSR041_TRIG);      //发送口高电平输出
		delay_nus(20);
		GPIO_ResetBits(HCSR041_PORT, HCSR041_TRIG);
		while (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3 )== 0);      //等待接收口高电平输出
		OpenTimerForHc1();        //打开定时器
		i = i + 1;
		while (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3)== 1);
		CloseTimerForHc1();        //关闭定时器
		t = GetEchoTimer1();        //获取时间,分辨率为1US
		lengthTemp = ((float)t /58.0);//cm
		sum = lengthTemp + sum;

	}
	lengthTemp = sum / 5.0;
	return (u32)lengthTemp;
}

