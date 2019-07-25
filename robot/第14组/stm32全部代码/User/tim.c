#include  "tim.h"

//TIM2的部分主要用于控制小车车轮所需要的PWM信号
uint16_t capture = 0;
extern __IO uint16_t CCR1_Val;
extern __IO uint16_t CCR2_Val;
extern __IO uint16_t CCR3_Val;
extern __IO uint16_t CCR4_Val;
__IO uint16_t CCR1_Val;
__IO uint16_t CCR2_Val;
__IO uint16_t CCR3_Val;
__IO uint16_t CCR4_Val;

void front(void)
{
	GPIO_SetBits(GPIOA,GPIO_Pin_9);
	GPIO_SetBits(GPIOA,GPIO_Pin_11);
	GPIO_ResetBits(GPIOA,GPIO_Pin_10);
	GPIO_ResetBits(GPIOA,GPIO_Pin_12);
}
void back(void)
{
	GPIO_ResetBits(GPIOA,GPIO_Pin_9);
	GPIO_ResetBits(GPIOA,GPIO_Pin_11);
	GPIO_SetBits(GPIOA,GPIO_Pin_10);
	GPIO_SetBits(GPIOA,GPIO_Pin_12);
}

void init_val(int direction)
{
	if(direction == 1) {front();GPIO_SetBits(GPIOE,GPIO_Pin_15);CCR1_Val=1000;CCR2_Val=1000;CCR3_Val=1000;CCR4_Val=1000;}
	if(direction == 2) {front();GPIO_SetBits(GPIOE,GPIO_Pin_15);CCR1_Val=500;CCR2_Val=1200;CCR3_Val=1200;CCR4_Val=500;}
	if(direction == 3) {front();GPIO_SetBits(GPIOE,GPIO_Pin_15);CCR1_Val=1200;CCR2_Val=500;CCR3_Val=500;CCR4_Val=1200;}
	if(direction == 4) {front();GPIO_SetBits(GPIOE,GPIO_Pin_15);CCR1_Val=500;CCR2_Val=500;CCR3_Val=500;CCR4_Val=500;}
	if(direction == 5) {back();GPIO_SetBits(GPIOE,GPIO_Pin_15);CCR1_Val=1000;CCR2_Val=1000;CCR3_Val=1000;CCR4_Val=1000;}
	if(direction == 6) {front();GPIO_SetBits(GPIOE,GPIO_Pin_15);CCR1_Val=500;CCR2_Val=500;CCR3_Val=500;CCR4_Val=500;}
	if(direction == 7) {front();GPIO_SetBits(GPIOE,GPIO_Pin_15);CCR1_Val=1200;CCR2_Val=1200;CCR3_Val=1200;CCR4_Val=1200;}	
	//if(direction == 8) {roll();GPIO_SetBits(GPIOE,GPIO_Pin_15);CCR1_Val=800;CCR2_Val=800;CCR3_Val=800;CCR4_Val=800;}	
	if(direction == 0) {GPIO_ResetBits(GPIOE,GPIO_Pin_15);front();CCR1_Val=1000;CCR2_Val=1000;CCR3_Val=1000;CCR4_Val=1000;}
	if(direction== 11) {front();GPIO_SetBits(GPIOE,GPIO_Pin_15);CCR1_Val=1000;CCR2_Val=1000;CCR3_Val=500;CCR4_Val=500;}
	if(direction== 10) {front();GPIO_SetBits(GPIOE,GPIO_Pin_15);CCR1_Val=500;CCR2_Val=500;CCR3_Val=1000;CCR4_Val=1000;}
}
void TIM2_Init(void)
{	  
	CCR1_Val=500;
	CCR2_Val=500;
	CCR3_Val=500;
	CCR4_Val=500;
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;	
	TIM_OCInitTypeDef  TIM_OCInitStructure;	
	NVIC_InitTypeDef NVIC_InitStructure;
 
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//子优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器		
	// 开启定时器时钟,即内部时钟CK_INT=72M	

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE); 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	TIM_DeInit(TIM2);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_0|GPIO_Pin_2|GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
		
	TIM_TimeBaseStructure.TIM_Period= 65535;	//周期		
	TIM_TimeBaseStructure.TIM_Prescaler= 35;		//分频
	TIM_TimeBaseStructure.TIM_ClockDivision=0;		
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;	//	向上触发		
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);      //初始化TIM2
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Toggle;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 
	TIM_OCInitStructure.TIM_Pulse = CCR1_Val;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);
	TIM_OC2Init(TIM2, &TIM_OCInitStructure);
	TIM_OC3Init(TIM2, &TIM_OCInitStructure);
	TIM_OC4Init(TIM2, &TIM_OCInitStructure);				//初始化四个通道的参数
	
	TIM_Cmd(TIM2, ENABLE);
  /* TIM IT enable */                //TIM2中断源设置，开启相应通道捕获比较中断
	TIM_ITConfig(TIM2, TIM_IT_CC1 | TIM_IT_CC2 | TIM_IT_CC3 | TIM_IT_CC4, ENABLE);
		 
} 
void TIM2_IRQHandler(void)
{  
	if (TIM_GetITStatus(TIM2, TIM_IT_CC1) != RESET)   			
	{		
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC1 ); 		
    capture = TIM_GetCapture1(TIM2);                            
		TIM_SetCompare1(TIM2, capture + CCR1_Val );       
	} 
	if (TIM_GetITStatus(TIM2, TIM_IT_CC2) != RESET)   			
	{		
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC2 ); 		
    capture = TIM_GetCapture2(TIM2);                            
		TIM_SetCompare2(TIM2, capture + CCR2_Val );       
	}                                                                               
	if (TIM_GetITStatus(TIM2, TIM_IT_CC3) != RESET)   			
	{		
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC3 ); 		
    capture = TIM_GetCapture3(TIM2);                            
		TIM_SetCompare3(TIM2, capture + CCR3_Val );       
	} 
	if (TIM_GetITStatus(TIM2, TIM_IT_CC4) != RESET)   		
	{		
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC4 ); 		
    capture = TIM_GetCapture4(TIM2);                            
		TIM_SetCompare4(TIM2, capture + CCR4_Val );       
	} 	
}


void TIM3_Init(u16 arr,u16 psc)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;	
	TIM_OCInitTypeDef  TIM_OCInitStructure;	
		
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE); 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	TIM_TimeBaseStructure.TIM_Period= arr;	//周期		
	TIM_TimeBaseStructure.TIM_Prescaler= psc;		//分频
	TIM_TimeBaseStructure.TIM_ClockDivision=0;		
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;	//	向上触发		
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);      //初始化TIM3
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 
	TIM_OCInitStructure.TIM_Pulse = 0;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);			//初始化通道参数
	TIM_Cmd(TIM3, ENABLE);
	 
	TIM_CtrlPWMOutputs(TIM3,ENABLE);
}

