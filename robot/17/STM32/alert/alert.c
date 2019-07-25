#include <alert.h>

int flag_alert = 0;//蜂鸣器默认关闭

void alert_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//初始化要用到的所有端口
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//PB9是蜂鸣器控制引脚
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOB,GPIO_Pin_9);//关闭使能端口
	
	TIM2_Init(400,35999);//分辨率为0.5ms,200ms溢出
}


void alert(void)
{
	while(flag_alert == 1)//如果蜂鸣器开启
	{
		GPIO_SetBits(GPIOB,GPIO_Pin_9);//叫一下
		Delay_us(625);
		GPIO_ResetBits(GPIOB,GPIO_Pin_9);//叫一下
		Delay_us(625);
	}
}


void alert_on(void)
{
	flag_alert = 1;//开启蜂鸣器
	TIM_Cmd(TIM2,ENABLE);//开启计时器
}


void alert_off(void)
{
	flag_alert = 0;//关闭蜂鸣器
	GPIO_ResetBits(GPIOB,GPIO_Pin_9);//GPIO置低
}

void TIM2_IRQHandler (void)//200ms 后结束蜂鸣器响
{
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)//判断是否是ARR溢出中断,如果发生中断
	{
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
		flag_alert = 0;//关闭蜂鸣器
		RS232_1_Send_Data((u8 *)"alert off\n",10);
		TIM_Cmd(TIM2,DISABLE);
	}
}
