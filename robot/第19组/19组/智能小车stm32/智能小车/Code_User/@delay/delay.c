#include "delay.h"

static u16 SysTick_each_us_CNT ;	// SysTick计时1us所需的计数次数
static u16 SysTick_each_ms_CNT ;	// SysTick计时1ms所需的计数次数

// 初始化SysTick定时器
void delay_Init(void)
{
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);	// SysTick的时钟选为外部时钟的8分频（HCLK/8）
	SysTick_each_us_CNT = SystemCoreClock / 8 / 1000000;	// 得到1us的计数次数
	SysTick_each_ms_CNT = SysTick_each_us_CNT * 1000;		// 得到1ms的计数次数
}


u8 delay_us(u32 delay_us_Num)
{		
	u8  NOP_CNT = 4 ;
	u32 T_SysTick_CTR ;
	if( delay_us_Num == 0 )		// 参数不允许为0
	{ return 0 ; }				// 跳出函数
	if( delay_us_Num == 1 )
	{
		while( --NOP_CNT )		// NOP_CNT = 4 ;
		{
			__NOP();			// NOP：一个指令周期的空操作
		}
		return 0 ;				// 跳出函数
	}
	if( delay_us_Num>1 && delay_us_Num<1000 )
	{ delay_us_Num -- ; }
	SysTick->LOAD = (u32)SysTick_each_us_CNT * delay_us_Num ;			// 将此次函数的计数次数写入重装载寄存器
	SysTick->VAL = 0x00; 						// 将SysTick->VAL清0，从而使得SysTick->LOAD中的值装入SysTick->VAL
	SysTick->CTRL |= (u32)0x01 ;				// 使能SysTick定时器，计数器开始递减
	do{
		T_SysTick_CTR = SysTick->CTRL ;			// 将SysTick->CTRL中的值缓存到T_SysTick_CTR变量中
	}while( (T_SysTick_CTR & 0x01) && !(T_SysTick_CTR & (1<<16) ) );	// 当SysTick->CTRL的最低位为1，且第16位为0时，继续循环
	SysTick->CTRL &= ~ ( (u32)0x01 ) ;			// 失能SysTick定时器
	SysTick->VAL = 0X00;      					// 清0计数器
	return 0 ;	// 跳出函数
}

u8 delay_ms(u16 delay_ms_Num)
{	 		  	  
	u32 T_SysTick_CTR ;
	
	
	if( delay_ms_Num == 0 )		// 参数不允许为0
	{ return 0 ; }				// 跳出函数

	SysTick->LOAD = (u32)SysTick_each_ms_CNT * delay_ms_Num ;			// 将此次函数的计数次数写入重装载寄存器中
	
	SysTick->VAL = 0x00; 						// 将SysTick->VAL清0，从而使得SysTick->LOAD中的值装入SysTick->VAL
	SysTick->CTRL |= (u32)0x01 ;				// 使能SysTick定时器，计数器开始递减
	do{
		T_SysTick_CTR = SysTick->CTRL ;			// 将SysTick->CTRL中的值缓存到T_SysTick_CTR变量中
		
	}while( (T_SysTick_CTR & 0x01) && !(T_SysTick_CTR & (1<<16) ) );	// 当SysTick->CTRL的最低位为1，且第16位为0时，继续循环
	
	
	SysTick->CTRL &= ~ ( (u32)0x01 ) ;			// 失能SysTick定时器
	
	SysTick->VAL = 0X00;      					// 清空计数器
	
	return 0 ;	// 跳出函数
}
void delay_nms(u16 time)
{
	u16 i=0;
	while(time--)
	{
		i=2000;
		while(i--);
	}
}
void Delay_Ms(u16 time)  
{ 
    uint16_t i,j;
    for(i=0;i<time;i++)
          for(j=0;j<10260;j++);
}
void delay_nus(u16 time)  
{ 
    uint16_t i,j;
    for(i=0;i<time;i++)
          for(j=0;j<9;j++);
}
