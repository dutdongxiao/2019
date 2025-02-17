/*-------------------------------------------------------------------------------
文件名称：led.c
文件描述：根据硬件连接配置LED端口，打开对应的寄存器        
备    注：无
---------------------------------------------------------------------------------*/
#include "led.h"
/*-------------------------------------------------------------------------------
程序名称：LED_Init
程序描述：初始化LED相关端口，打开端口时钟，配置端口输出  
输入参数：无
返回参数：无
备    注：无
---------------------------------------------------------------------------------*/

void LED_Init(void)
{
	//使能对应控制正反转四通道对应管脚
  GPIO_InitTypeDef GPIO_InitStructure;
  //打开PB口时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
	//PB5,PE5引脚设置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	//端口速度
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	//端口模式，此为输出推挽模式
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	//初始化对应的端口
  GPIO_Init(GPIOE, &GPIO_InitStructure);
}

