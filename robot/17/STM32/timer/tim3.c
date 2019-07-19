#include "TIM3.h"
#include "led.h"

extern int r_speed;

void TIM3_PWM_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	/* TIM3的PWM1和PWM2通道对应引脚是PA67PB01*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}
void TIM3_Init(u16 arr,u16 psc,u16 cmp)
{	  
	/*--------------------时基结构体初始化-------------------------*/	
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;	
	TIM_OCInitTypeDef  TIM_OCInitStructure;	// 配置为PWM模式	
	NVIC_InitTypeDef NVIC_InitStructure;
	
	// 开启定时器时钟,即内部时钟CK_INT=72M	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE); 
	TIM_TimeBaseStructure.TIM_Period= arr;	//周期	
	TIM_TimeBaseStructure.TIM_Prescaler= psc;		//分频
	TIM_TimeBaseStructure.TIM_ClockDivision=0;		
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;	//	向上触发		
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); 
  
	
	//设置中断
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//子优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
	
	//TIM_ITConfig(TIM3,TIM_IT_CC1,ENABLE);
	
	//设置比较寄存器
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
	TIM_OCInitStructure.TIM_Pulse = cmp;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//比较输出使能
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);
	TIM_OC3Init(TIM3, &TIM_OCInitStructure);
	TIM_OC4Init(TIM3, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Disable);//失能TIMx在CCR1上的预装载寄存器
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Disable);//失能TIMx在CCR1上的预装载寄存器
	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Disable);//失能TIMx在CCR1上的预装载寄存器
	TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Disable);//失能TIMx在CCR1上的预装载寄存器
	
	//TIM_ITConfig(TIM3, TIM_IT_CC1|TIM_IT_CC2|TIM_IT_CC3|TIM_IT_CC4 , ENABLE);
	TIM_Cmd(TIM3, ENABLE);
	//TIM_SetCompare1(TIM3,10000); 	 
} 



void LedPwmCtrl(uint16_t PWM)
{
	//uint16_t Capture1;
	//Capture1 = TIM_GetCapture1(TIM3);
    TIM_SetCompare1(TIM3, PWM);//设置TIMx自动重装载寄存器值
}

