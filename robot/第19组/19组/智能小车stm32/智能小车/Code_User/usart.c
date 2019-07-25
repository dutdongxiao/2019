#include "stm32f10x.h"
//#include "bit_band.h"
#include "MotorDrive.h"
#include "stm32f10x_tim.h" 
#include "stm32f10x_gpio.h" 
#include "stm32f10x_rcc.h" 
#include "misc.h" 
#include "stm32f10x_conf.h" 
#include "stm32f10x_usart.h"
#include "usart.h"
#include "hcsr041.h"
 void Usart2_Init(u32 bound) 
	{
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);  //使能USART2，GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	//USART2_TX   GPIOA.2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.2

	//USART2_RX	  GPIOA.3
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   //浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);           //初始化GPIOA.3  

	//USART2 NVIC配置
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;        //抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		      //子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			        //IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	                           //根据指定的参数初始化NVIC寄存器

	 //USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;        //字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;           //一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;            //无奇偶校验位
	//无硬件数据流控制
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	       //收发模式

	USART_Init(USART2, &USART_InitStructure);                     //初始化串口2
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);               //开启串口接收中断
	USART_Cmd(USART2, ENABLE);                                  //使能串口2 

}
	

/**************************************************************************
函数功能：串口2接收中断
*************************************************************************/
u8 USART2_RX_BUF[USART2_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
u16 USART2_RX_STA=0;       //接收状态标记	  
//time_t timecount;  
u8 i=0;
void USART2_IRQHandler(void)
{	
	u8 Res;
	char mes[40]="RECEIVE IS:";
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //接收中断
	{
		Res =USART_ReceiveData(USART2);	//读取接收到的数据
		if((USART2_RX_STA&0x8000)==0)//接收未完成
		{
			if(Res==0x24)
			{ 
				USART2_RX_STA|=0x8000;	//接收完成了 
				USART2_RX_BUF[USART2_RX_STA&0X3FFF]='\0' ;
			}
			else //还没收到0X0D
			{	
				USART2_RX_BUF[USART2_RX_STA&0X3FFF]=Res ;
				USART2_RX_STA++;
				if(USART2_RX_STA>(USART2_REC_LEN-1))
				USART2_RX_STA=0;//接收数据错误,重新开始接收	  		 
			}
		}
		if((USART2_RX_STA&0x8000)!=0)//接收完成
		{
			//数据已收到
			RS232_1_Send_Data((u8*)mes,14);
			RS232_1_Send_Data(USART2_RX_BUF,USART2_RX_STA&0x00FF);
			RS232_1_Send_Data((u8*)"\n",1);
			USART2_RX_STA=0;//接收数据完成，开始后续处理
		}
	}
}
/**********************************************************
*	功能：发送数据
*	参数：buf 发送缓冲区首地址
*		  len 待发送的字节数
**********************************************************/
void RS232_1_Send_Data(u8 *buf,u8 len)
{
	u8 t;
  	for(t=0;t<len;t++)		//循环发送数据
	{		   
		while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);	//发送完成  
		USART_SendData(USART2,buf[t]);
	}	 
 
	while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);		
//	RS232_RX_CNT=0;	  
}
/************************************************************
判断指令
执行指令
*********************************************************/
void USART2_carry(void)
{
	char mes1[40]="HUMAM HEAT";
	char mes2[40]="BARRIER";
	char mes3[40]="A PIT";
	
	for(i=0;i<(USART2_REC_LEN-1);i++)
	{
		//向前
	if (USART2_RX_BUF[i]==0x31)
	{
			if(HC_SR501_Status() == 1)
	{RS232_1_Send_Data((u8*)mes1,14);}
		while(1)
		{	
		if( Hcsr04GetLength1()<20)
		{
			RS232_1_Send_Data((u8*)mes2,14);
			for(i=0;i<(USART2_REC_LEN-1);i++)
	      {USART2_RX_BUF[i]=0;}
		 Car_Stop(0);		
		 break;
		}
		else if (Hcsr041GetLength1()>10)
					{
						RS232_1_Send_Data((u8*)mes3,14);
			    for(i=0;i<(USART2_REC_LEN-1);i++)
	           {USART2_RX_BUF[i]=0;}
		        Car_Stop(0);		
		        break;
					}
		Car_forward(30);Delay_Ms(1000);break;
						}
		if(HC_SR501_Status() == 1)
	{RS232_1_Send_Data((u8*)mes1,14);}
	}
	
	//向后
	if (USART2_RX_BUF[i]==0x32)
	{
				Car_backward(30);
				Delay_Ms(2000);
		   break;
	}
	//向左
		if (USART2_RX_BUF[i]==0x33)
	{
			if(HC_SR501_Status() == 1)
	{RS232_1_Send_Data((u8*)mes1,14);}
		while(1)
		{	
		if( Hcsr04GetLength1()<20)
		{
			RS232_1_Send_Data((u8*)mes2,14);
			for(i=0;i<(USART2_REC_LEN-1);i++)
	      {USART2_RX_BUF[i]=0;}
		 Car_Stop(0);		
		 break;
		}
		else if (Hcsr041GetLength1()>10)
					{
						RS232_1_Send_Data((u8*)mes3,14);
			    for(i=0;i<(USART2_REC_LEN-1);i++)
	           {USART2_RX_BUF[i]=0;}
		        Car_Stop(0);		
		        break;
					}
		Car_Turn_Left(30);Delay_Ms(1000);break;
						}
		if(HC_SR501_Status() == 1)
	{RS232_1_Send_Data((u8*)mes1,14);}
	}	
		
	//向右
	if (USART2_RX_BUF[i]==0x34)
	{
		if(HC_SR501_Status() == 1)
	{RS232_1_Send_Data((u8*)mes1,14);}
		while(1)
		{	
		if( Hcsr04GetLength1()<20)
		{
			RS232_1_Send_Data((u8*)mes2,14);
			for(i=0;i<(USART2_REC_LEN-1);i++)
	      {USART2_RX_BUF[i]=0;}
		 Car_Stop(0);		
		 break;
		}
		else if (Hcsr041GetLength1()>10)
					{
						RS232_1_Send_Data((u8*)mes3,14);
			    for(i=0;i<(USART2_REC_LEN-1);i++)
	           {USART2_RX_BUF[i]=0;}
		        Car_Stop(0);		
		        break;
					}
		Car_Turn_Left(30);Delay_Ms(1000);break;
						}
		if(HC_SR501_Status() == 1)
	{RS232_1_Send_Data((u8*)mes1,14);}
}
	
	if (USART2_RX_BUF[i]==0x35)
	{
				Car_Stop(0);
	}
	if (USART2_RX_BUF[i]=='\0')
	{
			break;
	}
}
	for(i=0;i<(USART2_REC_LEN-1);i++)
	{USART2_RX_BUF[i]=0;}
}

