#include "stm32f10x_tim.h"
#include "stm32f10x.h"
#include "delay.h"
#include "tim.h"
#include "key.h"
#include "init.h" 
#include "servo.h"
#include  "ultrasound.h"
#include "stm32f10x_usart.h"

#define straight 1
#define right 2
#define left 3
#define fast 4
#define back 5
#define faster 6
#define slower 7
#define roll 8
#define stop 0
#define ll 10
#define rr 11
#define SONAR_PORT  GPIOE
#define TRIG_PIN    GPIO_Pin_3
#define ECHO_PIN    GPIO_Pin_1

u8 Res=10;
u16 time_ultrasound;
u16 time_count;
int voice_flag=0;
int x=0;
void decoder_open(u8 Res);
void decoder(u8 Res);
void start_voice(void);

int main(void)
 {    
	 /* RCC_cfg();                      
		Usart3_Init(9600);
	  led_init();
	  servo_init();
	  delay_init();	
		while(1)
	 {
		 decoder_open(Res);		 
	 }*/
	

		RCC_cfg();
	  Usart3_Init(9600);
	 
	  led_init();
	  delay_init();	
	  key_init();
	  drive_init();
		init_val(straight);
	  GPIO_ResetBits(GPIOE,GPIO_Pin_15);
	  ultrasound_init();
		GPIO_ResetBits(GPIOE,TRIG_PIN);
		while(1)
		{
			if(key_scan(GPIOE,GPIO_Pin_4) == KEY_ON)
			{
				break;
			}
			
		}
		voice_flag=1;
		while(1)
		{
			time_ultrasound=get_time_ultrasound();
			if(voice_flag==1)
			{
				if(time_ultrasound<=1800) 
				{
					init_val(stop);
				}
				else if(time_ultrasound>1800 && time_ultrasound<4200)
					init_val(straight);
				else if(time_ultrasound>4200)
					init_val(fast);
				GPIO_SetBits(GPIOE,TRIG_PIN);
				delay_us (10);
				GPIO_ResetBits(GPIOE,TRIG_PIN);
				delay_ms(50);	
			}
			if(time_ultrasound<=1800) 
			{
				init_val(stop);
				continue;
			}
			decoder(Res);
		}
		
		
	
 }
void EXTI1_IRQHandler(void)
{
	
	x++;
	x=x%2;
	if(x==0)
	{
		time_count=TIM5->CNT;
		TIM_SetCounter(TIM5,0);
		TIM_Cmd(TIM5,DISABLE);	
	}
	else
	{
		TIM_Cmd(TIM5,ENABLE);
	}
	EXTI_ClearITPendingBit(EXTI_Line1);
}


void USART3_IRQHandler(void)
		{ 
    if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
			{  
        Res =USART_ReceiveData(USART3);				
			}
}
u16 get_time_ultrasound(void)
{
	return time_count;
}

void decoder(u8 Res)
{
	if(Res=='A')
		init_val(straight);
	else if(Res=='R')
		init_val(right);
	else if(Res=='L')
	  init_val(left);
  else if(Res=='S')
		{init_val(stop);voice_flag=0;}
	else if(Res=='B')
		init_val(back);
	else if(Res=='P')
		{init_val(faster);}
	else if(Res=='M')
		{init_val(slower);}
	else if(Res=='V')
		{
			voice_flag=1;
			GPIO_SetBits(GPIOE,TRIG_PIN);
			delay_us (10);
			GPIO_ResetBits(GPIOE,TRIG_PIN);
		}
	else if(Res=='T')
	{
		init_val(right);
		voice_flag=0;
	}
	else if(Res=='t')
	{
		init_val(left);
		voice_flag=0;
	}
	else if(Res=='l')
	{
		init_val(ll);
	  voice_flag=0;
	}
	else if(Res=='r')
	{
		init_val(rr);
		voice_flag=0;
	}
}
void decoder_open(u8 Res)
{
	if(Res=='Y')
	{
		servo_angle(90);
		GPIO_SetBits(GPIOE,GPIO_Pin_5);
	}
	else if(Res=='X')
	{
		GPIO_ResetBits(GPIOE,GPIO_Pin_5);
		servo_angle(1);
	}
}
