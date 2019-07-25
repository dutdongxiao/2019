#include "key.h"
#include "delay.h"
 
void key_init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
	
}
 
u8 key_scan(GPIO_TypeDef* GPIOx, u16 GPIO_Pin)
{
	if(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == KEY_ON)
	{
		delay_ms(50);
		if(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == KEY_ON)
		{
			while(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == KEY_ON);
			return KEY_ON;
		}
		else
			return KEY_OFF;
	}
	else
		return KEY_OFF;
}

