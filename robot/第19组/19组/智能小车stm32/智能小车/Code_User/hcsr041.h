#ifndef __HCSR04_H  
#define __HCSR04_H  
#include "stm32f10x.h"  
#include "delay.h"  
#include "stm32f10x_tim.h" 
#include "stm32f10x_gpio.h" 
#include "stm32f10x_rcc.h" 
#include "stm32f10x_conf.h"
 void Hcsr041Init(void);
 u32 Hcsr041GetLength1(void);

#endif
