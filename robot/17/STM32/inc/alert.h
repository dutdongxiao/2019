#ifndef __ALERT_H
#define __ALERT_H
#include "stm32f10x.h"
#include <delay.h>
#include <tim2.h>
#include <usart1.h>
void alert_init(void);
void alert(void);
void alert_on(void);
void alert_off(void);
void alert_setoff(void);
#endif