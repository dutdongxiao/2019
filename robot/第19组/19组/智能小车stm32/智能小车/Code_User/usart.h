#ifndef __USART_H  
#define __USART_H  
#include "stm32f10x.h"  
#include "delay.h"  
#include "stm32f10x_tim.h" 
#include "stm32f10x_gpio.h" 
#include "stm32f10x_rcc.h" 
#include "stm32f10x_conf.h"
#include "stm32f10x_usart.h"
#include "hcsr501.h"
#include "hcsr04.h"
#include "hcsr041.h"
#define USART2_REC_LEN  			10  	//定义最大接收字节数 200
void RS232_1_Send_Data(u8 *buf, u8 len);
//超声波模块初始化  
extern void Usart2_Init(u32 bound);
//超声波模块一次距离获取  
//返回0 成功  
//返回1 失败  
extern void USART2_IRQHandler(void);
void RS232_1_Send_Data(u8 *buf, u8 len);
extern void USART2_carry(void);
#endif

