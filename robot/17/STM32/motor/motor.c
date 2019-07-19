#include <motor.h>

/*************************************
             使能ST接PC10
左前接PC0 右前接PC1 左后接PC2 右后接PC3
**************************************/
extern int l_speed;
extern int r_speed;
extern int flag_stop;//检测到障碍以后的停止标志,0代表没有障碍

u16 const speed = 1500;

void turn_left(void)
{
	TIM_Cmd(TIM3, ENABLE);
	GPIO_SetBits(GPIOC,GPIO_Pin_0);
	GPIO_SetBits(GPIOC,GPIO_Pin_2);
	GPIO_SetBits(GPIOC,GPIO_Pin_1);
	GPIO_SetBits(GPIOC,GPIO_Pin_3);//全轮反转
}

void turn_right(void)
{
	TIM_Cmd(TIM3, ENABLE);
	GPIO_ResetBits(GPIOC,GPIO_Pin_0);
	GPIO_ResetBits(GPIOC,GPIO_Pin_2);
	GPIO_ResetBits(GPIOC,GPIO_Pin_1);
	GPIO_ResetBits(GPIOC,GPIO_Pin_3);//全轮正转
}

void backward(void)
{
	TIM_Cmd(TIM3, ENABLE);
	GPIO_SetBits(GPIOC,GPIO_Pin_0);
	GPIO_SetBits(GPIOC,GPIO_Pin_2);//左前左后轮反转
	GPIO_ResetBits(GPIOC,GPIO_Pin_1);
	GPIO_ResetBits(GPIOC,GPIO_Pin_3);//右前右后轮正转
}

void stop(void)
{
	TIM_Cmd(TIM3, DISABLE);
}

void keep_true(void)
{
	char *mes = "Obstacles ahead\n";
	if(flag_stop == 0)//如果没有障碍
	{
	  TIM_Cmd(TIM3, ENABLE);
	  GPIO_ResetBits(GPIOC,GPIO_Pin_0);
	  GPIO_ResetBits(GPIOC,GPIO_Pin_2);//左前左后轮正转
	  GPIO_SetBits(GPIOC,GPIO_Pin_1);
	  GPIO_SetBits(GPIOC,GPIO_Pin_3);//右前右后轮反转
	}
	else
	{
		RS232_1_Send_Data((u8 *)mes,16);
	}
}

void motor_init(void)

{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	TIM3_PWM_Init();
	TIM3_Init(499,35,1);
	//初始化要用到的所有端口
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);//使能所有转向控制I/O和使能I/O
	GPIO_SetBits(GPIOC,GPIO_Pin_10);//开启使能端口
	STOP;
}

void TIM3_IRQHandler(void)
{
	int Capture;
	if (TIM_GetITStatus(TIM3,TIM_IT_CC1)!=RESET)  //检查TIM3CC1中断发生与否
	{
		Capture = TIM_GetCapture1(TIM3);
		TIM_ClearITPendingBit(TIM3,TIM_IT_CC1);  //清除TIM3更新中断标志 
    TIM_SetCompare3(TIM3, Capture + l_speed );//设置TIM3自动重装载寄存器值
	if (TIM_GetITStatus(TIM3,TIM_IT_CC2)!=RESET)  //检查TIM3CC2中断发生与否
	{
		Capture = TIM_GetCapture1(TIM3);
		TIM_ClearITPendingBit(TIM3,TIM_IT_CC2);  //清除TIM3更新中断标志 
    TIM_SetCompare3(TIM3, Capture + r_speed );//设置TIM3自动重装载寄存器值
	}
	if (TIM_GetITStatus(TIM3,TIM_IT_CC3)!=RESET)  //检查TIM3CC3中断发生与否
	{
		Capture = TIM_GetCapture1(TIM3);
		TIM_ClearITPendingBit(TIM3,TIM_IT_CC3);  //清除TIM3更新中断标志 
    TIM_SetCompare3(TIM3, Capture + l_speed );//设置TIM3自动重装载寄存器值
	}
	if (TIM_GetITStatus(TIM3,TIM_IT_CC4)!=RESET)  //检查TIM3CC4中断发生与否
	{
		Capture = TIM_GetCapture1(TIM3);
		TIM_ClearITPendingBit(TIM3,TIM_IT_CC4);  //清除TIM3更新中断标志 
    TIM_SetCompare3(TIM3, Capture + r_speed );//设置TIM3自动重装载寄存器值
	}
	}
}
