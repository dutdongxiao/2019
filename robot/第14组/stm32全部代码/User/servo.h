#ifndef _SERVO_H
#define _SERVO_H
#include "stm32f10x.h"

void servo_init(void);
void servo_angle(u16 angle);
void servo_test(void);


#endif
