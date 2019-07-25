#include <ultrasonic.h>

extern int flag_stop;//��⵽�ϰ��Ժ��ֹͣ��־,0����û���ϰ�
extern uint16_t time_ ;//��Ÿߵ�ƽʱ��

void TIM5_Init(void)//һ����1us
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE); 
	TIM_TimeBaseStructure.TIM_Period= 65535;	//����	
	TIM_TimeBaseStructure.TIM_Prescaler= 71;		//��Ƶ
	TIM_TimeBaseStructure.TIM_ClockDivision=0;		
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;	//	���ϴ���		
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure); 
	TIM_Cmd(TIM5,DISABLE);
}

void ultrasonic_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	
	TIM5_Init();
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);//���ø��ö˿�
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);//����PD3Ϊtrigger��PD4Ϊ�ⲿ�ж�����
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
	EXTI_ClearITPendingBit(EXTI_Line4);//����жϱ�־λ
	EXTI_InitStructure.EXTI_Line = EXTI_Line4;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_Init(&EXTI_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
}

void ultrasonic_trig(void)
{
	GPIO_SetBits(GPIOD,GPIO_Pin_3);
  Delay_us(11);
	GPIO_ResetBits(GPIOD,GPIO_Pin_3);
}

void EXTI4_IRQHandler(void)
{
	//uint16_t time = 0;//��Ÿߵ�ƽʱ��
	char *mes = "Obstacles detected\n";
	char *mes_ = "Obstacles removed\n";
	if(EXTI_GetITStatus(EXTI_Line4) != RESET)
	{
	  LED2_ON;
	  TIM_Cmd(TIM5,ENABLE);
	  TIM5->CNT = 0;
	  while(1)//ѭ�����
	  {
	    if(GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_4) == RESET)//����ߵ�ƽ����
	    {
	  		time_ = TIM_GetCounter(TIM5);
	  	  if(time_/2 < 600)//�������С��20cm
	  		{
		  		LED2_OFF;
	  			STOP;
					if(flag_stop == 0)
					{
						RS232_1_Send_Data((u8 *)mes,19);
					}
					flag_stop = 1;//ǰ����⵽�ϰ�
		  		time_ = 0;
		  	}
				else
				{
					if(flag_stop == 1)
					{
						RS232_1_Send_Data((u8 *)mes_,18);
					}
					flag_stop = 0;//û���ϰ�
				}
	  		break;
	    }
	  }
	  TIM_Cmd(TIM5,DISABLE);
	  EXTI_ClearITPendingBit(EXTI_Line4);//����жϱ�־λ
  }
}
