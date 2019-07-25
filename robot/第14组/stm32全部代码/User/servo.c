#include "servo.h"
#include "tim.h"
#include "delay.h"

void servo_init(void)
{
 TIM3_Init(1999,719);
	servo_angle(1);
 //TIM_SetCompare1(TIM3,150);
}

void servo_angle(u16 angle)
{
 u16 pulse;
 if(angle<1) angle=1;
 if(angle>90) angle=90;
  pulse=(u16)(50+angle*100/90.0);
	TIM_SetCompare1(TIM3,pulse);
}

void servo_test(void)
{
	delay_init();
 while(1)
 {
	delay_ms(500);
	delay_ms(500);
	delay_ms(500);
	delay_ms(500);
	GPIO_SetBits(GPIOE,GPIO_Pin_5);
	servo_angle(1);
	delay_ms(500);
	delay_ms(500);
	delay_ms(500);
	delay_ms(500);
	servo_angle(90);
	GPIO_ResetBits(GPIOE,GPIO_Pin_5);
 }
}
