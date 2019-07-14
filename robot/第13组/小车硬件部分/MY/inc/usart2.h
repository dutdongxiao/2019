#ifndef __USART2_H
#define __USART2_H

#include "stm32f10x.h"

void uart2_init(u32 bound);
void Set_Num_Angel(u8 id,u32 arg);
void USART2_Send_Char(uint8_t data);
void shake(void);
void node(void);
void judge_angel(u8 flag);
#endif