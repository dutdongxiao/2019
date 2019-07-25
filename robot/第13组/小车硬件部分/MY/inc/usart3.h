#ifndef __UART3_H
#define __UART3_H

#include "stm32f10x.h"
#include "time.h"	
#define USART3_REC_LEN  			200  	//定义最大接收字节数 200
void uart3_init(u32 bound);
void RS232_1_Send_Data(u8 *buf,u8 len);
int compare(u8* buf,const char * str);
void judge_data(u8 *buf,u8 len);
#endif
/*----------------------德飞莱 技术论坛：www.doflye.net--------------------------*/
