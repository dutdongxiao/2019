#ifndef __ULTRASONIC_H
#define __ULTRASONIC_H
#include <delay.h>
#include <led.h>
#include <motor.h>
#include <usart1.h>
#include "stm32f10x.h"
void TIM5_Init(void);//һ����1us
void ultrasonic_trig(void);
void ultrasonic_init(void);
#endif
