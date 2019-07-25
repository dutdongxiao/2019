#ifndef __HC_SR501_H__
#define	__HC_SR501_H__
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "delay.h"


#define			HC_SR501			GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0 )
#define			HC_SR501_Pin		GPIO_Pin_0
#define    	 	HC_SR50PORT         GPIOA
#define     	HC_SR50CLKLINE      RCC_APB2Periph_GPIOA


extern void HC_SR501Configuration(void);
extern u16 HC_SR501_Status(void);
#endif
