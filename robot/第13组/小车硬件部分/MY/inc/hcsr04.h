#ifndef __HSCR04_H
#define __HSCR04_H

#include "stm32f10x.h"
#include "time.h"	
void Hcsr04Init(void);
void TRIG_Send(int i);
int ECHO_Reci(void);
static void OpenTimerForHc(void);
static void CloseTimerForHc(void);
void hcsr04_NVIC(void);
void TIM4_IRQHandler(void);
u32 GetEchoTimer(void);
float Hcsr04GetLength(void ); 

#endif