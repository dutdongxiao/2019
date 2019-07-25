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
#define USART2_REC_LEN  			10  	//�����������ֽ��� 200
void RS232_1_Send_Data(u8 *buf, u8 len);
//������ģ���ʼ��  
extern void Usart2_Init(u32 bound);
//������ģ��һ�ξ����ȡ  
//����0 �ɹ�  
//����1 ʧ��  
extern void USART2_IRQHandler(void);
void RS232_1_Send_Data(u8 *buf, u8 len);
extern void USART2_carry(void);
#endif

