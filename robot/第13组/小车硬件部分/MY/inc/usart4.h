#ifndef __UART4_H
#define __UART4_H

#include "stm32f10x.h"
#include "string.h"
#include "time.h"	
#define UART4_REC_LEN  			200  	//定义最大接收字节数 200
void uart4_init(u32 bound);
void RS232_4_Send_Data(u8 *buf,u8 len);
int compare_4(u8* buf,const char * str);
void judge_data_4(u8 *buf,u8 len);
void judge(int flag);
void Delay(u8 len);
#endif
/*----------------------德飞莱 技术论坛：www.doflye.net--------------------------*/
