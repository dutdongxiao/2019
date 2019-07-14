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
#include "key.h"
#include "usart1.h"
#include "gpio.h" 
#include "tim.h"
int main(void)
{
	
  u8 a,b,c,d,e,f,g,h,i,j;//定义变量 
	int x,y;
  LED_Init();//LED初始化
	GPI_Init();
  SysTick_Init();//延时初始化
	TIM_SetCompare1(TIM3,200);
	//TIM3_PWM_Init(500,83);
	TIM2_PWM_Init();
	GPIO_ResetBits(GPIOD,GPIO_Pin_1);//STOP
	//LED2_REV;
	TIM_SetCompare1(TIM2,85);  //xt
  TIM_SetCompare1(TIM4,70);//xt

	x=85;y=70;//xt
while (1)
  {
		
		  if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_1)==1)    //  右
		{
			if(x>50)
			{
			x--;
			TIM_SetCompare1(TIM2,x);
			}
		}
if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2)==1)  //   左
		{
			if(x<120)
			{
			x++;
			TIM_SetCompare1(TIM2,x);
			}
		}
if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3)==1)  //   上
		{
			if(y<85)
			{
			y++;
			TIM_SetCompare1(TIM4,y);
			}
		}
if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4)==1)  //  下 
		{
			if(y>60)
			{
			y--;
			TIM_SetCompare1(TIM4,y);
			}
		}
	a=GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_4);
	b=GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_5);
	c=GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_6);
	d=GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_7); 
	e=GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_2);
	f=GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_3);
  Delay_ms (100);
	//FROM FRONT A,C in left ,B,D in right   
	//	A   B
	//  C   D
	//B,D=0, A,C=1 BACK ; A,C=0, B,D=1 FRONT ; A,B,C,D=1 ,left; A,B,C,D=0,right
	//14-D,13-C,12-B,11-A
	
	
	
	//the car
	if(a==0&&b==0&&c==0&&d==0)
	{
		GPIO_ResetBits(GPIOD,GPIO_Pin_1);
		//Delay_ms (100);
		
	}
	if(a==0&&b==0&&c==1&&d==0&&e==0&&f==0)//left
	{ 
		TIM3_PWM_Init(500,83);
		GPIO_SetBits(GPIOD,GPIO_Pin_1);
	//	Delay_ms (100);
		GPIO_SetBits(GPIOD,GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14);	
	//	Delay_ms(100);
	}
	if(a==0&&b==0&&c==0&&d==1&&e==0&&f==0)//right
	{
	  TIM3_PWM_Init(500,83);
		GPIO_SetBits(GPIOD,GPIO_Pin_1);
	//	Delay_ms (100);
		GPIO_ResetBits(GPIOD,GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14);	
	//	Delay_ms(100);
	}
	
	if(a==1&&b==0&&c==0&&d==0&&e==0&&f==0)//front
	{ 
	  
		TIM3_PWM_Init(500,83);
		GPIO_SetBits(GPIOD,GPIO_Pin_1);
	//	Delay_ms (100);
		GPIO_SetBits(GPIOD,GPIO_Pin_12|GPIO_Pin_14);
		GPIO_ResetBits(GPIOD,GPIO_Pin_11|GPIO_Pin_13);	
	//	Delay_ms (100);
	}
	if(a==0&&b==1&&c==0&&d==0&&e==0&&f==0)//back
	{ 
	  TIM3_PWM_Init(500,83);
		GPIO_SetBits(GPIOD,GPIO_Pin_1);
	//	Delay_ms (100);
		GPIO_SetBits(GPIOD,GPIO_Pin_11|GPIO_Pin_13);
		GPIO_ResetBits(GPIOD,GPIO_Pin_12|GPIO_Pin_14);
	//	Delay_ms(100);		
	}
	
	
	//low
	if(a==0&&b==0&&c==1&&d==0&&e==1&&f==0)//left
	{ 
	 
		TIM3_PWM_Init(500,200);	
		GPIO_SetBits(GPIOD,GPIO_Pin_1);
	//	Delay_ms (100);
		GPIO_SetBits(GPIOD,GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14);	
	//	Delay_ms(100);
	}
	if(a==0&&b==0&&c==0&&d==1&&e==1&&f==0)//right
	{
	  
		TIM3_PWM_Init(500,200);	
		GPIO_SetBits(GPIOD,GPIO_Pin_1);
//		Delay_ms (100);
		GPIO_ResetBits(GPIOD,GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14);	
//		Delay_ms(100);
	}
	if(a==1&&b==0&&c==0&&d==0&&e==1&&f==0)//front
	{ 
	  
		TIM3_PWM_Init(500,200);	
		GPIO_SetBits(GPIOD,GPIO_Pin_1);
	//	Delay_ms (100);
		GPIO_SetBits(GPIOD,GPIO_Pin_12|GPIO_Pin_14);
		GPIO_ResetBits(GPIOD,GPIO_Pin_11|GPIO_Pin_13);	
	//	Delay_ms(100);
	}
	if(a==0&&b==1&&c==0&&d==0&&e==1&&f==0)//back
	{ 
	  TIM3_PWM_Init(500,200);	
		GPIO_SetBits(GPIOD,GPIO_Pin_1);
	//	Delay_ms (100);
		GPIO_SetBits(GPIOD,GPIO_Pin_11|GPIO_Pin_13);
		GPIO_ResetBits(GPIOD,GPIO_Pin_12|GPIO_Pin_14);
	//	Delay_ms(100);		
	}
	
	
	
	
	//fast
	if(a==0&&b==0&&c==1&&d==0&&e==0&&f==1)//left
	{ 
	 
		TIM3_PWM_Init(500,20)	;	
		GPIO_SetBits(GPIOD,GPIO_Pin_1);
	//	Delay_ms (100);
		GPIO_SetBits(GPIOD,GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14);	
	//	Delay_ms(100);
	}
	if(a==0&&b==0&&c==0&&d==1&&e==0&&f==1)//right
	{
	  
		TIM3_PWM_Init(500,20)	;	
		GPIO_SetBits(GPIOD,GPIO_Pin_1);
	//	Delay_ms (100);
		GPIO_ResetBits(GPIOD,GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14);	
	//	Delay_ms(100);
	}
	if(a==1&&b==0&&c==0&&d==0&&e==0&&f==1)//front
	{ 
	  
		TIM3_PWM_Init(500,20)	;	
		GPIO_SetBits(GPIOD,GPIO_Pin_1);
	//	Delay_ms (100);
		GPIO_SetBits(GPIOD,GPIO_Pin_12|GPIO_Pin_14);
		GPIO_ResetBits(GPIOD,GPIO_Pin_11|GPIO_Pin_13);	
	//	Delay_ms(100);
	}
	if(a==0&&b==1&&c==0&&d==0&&e==0&&f==1)//back
	{ 
	  TIM3_PWM_Init(500,20)	;	
		GPIO_SetBits(GPIOD,GPIO_Pin_1);
	//	Delay_ms (100);
		GPIO_SetBits(GPIOD,GPIO_Pin_11|GPIO_Pin_13);
		GPIO_ResetBits(GPIOD,GPIO_Pin_12|GPIO_Pin_14);
	//	Delay_ms(100);		
	}
	
	/*
	if(e==0&&f==0)//mid
	{ 
		TIM3_PWM_Init(500,83)	;	
		Delay_ms (50);
	}
	if(e==1&&f==0)//low
	{ 
		TIM3_PWM_Init(500,200)	;	
		Delay_ms (50);
	}
	if(e==0&&f==1)//fast
	{ 
		TIM3_PWM_Init(500,20)	;	
		Delay_ms (50);
	}*/
	
	
	
	//xt


  }
}

/*----------------------德飞莱 技术论坛：www.doflye.net--------------------------*/
