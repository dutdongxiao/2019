#ifndef __TIM2_H
#define __TIM2_H

#include "stm32f10x.h"	 
  /**************************************************************************
作者：平衡小车之家
我的淘宝小店：http://shop114407458.taobao.com/
**************************************************************************/
void TIM3_Init(void);
void LedToggleCtrl(uint8_t PWM1);
void TIM3_Toggle_Init(void);
void TIM3_IRHandler(void);
void Run(void);
void Open(void);
void Stop(void);
void State_Rev(void);
void Round(void);
void Round_1 (void);
void Back(void);
void Left(void);
void Right(void);
void Q_shift(void);
void E_shift(void);
void Heart(void);
void One(void);
void Two(void);
void Three(void);
void Four(void);
void Five(void);
void RUN_Init(void);
#endif
