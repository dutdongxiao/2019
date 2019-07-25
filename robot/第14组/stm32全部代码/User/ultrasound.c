#include "ultrasound.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "delay.h"
#include "misc.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_flash.h"
#include "stm32f10x_tim.h"

#define SONAR_PORT  GPIOE
#define TRIG_PIN    GPIO_Pin_3
#define ECHO_PIN    GPIO_Pin_1
//int x=0;
//u16 time_count=0;

void RCC_cfg()
{
	ErrorStatus HSEStartUpStatus;
	RCC_DeInit();
	RCC_HSEConfig(RCC_HSE_ON);
	HSEStartUpStatus = RCC_WaitForHSEStartUp();
	if(HSEStartUpStatus == SUCCESS)
	{
		RCC_HCLKConfig(RCC_SYSCLK_Div1);
		RCC_PCLK1Config(RCC_HCLK_Div2);
		FLASH_SetLatency(FLASH_Latency_2);
		FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
		RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
		RCC_PLLCmd(ENABLE);
		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
		while(RCC_GetSYSCLKSource() != 0x08);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO,ENABLE);
	}
}
void TIMER_cfg()
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_DeInit(TIM5);
	TIM_InternalClockConfig(TIM5);
	TIM_TimeBaseStructure.TIM_Prescaler = 72-1;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = 65535;
	TIM_TimeBaseInit(TIM5,&TIM_TimeBaseStructure);
	TIM_ARRPreloadConfig(TIM5, DISABLE);
}
void NVIC_cfg()
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
void EXTI_cfg()
{
	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_ClearITPendingBit(EXTI_Line1);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource1);
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
}

void ultrasound_init(void)
{
	RCC_cfg();
	NVIC_cfg();
	EXTI_cfg();
	TIMER_cfg();
	
  GPIO_InitTypeDef GPIO_Initstructure;
	
	GPIO_Initstructure.GPIO_Pin = ECHO_PIN;
	GPIO_Initstructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Initstructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(SONAR_PORT, &GPIO_Initstructure);


	GPIO_Initstructure.GPIO_Pin = TRIG_PIN;
	GPIO_Initstructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Initstructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(SONAR_PORT, &GPIO_Initstructure);


	
	GPIO_ResetBits(GPIOE,TRIG_PIN);
	//delay_ms(1000);
}
/*void ultrasound_start(void)
{
}
u16 get_time_ultrasound(void)
{
	return time_count;
}
*/
