#ifndef __TIM3_H
#define __TIM3_H


#include "stm32f10x.h"	 

void TIM3_Init(u16 arr,u16 psc,u16 cmp);
void LedPwmCtrl(uint16_t PWM1);
void TIM3_PWM_Init(void);
void TIM3_IRQHandler(void);
#endif
