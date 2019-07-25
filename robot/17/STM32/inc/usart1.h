#ifndef __UART1_H
#define __UART1_H
#include <motor.h>
#include <steering_motor.h>
#include <tim4.h>
#include "stm32f10x.h"
#include "time.h"	
#include <echo.h>
#include <alert.h>
#define USART1_REC_LEN  			30  	//定义最大接收字节数 30
void uart1_init(u32 bound);
void RS232_1_Send_Data(u8 *buf,u8 len);
int isspace(int x);
int isdigit(int x);
int atoi(const char *nptr);
#endif
