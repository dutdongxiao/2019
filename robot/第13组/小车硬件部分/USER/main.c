/*-------------------------------------------------------------------------------
�ļ����ƣ�main.c
�ļ���������������LED��˸��S1ȡ��LED2��S2ȡ��LED3��S3ͬʱȡ��LED2��LED3
          S4ʹLED2��LED3��˸5��
Ӳ��ƽ̨����ĪM3S������
��д����shifang
�̼���  ��V3.5
������̳��www.doflye.net
��    ע��ͨ�����޸Ŀ�����ֲ�����������壬����������Դ�����硣
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
	uint8_t j,cnt; //�������
	LED_Init();
	RUN_Init();
	KEY_Init();
	TIM3_Toggle_Init();
	TIM3_Init();
	SysTick_Init();//��ʱ��ʼ��
	Hcsr04Init();  //TIM4������ų�ʼ��
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
//  Stop();        //��ʼ״̬ʹС��ֹͣ	
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

/*----------------------�·��� ������̳��www.doflye.net--------------------------*/

