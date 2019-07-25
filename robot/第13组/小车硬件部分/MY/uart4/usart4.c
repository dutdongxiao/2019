#include "stm32f10x.h"
#include "usart4.h"
#include "usart2.h"
#include "delay.h"
#include "led.h"
#include "string.h"
#include "TIM3.h"
int flag_heart = 0;
int flag_one = 0;
int flag_two = 0;
int flag_three = 0;
int flag_four = 0;
int flag_five = 0;
int flag_shake = 0;
int flag_node = 0;
int angel1 = 150;
int angel2 = 150;
void uart4_init(u32 bound)
{  	 
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	//使能UGPIOB时钟
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);	//使能USART1时钟

	//UART3_TX  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PB10
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOC, &GPIO_InitStructure);
  //UART3_RX	  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PB11
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  //Usart3 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
    
	//USART 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
  USART_Init(UART4, &USART_InitStructure);     //初始化串口3
  USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);//开启串口接受中断
  USART_Cmd(UART4, ENABLE);                    //使能串口3 
	LED2_ON;
	Delay_ms(2000);
	LED2_OFF;
}

/**************************************************************************
函数功能：串口3接收中断
入口参数：无
返回  值：无
**************************************************************************/
u8 UART4_RX_BUF[UART4_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
u16 UART4_RX_STA=0;       //接收状态标记	  
time_t timecount_4;      
void UART4_IRQHandler(void)
{	
	u8 Res;
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)
	{
	//	LED2_REV;
		Res = USART_ReceiveData(UART4);	
		judge(Res);
	}
	/*
	//char mes[40]="我收到的数据是";
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)  //接收中断(自定义与m6通信 接收到的数据必须是0x0d 0x0a结尾，回车换行)
	{
		LED2_ON;
		Res =USART_ReceiveData(UART4);	//读取接收到的数据
		if((UART4_RX_STA&0x8000)==0)//接收未完成，标志位第15位不为高
		{
			if(Res==0x24)							 //接收到美元符
			{ 
				UART4_RX_STA|=0x8000;	 //接收完成了，将标志位第15位置高 
				UART4_RX_BUF[UART4_RX_STA&0X3FFF]='\0' ; 
			}
			else //还没收到0X0D(美元符)
			{	
				UART4_RX_BUF[UART4_RX_STA&0X3FFF]=Res ;
				UART4_RX_STA++;
				if(UART4_RX_STA>(UART4_REC_LEN-1))
				UART4_RX_STA=0;//接收数据错误,重新开始接收	  		 
			}
		}
		if((UART4_RX_STA&0x8000)!=0)//接收完成
		{
			judge_data_4(UART4_RX_BUF,UART4_RX_STA&0x00FF);
			//数据已收到
//			RS232_1_Send_Data((u8*)mes,14);
			  //RS232_4_Send_Data(UART4_RX_BUF,UART4_RX_STA&0x00FF);
//			RS232_1_Send_Data((u8*)"\n",1);
			UART4_RX_STA=0;//接收数据完成，开始后续处理
		}
	}*/
}
/**********************************************************
*	功能：发送数据
*	参数：buf 发送缓冲区首地址
*		  len 待发送的字节数
**********************************************************/
void Delay(u8 len)
{
	u32 i,j;
	for(i=0;i<500000;i++)
		for(j=0;j<len;j++);
}
void judge(int flag)
{
	u32 i,j;
	if(flag == 0x68)
	{
		flag_heart = 1;
	}
	if(flag == 0x72)
		Round();
	if(flag == 0x70)
	{
	//	Delay_ms(1000);
	//	LED2_REV;
		State_Rev();
	}
	if(flag == 0x77)
	{
	//	LED2_ON;
		Run();
		//LED2_OFF;
	}
	if(flag == 0x6C)
		flag_one = 1;
	if(flag == 0x73)
		Back();
	if(flag == 0x61)
		Left();
	if(flag == 0x64)
		Right();
	if(flag == 0x71)
		Q_shift();	
	if(flag == 0x65)
		E_shift();	
	if(flag == 0x31)
	{
		if(angel1-5 < 0)
				return ;
			else
			angel1 = angel1-5;
			Set_Num_Angel(1,angel1);
	}
	if(flag == 0x32)
	{
			if(angel2-5 < 0)
				return ;
			else
				angel2 = angel2-5;
			Set_Num_Angel(2,angel2);
	}
	if(flag == 0x33)
	{
		if(angel1+5 > 300)
				return ;
			else
			angel1 = angel1+5;
			Set_Num_Angel(1,angel1);
	}
	if(flag == 0x35)
	{
		if(angel1+5 > 300)
				return ;
		else
			angel2 = angel2+5;
			Set_Num_Angel(2,angel2);
	}
	if(flag == 0x37)
		flag_two = 1;
	if(flag == 0x38)
	{
		flag_four = 1;
		//LED2_ON;
	}
	if(flag == 0x39)
		Round_1();
}
int compare_4(u8* buf,const char * str)
{
	int i;
	for(i=0;i<strlen(str);i++)
	{
		if(buf[i] != str[i])
			return 0;
	}
	return 1;
}
void judge_data_4(u8 *buf,u8 len)
{
	  if(len == 3)
		{
			if(compare_4(buf,"zan"))
				flag_node = 1;
			if(compare_4(buf,"one"))
				flag_one = 1;
			if(compare_4(buf,"two"))
				flag_two = 1;
		}
    if(len == 4)
		{
			if(compare_4(buf,"diss"))
				flag_shake = 1;
			if(compare_4(buf,"four"))
				flag_four = 1;
			if(compare_4(buf,"five"))
				flag_five = 1;
		}
		if(len == 5)
		{
			if(compare_4(buf,"three"))
				flag_three = 1;
			if(compare_4(buf,"heart"))
			{
				flag_heart = 1;
		//		LED2_ON;
			}
		}
}
void RS232_4_Send_Data(u8 *buf,u8 len)
{
	  u8 t;
  	for(t=0;t<len;t++)		//循环发送数据
	{		   
		while(USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET);	//发送完成  
		USART_SendData(UART4,buf[t]);
	}	 
 
	while(USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET);		//等待最后一个Bit发送完成
//	RS232_RX_CNT=0;	  
}
