#include <ultrasonic.h>

extern int flag_stop;//检测到障碍以后的停止标志,0代表没有障碍
extern uint16_t time_ ;//存放高电平时间

void TIM5_Init(void)//一周期1us
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE); 
	TIM_TimeBaseStructure.TIM_Period= 65535;	//周期	
	TIM_TimeBaseStructure.TIM_Prescaler= 71;		//分频
	TIM_TimeBaseStructure.TIM_ClockDivision=0;		
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;	//	向上触发		
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure); 
	TIM_Cmd(TIM5,DISABLE);
}

void ultrasonic_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	
	TIM5_Init();
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);//启用复用端口
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);//启用PD3为trigger，PD4为外部中断输入
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	GPIO_ResetBits(GPIOD,GPIO_Pin_3);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOD,GPIO_PinSource4);
	EXTI_ClearITPendingBit(EXTI_Line4);//清除中断标志位
	EXTI_InitStructure.EXTI_Line = EXTI_Line4;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_Init(&EXTI_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
}

void ultrasonic_trig(void)
{
	GPIO_SetBits(GPIOD,GPIO_Pin_3);
  Delay_us(11);
	GPIO_ResetBits(GPIOD,GPIO_Pin_3);
}

void EXTI4_IRQHandler(void)
{
	//uint16_t time = 0;//存放高电平时间
	char *mes = "Obstacles detected\n";
	char *mes_ = "Obstacles removed\n";
	if(EXTI_GetITStatus(EXTI_Line4) != RESET)
	{
	  LED2_ON;
	  TIM_Cmd(TIM5,ENABLE);
	  TIM5->CNT = 0;
	  while(1)//循环检测
	  {
	    if(GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_4) == RESET)//如果高电平结束
	    {
	  		time_ = TIM_GetCounter(TIM5);
	  	  if(time_/2 < 600)//如果距离小于20cm
	  		{
		  		LED2_OFF;
	  			STOP;
					if(flag_stop == 0)
					{
						RS232_1_Send_Data((u8 *)mes,19);
					}
					flag_stop = 1;//前方检测到障碍
		  		time_ = 0;
		  	}
				else
				{
					if(flag_stop == 1)
					{
						RS232_1_Send_Data((u8 *)mes_,18);
					}
					flag_stop = 0;//没有障碍
				}
	  		break;
	    }
	  }
	  TIM_Cmd(TIM5,DISABLE);
	  EXTI_ClearITPendingBit(EXTI_Line4);//清除中断标志位
  }
}
