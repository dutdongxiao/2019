#include "usart2.h"
#include "delay.h"

int angel_1 = 170;
int angel_2 = 170;

void uart2_init(u32 bound)
{  	 
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//使能UGPIOA时钟
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	//使能USART2时钟

	//UART2_TX  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA2
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);
  //UART2_RX	  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA3
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  //Usart2 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
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
  USART_Init(USART2, &USART_InitStructure);     //初始化串口3
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启串口接受中断
  USART_Cmd(USART2, ENABLE);                    //使能串口3 
	
  Set_Num_Angel(1,angel_1);
  Set_Num_Angel(2,angel_2);
}
void judge_angel(u8 flag)
{
		if(flag == 0x31)
		{
			if(angel_1-5 < 0)
				return ;
			else
			  angel_1 = angel_1-5;
			Set_Num_Angel(1,angel_1);
		}
		if(flag == 0x32)
		{
			 if(angel_1+5 > 300)
				return ;
			else
			  angel_1 = angel_1+5;
			Set_Num_Angel(1,angel_1);
		}
		if(flag == 0x33)
		{
			if(angel_2-5 < 0)
				return ;
			else
				angel_2 = angel_2-5;
			Set_Num_Angel(2,angel_2);
		}
		if(flag == 0x34)
		{
			if(angel_2+5 > 300)
				return ;
			else
			  angel_2 = angel_2+5;
			Set_Num_Angel(2,angel_2);
		}
}
void shake(void)
{
	int i = 0;
	for(i = 0; i < 3;i++)
	{
		Set_Num_Angel(1,145);
		Delay_ms(1000);
		Set_Num_Angel(1,195);
		Delay_ms(1000);
	}
}
void node(void)
{
	int i = 0;
	for(i=0;i<3;i++)
	{
		Set_Num_Angel(2,120);
		Delay_ms(750);
		Set_Num_Angel(2,170);
		Delay_ms(750);
	}
}
void Set_Num_Angel(u8 id,u32 arg)      //设置舵机旋转的目标角度
{
    static u8 i = 0; 
    u8 sum = 0x00; 
    uint8_t dat[] = {0xFF,0xFF,0,5,0x03,0x1E,0,0,0xFF};  //定义一个数据包
    dat[2] = id;                          //设置ID号
    dat[6] = (arg*0x3FF/300)&0xFF;        //设置目标角度的低字节
    dat[7] = (arg*0x3FF/300)>>8;          //设置目标角度的高字节
    for(i=2;i<8;i++)
    {
        sum+=dat[i];                    //计算校验和
    } 
    dat[8] = ~sum&0xFF;                      
    
    for(i=0;i<9;i++)
    {
        USART2_Send_Char(dat[i]);       //将数据包发出
    }
}
void USART2_Send_Char(uint8_t data)
{
	  USART_SendData(USART2,data);
    while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);	//发送完成  
}