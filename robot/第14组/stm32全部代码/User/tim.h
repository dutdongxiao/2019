#ifndef _TIM_H
#define _TIM_H
#include "stm32f10x.h"

void init_val(int direction);
void TIM2_Init(void);
void TIM2_IRQHandler(void);
void TIM3_Init(u16 arr,u16 psc);
void tim1_pwmInit(uint16_t arr, uint16_t psc);

#endif
