#include "hcsr04.h"
#include "delay.h"
#include "led.h"
 
#define HCSR04_PORT     GPIOD
#define HCSR04_CLK      RCC_APB2Periph_GPIOD
#define HCSR04_TRIG     GPIO_Pin_6
#define HCSR04_ECHO     GPIO_Pin_7

void TRIG_Send(int i)
{
	if(i == 0)
		GPIO_ResetBits(HCSR04_PORT,GPIO_Pin_6);
	else
		GPIO_SetBits(HCSR04_PORT,GPIO_Pin_6);
}
int ECHO_Reci(void)
{
	return GPIO_ReadInputDataBit(HCSR04_PORT,GPIO_Pin_7);
}
u16 msHcCount = 0;//ms计数
void Hcsr04Init()
{  
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;     //生成用于定时器设置的结构体
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(HCSR04_CLK, ENABLE);

           //IO初始化
    GPIO_InitStructure.GPIO_Pin =HCSR04_TRIG;       //发送电平引脚
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//推挽输出
    GPIO_Init(HCSR04_PORT, &GPIO_InitStructure);
    GPIO_ResetBits(HCSR04_PORT,HCSR04_TRIG);
     
    GPIO_InitStructure.GPIO_Pin =   HCSR04_ECHO;     //返回电平引脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(HCSR04_PORT, &GPIO_InitStructure);  
    GPIO_ResetBits(HCSR04_PORT,HCSR04_ECHO);    
     
            //定时器初始化 使用基本定时器TIM4
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);   //使能对应RCC时钟
        //配置定时器基础结构体
    TIM_TimeBaseStructure.TIM_Period = (1000-1); //设置在下一个更新事件装入活动的自动重装载寄存器周期的值         计数到1000为1ms
    TIM_TimeBaseStructure.TIM_Prescaler =(72-1); //设置用来作为TIMx时钟频率除数的预分频值  1M的计数频率 1US计数
    TIM_TimeBaseStructure.TIM_ClockDivision= 0;//不分频
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位         
        
    TIM_ClearFlag(TIM4, TIM_FLAG_Update);   //清除更新中断，免得一打开中断立即产生中断
    TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);    //打开定时器更新中断
    hcsr04_NVIC();

    TIM_Cmd(TIM4,DISABLE);     
}
//tips：static函数的作用域仅限于定义它的源文件内，所以不需要在头文件里声明
static void OpenTimerForHc()        //打开定时器
{
    TIM_SetCounter(TIM4,0);//清除计数
    msHcCount = 0;
    TIM_Cmd(TIM4, ENABLE);  //使能TIMx外设
}
 
static void CloseTimerForHc()        //关闭定时器
{
    TIM_Cmd(TIM4, DISABLE);  //使能TIMx外设
}
 
 //NVIC配置
void hcsr04_NVIC()
{
    NVIC_InitTypeDef NVIC_InitStructure;
    
    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;             //选择串口1中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //抢占式中断优先级设置为1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;         //响应式中断优先级设置为1
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;        //使能中断
    NVIC_Init(&NVIC_InitStructure);
}

//定时器3中断服务程序
void TIM4_IRQHandler(void)   //TIM4中断
{
    if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)  //检查TIM4更新中断发生与否
        {
            TIM_ClearITPendingBit(TIM4, TIM_IT_Update  );  //清除TIMx更新中断标志 
            msHcCount++;
        }
}
 
 //获取定时器时间
u32 GetEchoTimer(void)
{
        u32 t = 0;
        t = msHcCount*1000;//得到MS
        t += TIM_GetCounter(TIM4);//得到US
	/*
	      if(t < 2)
					LED2_ON;
	*/
          TIM4->CNT = 0;  //将TIM4计数寄存器的计数值清零
                Delay_ms(50);
        return t;
}
 
 //一次获取超声波测距数据 两次测距之间需要相隔一段时间，隔断回响信号
//为了消除余震的影响，取五次数据的平均值进行加权滤波。
float Hcsr04GetLength(void )
{
        u32 t = 0;
        int i = 0;
        float lengthTemp = 0;
        float sum = 0;
        while(i!=5)
        {
        TRIG_Send(1);      //发送口高电平输出
        Delay_us(20);
        TRIG_Send(0);
				//OpenTimerForHc();
        while(ECHO_Reci() == 0)
				{ /*
					if(GetEchoTimer() > 100000)
					{
						LED2_ON;
						CloseTimerForHc();
						return 1000;
					}*/
				}        			 	  			//等待接收口高电平输出s
				//CloseTimerForHc();
            OpenTimerForHc();        //打开定时器
            i = i + 1;
            while(ECHO_Reci() == 1);
            CloseTimerForHc();        //关闭定时器
            t = GetEchoTimer();        //获取时间,分辨率为1US
					  t = (float)(t);
            lengthTemp = (t*17.0)/1000.0;//cm
            sum = lengthTemp + sum ;
    }
        lengthTemp = sum/5.0;
        return lengthTemp;
}
