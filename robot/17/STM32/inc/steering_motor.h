#ifndef __STEERING_MOTOR_H
#define __STEERING_MOTOR_H
#include <tim2.h>
#include <tim4.h>
#include <delay.h>
#include "stm32f10x.h"

void steering_motor_init(void);
void down(void);
void up(void);
void left(void);
void right(void);
void s_stop(void);
void Set_Num_Angle(u8 id,u32 arg);
void Set_Num_Angle_smooth(u8 id,u32 arg);
#endif
