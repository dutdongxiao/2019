
#ifndef __KEY_H
#define __KEY_H

#include "stm32f10x.h"

#define S1 GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4)
void KEY_Init(void);

#endif
/*----------------------德飞莱 技术论坛：www.doflye.net--------------------------*/
