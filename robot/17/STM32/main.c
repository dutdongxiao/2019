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
#include "delay.h"
#include "tim2.h"
#include "tim3.h"
#include <tim4.h>
#include "led.h"
#include <motor.h>
#include <steering_motor.h>
#include <usart1.h>
#include <ultrasonic.h>
#include <echo.h>
#include <alert.h>


int l_speed = 199;
int r_speed = 199;
uint16_t time_ = 0;
int flag_stop = 0;//检测到障碍以后的停止标志,0代表没有障碍
//u16 last_pos_nod = 50;//俯仰舵机上一次位置
extern int steer_lr;
int steer_flag_up = 0;//向上使能
int steer_flag_down = 0;//向下使能
int steer_flag_right = 0;//向右使能
int steer_flag_left = 0;//向左使能



extern int command_flag;
extern u8 command_ptr;



int main(void)
{
	//GPIO_InitTypeDef GPIO_InitStructure;
	//char *buf = "r$";
	//int i = 0;
	//char *buff = "s$";
	//char *mes = "l$";
	SysTick_Init();//延时初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);//中断优先级分组
	LED_Init();
	uart1_init(115200);
	TIM4_Init(65535,35999);//分辨率为0.5ms
	motor_init();
	ultrasonic_init();
	alert_init();
	LED2_OFF;
	steering_motor_init();
	
	//TIM_Cmd(TIM4,ENABLE);
	
	//Delay_ms(1000);
	//left();
  //Delay_ms(3000);
  //Set_Num_Angle_smooth(1,0x03ff);
	
	//s_stop();
	
	//Set_Num_Angle(2,90);
	
	while(1)
	{
		alert();
		//trailing();
		//Delay_ms(65535);
		//time__ = TIM4->CNT;
		//Set_Num_Angle(0);
		up();
		down();
		//RS232_1_Send_Data(mes,2);
		left();
		//Delay_ms(30);
		//RS232_1_Send_Data(buf,2);
		//Set_Num_Angle(300);
    //Delay_ms(300);
		right();
		//if(steer_lr > 160)
		//{
		 // RS232_1_Send_Data(buff,2);
	   // Delay_ms(500);
		//}
		ultrasonic_trig();
		Delay_ms(70);
		//LED2_OFF;
	}
}


