#ifndef __MOTOR_H
#define __MOTOR_H
#define LEFT turn_left()
#define RIGHT turn_right()
#define FORWARD keep_true()
#define BACKWARD backward()
#define STOP stop()

#include <tim3.h>
#include <ultrasonic.h>
#include "stm32f10x.h"
void motor_init(void);
void TIM3_IRQHandler(void);
void turn_left(void);
void turn_right(void);
void keep_true(void);
void backward(void);
void stop(void);
#endif



