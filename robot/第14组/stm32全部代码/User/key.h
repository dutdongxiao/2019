#ifndef __KEY__H
#define __KEY__H
//#include "sys.h"
#include "stm32f10x.h"
 
#define S1  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4)

 
#define KEY_ON 0
#define KEY_OFF 1
 
void key_init(void);
u8 key_scan(GPIO_TypeDef* GPIOx, u16 GPIO_Pin);
 
#endif
