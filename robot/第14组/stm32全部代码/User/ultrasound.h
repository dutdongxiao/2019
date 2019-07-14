#ifndef _ULTRASOUND_H
#define _ULTRASOUND_H
#include "stm32f10x.h"


void RCC_cfg(void);
void TIMER_cfg(void);
void NVIC_cfg(void);
void EXTI_cfg(void);
void EXTI1_IRQHandler(void);
void ultrasound_init(void);
void ultrasound_start(void);
u16 get_time_ultrasound(void);


#endif
