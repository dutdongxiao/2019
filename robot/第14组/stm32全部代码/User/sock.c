#include <stm32f10x.h>
#include <stm32f10x_gpio.h>
#include "stm32f10x_usart.h"
#include "delay.h"
#include "stm32f10x_rcc.h"
#include "misc.h"
#include "stdio.h"
#include "stm32f10x_flash.h"

void Usart2_Init(u32 bound){
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
	 
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOE, ENABLE);	
  
  //USART2_TX   GPIOA.2
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	
  GPIO_Init(GPIOA, &GPIO_InitStructure);
   
  //USART2_RX	  GPIOA.3
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure); 
 
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			
  NVIC_Init(&NVIC_InitStructure);	
  

  USART_InitStructure.USART_BaudRate = bound;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	
 
  USART_Init(USART2, &USART_InitStructure); 
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
  USART_Cmd(USART2, ENABLE);                    
 
}


void USART2_IRQHandler(void)
		{ 
    u8 Res;
    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
			{  
        Res =USART_ReceiveData(USART2);				
				if(Res==0x55)
				{
					GPIO_SetBits(GPIOE,GPIO_Pin_5);
				}
				else
					GPIO_ResetBits(GPIOE,GPIO_Pin_5);				
			}
}

