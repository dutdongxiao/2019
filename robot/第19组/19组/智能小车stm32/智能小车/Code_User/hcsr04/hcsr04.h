#ifndef __HCSR041_H  
#define __HCSR041_H  
#include "stm32f10x.h"  
#include "delay.h"  
#include "stm32f10x_tim.h" 
#include "stm32f10x_gpio.h" 
#include "stm32f10x_rcc.h" 
#include "stm32f10x_conf.h"
//³¬Éù²¨Ä£¿é³õÊ¼»¯  
void Hcsr04Init(void);

u32 Hcsr04GetLength1(void);

#endif
