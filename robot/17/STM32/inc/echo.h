#ifndef __ECHO_H
#define __ECHO_H

#include "stm32f10x.h"
#include <motor.h>
#include <tim4.h>
#include <usart1.h>
#include <led.h>
void trailing(void);
void echo_record(char dir);
#endif
