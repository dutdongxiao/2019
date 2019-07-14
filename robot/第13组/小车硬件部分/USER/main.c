/*-------------------------------------------------------------------------------
文件名称：main.c
文件描述：按键控制LED闪烁，S1取反LED2，S2取反LED3，S3同时取反LED2，LED3
          S4使LED2，LED3闪烁5次
硬件平台：尼莫M3S开发板
编写整理：shifang
固件库  ：V3.5
技术论坛：www.doflye.net
备    注：通过简单修改可以移植到其他开发板，部分资料来源于网络。
---------------------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "led.h"
#include "delay.h"
#include "tim3.h"
#include "key.h"
#include "usart2.h"
#include "usart3.h"
#include "usart4.h"
#include "hcsr04.h"
int main(void)
{	
	u32 i,k;
	uint8_t j,cnt; //定义变量
	LED_Init();
	RUN_Init();
	KEY_Init();
	TIM3_Toggle_Init();
	TIM3_Init();
	SysTick_Init();//延时初始化
	Hcsr04Init();  //TIM4相关引脚初始化
  uart3_init(115200);
  uart2_init(1000000);
  uart4_init(9600);
	extern int flag_heart;
	extern int flag_one;
	extern int flag_two;
  extern int flag_three;
  extern int flag_four ;
	extern int flag_five ;
	extern int flag_shake;
	extern int flag_node;
	LED2_OFF;
//  Stop();        //初始状态使小车停止	
	while(1)
	{
			if(!S1)
			{
				Delay_ms(10);
				if(!S1)
				{
					while(!S1);
          State_Rev();
				}
			}
			if(flag_heart == 1)
			{ 
				LED2_REV;
				Heart();
				flag_heart =0;
			}
			if(flag_one == 1)
			{
				One();
				flag_one = 0;
			}
			if(flag_shake == 1)
			{
				shake();
				flag_shake = 0;
			}
			if(flag_node == 1)
			{
				LED2_REV;
				node();
				flag_node = 0;
			}
			if(flag_two == 1)
			{
				Two();
				flag_two = 0;
			}				
			if(flag_three == 1)
			{
				Three();
	   		flag_three = 0;
			}
			if(flag_four == 1)
			{
				Four();
				flag_four = 0;
			}
			/*
			if(Hcsr04GetLength() <= 50)
 				LED2_ON;
      else
				LED2_OFF;*/
			Delay_ms(100);
	}
}

/*----------------------德飞莱 技术论坛：www.doflye.net--------------------------*/

