#ifndef __UART2_H
#define __UART2_H
#include <steering_motor.h>
#include "stm32f10x.h"
#include "time.h"	
#define USART2_REC_LEN  			200  	//定义最大接收字节数 200
void uart2_init(void);
void RS232_2_Send_Data(u8 *buf,u8 len);
#endif

