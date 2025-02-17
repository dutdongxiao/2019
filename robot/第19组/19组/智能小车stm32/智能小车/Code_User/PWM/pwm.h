#ifndef __PWM_H
#define __PWM_H


#include "stm32f10x.h"


//------------------------------------------------------------------------------------------------
// TIMx_IT_Update_time = (TIMx_Reload_Num + 1) * ( TIMx_Frequency_Divide + 1 ) / TIMx_input_CLK
//   TIMx溢出时间   		 		重装载值						分频系数				 TIMx的输入时钟
//------------------------------------------------------------------------------------------------


// 溢出时间：100us
//--------------------------------
// 分频系数：720（分频值+1）
// 计数频率：100kHz
// 计数周期：10us
// 重装载值：99
// 溢出周期：1ms
// 溢出频率：1kHz
//--------------------------------------------------------------
#define	TIM1_Frequency_Divide	719		// TIM1时钟预分频值
#define	TIM1_Reload_Num			99		// 自动重装载寄存器的值

#define	TIM4_Frequency_Divide	719		// TIM1时钟预分频值
#define	TIM4_Reload_Num			99		// 自动重装载寄存器的值
//--------------------------------------------------------------


void TIM1_PWM_Init_JX(void) ;	// TIM1_PWM输出初始化 

void TIM4_PWM_Init_JX(void);	// TIM4_PWM输出初始化 


#endif /* __PWM_H */
