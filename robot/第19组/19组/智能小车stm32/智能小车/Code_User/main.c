
#include "stm32f10x.h"
#include "bit_band.h"
#include "delay.h"
#include "timer.h"
#include "pwm.h"
#include "MotorDrive.h"
#include "hcsr04.h" 
#include "stm32f10x_usart.h"
#include "stm32f10x_tim.h" 
#include "stm32f10x_gpio.h" 
#include "stm32f10x_rcc.h" 
#include "misc.h" 
#include "stm32f10x_conf.h" 
#include "hcsr501.h"
#include "usart.h"
#include "hcsr041.h" 
int main(void)
{	
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 	 //NVIC����2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	delay_Init();				// ��ʱ��ʼ��
	TIM3_Time_Init();			// ��ʱ��3��ʼ��
	TIM1_PWM_Init_JX();			// TIM1_PWM��ʼ��
	TIM4_PWM_Init_JX();			// TIM4_PWM��ʼ��
  Hcsr04Init();           //����������ʼ��
	Hcsr041Init();
	HC_SR501Configuration();
  Usart2_Init(115200);
	
	while(1)
	{
	
	
		USART2_carry();
}
}
