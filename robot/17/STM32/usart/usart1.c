#include <usart1.h>

extern int steer_flag_up;//向上使能
extern int steer_flag_down;//向下使能
extern int steer_flag_right;//向右使能
extern int steer_flag_left;//向左使能

void uart1_init(u32 bound)
{  	 
	  //GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//使能UGPIOB时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);	//使能USART1时钟

	//UART3_TX  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);
  	//UART3_RX	  
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  	GPIO_Init(GPIOA, &GPIO_InitStructure);

  	//Usart1 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
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
  	USART_Init(USART1, &USART_InitStructure);     //初始化串口3
  	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启串口接受中断
  	USART_Cmd(USART1, ENABLE);                    //使能串口3 
}

/**************************************************************************
函数功能：串口3接收中断
入口参数：无
返回  值：无
**************************************************************************/
u8 USART1_RX_BUF[USART1_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
extern char command_buffer[USART1_REC_LEN];  //指令缓冲，存储所有需要记录的指令
extern u8 command_exe;//执行指令缓冲指针，指向正在执行的指令
extern u8 command_flag;//开启回声模式标志位， == 0 不开启， == 1 开启记录， == 2 开启回声循迹
extern u8 command_time[USART1_REC_LEN];//每条指令执行时间的时间戳

u16 USART1_RX_STA=0;       //接收状态标记	  
time_t timecount;
void USART1_IRQHandler(void)
{	
	u8 Res;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(自定义与m6通信 接收到的数据必须是0x0d 0x0a结尾，回车换行)
	{
		Res =USART_ReceiveData(USART1);	//读取接收到的数据
		if((USART1_RX_STA&0x8000)==0)//接收未完成
		{
			
			if(Res==0x24)
			{ 
				USART1_RX_STA|=0x8000;	//接收完成了 
				USART1_RX_BUF[USART1_RX_STA&0X3FFF]='\0' ;
			}
			else //还没收到0X0D
			{	
				USART1_RX_BUF[USART1_RX_STA&0X3FFF]=Res ;
				USART1_RX_STA++;
				if(USART1_RX_STA>(USART1_REC_LEN-1))
				{
				  USART1_RX_STA=0;//接收数据错误,重新开始接收	 		
				}
			}
		}
		if((USART1_RX_STA&0x8000)!=0)//接收完成
		{
			//数据已收到
			USART1_RX_STA=0;//接收数据完成，开始后续处理
			if(*USART1_RX_BUF == 'F')//向前
			{
				if(command_flag == 1)
				{
					TIM_Cmd(TIM4,ENABLE);//开启定时器
					echo_record('F');//记录指令信息
				}
				if(command_flag == 2)//如果正在进行回声循迹
				{
					TIM_Cmd(TIM4,DISABLE);//关闭定时器
				  command_flag = 0;//退出回声循迹模式
				}
				FORWARD;
				RS232_1_Send_Data((u8 *)&"FORWARD\n",8);
			}
			if(*USART1_RX_BUF == 'L')//向左
			{
				if(command_flag == 1)
				{
					TIM_Cmd(TIM4,ENABLE);//开启定时器
					echo_record('L');//记录指令信息
				}
				if(command_flag == 2)//如果正在进行回声循迹
				{
					TIM_Cmd(TIM4,DISABLE);//关闭定时器
				  command_flag = 0;//退出回声循迹模式
				}
				LEFT;
				RS232_1_Send_Data((u8 *)&"LEFT\n",5);
			}
			if(*USART1_RX_BUF == 'B')//向后
			{
				if(command_flag == 1)
				{
					TIM_Cmd(TIM4,ENABLE);//开启定时器
					echo_record('B');//记录指令信息
				}
				if(command_flag == 2)//如果正在进行回声循迹
				{
					TIM_Cmd(TIM4,DISABLE);//关闭定时器
				  command_flag = 0;//退出回声循迹模式
				}
				BACKWARD;
				RS232_1_Send_Data((u8 *)&"BACK\n",5);
			}
			if(*USART1_RX_BUF == 'R')//向右
			{
				if(command_flag == 1)
				{
					TIM_Cmd(TIM4,ENABLE);//开启定时器
					echo_record('R');//记录指令信息
				}
				if(command_flag == 2)//如果正在进行回声循迹
				{
					TIM_Cmd(TIM4,DISABLE);//关闭定时器
				  command_flag = 0;//退出回声循迹模式
				}
				RIGHT;
				RS232_1_Send_Data((u8 *)&"RIGHT\n",6);
			}
			if(*USART1_RX_BUF == 'S')//停车
			{
				if(command_flag == 1)
				{
					TIM_Cmd(TIM4,ENABLE);//开启定时器
					echo_record('S');//记录指令信息
				}
				if(command_flag == 2)//如果正在进行回声循迹
				{
					TIM_Cmd(TIM4,DISABLE);//关闭定时器
				  command_flag = 0;//退出回声循迹模式
				}
				STOP;
				RS232_1_Send_Data((u8 *)&"STOP\n",5);
			}
			if(*USART1_RX_BUF == 'u')//摄像头向上
			{
				steer_flag_up = 1;
				steer_flag_down = 0;
				RS232_1_Send_Data((u8 *)&"up\n",3);
			}
			if(*USART1_RX_BUF == 'd')//摄像头向下
			{
				steer_flag_down = 1;
				steer_flag_up = 0;
				RS232_1_Send_Data((u8 *)&"down\n",5);
			}
			if(*USART1_RX_BUF == 'l')//摄像头向左
			{
				steer_flag_left = 1;
				steer_flag_right = 0;
				RS232_1_Send_Data((u8 *)&"left\n",5);
			}
			if(*USART1_RX_BUF == 'r')//摄像头向右
			{
				steer_flag_right = 1;
				steer_flag_left = 0;
				RS232_1_Send_Data((u8 *)&"right\n",6);
			}
			if(*USART1_RX_BUF == 's')//摄像头失能
			{
				steer_flag_up = 0;//向上
        steer_flag_down = 0;//向下
        steer_flag_right = 0;//向右
        steer_flag_left = 0;//向左
				RS232_1_Send_Data((u8 *)&"stop\n",5);
			}
			if(*USART1_RX_BUF == 'o')//舵机复位
			{
				steer_flag_up = 0;//向上
        steer_flag_down = 0;//向下
        steer_flag_right = 0;//向右
        steer_flag_left = 0;//向左
				steering_motor_init();
				RS232_1_Send_Data((u8 *)&"reset\n",6);
			}
			if(*USART1_RX_BUF == 'e')//开启回声记录
			{
				command_flag = 1;
				RS232_1_Send_Data((u8 *)&"echo record\n",12);
			}
			if(*USART1_RX_BUF == 'E')//开启回声循迹
			{
				command_flag = 2;//关闭回声循迹
				echo_record('K');//记录指令信息
				RS232_1_Send_Data((u8 *)&"echo trailing\n",14);
				//trailing();     在这里加的话会导致中断抢占优先级和响应优先级相同，tim溢出中断等待usart中断完成的情况
			}
			if(*USART1_RX_BUF == 'a')//独立设置左右舵机角度 
			{
				u8 lr = 0;//左右舵机角度
				u8 i = 0, j = 0;//记录一共几位数
				char numh[3],numl[3];//存储舵机角度的高低位
				while(USART1_RX_BUF[1 + i] != '%')
				{
					numh[i] = USART1_RX_BUF[1 + i];
					i++;
				}
				if(i == 2)//如果是两位数，整体右移一位最高位清零
				{
					numh[2] = numh[1];
					numh[1] = numh[0];
					numh[0] = '0';
				}
				if(i == 1)//如果是一位数，右移两位最高两位清零
				{
					numh[2] = numh[0];
					numh[1] = '0';
					numh[0] = '0';
				}
				while(USART1_RX_BUF[i + 2 + j + 1] != '%')
				{
					numl[j] = USART1_RX_BUF[i + 2 + j];
					j++;
				}
				if(j == 2)//如果是两位数，整体右移一位最高位清零
				{
					numl[2] = numl[1];
					numl[1] = numl[0];
					numl[0] = '0';
				}
				if(j == 1)//如果是一位数，右移两位最高两位清零
				{
					numl[2] = numl[0];
					numl[1] = '0';
					numl[0] = '0';
				}
				lr = atoi(numh) + atoi(numl);
				Set_Num_Angle(1,lr);
				RS232_1_Send_Data((u8 *)&"gyroscope control lr\n",21);
			}
			if(*USART1_RX_BUF == 'A')//上下舵机角度
			{
				u8 ud = 0;//上下舵机角度
				u8 i = 0;//记录一共几位数
				char numh[3];//存储舵机角度的高低位
				while(USART1_RX_BUF[2 + i] != '%')
				{
					numh[i] = USART1_RX_BUF[1 + i];
					i++;
				}
				if(i == 2)//如果是两位数，整体右移一位最高位清零
				{
					numh[2] = numh[1];
					numh[1] = numh[0];
					numh[0] = '0';
				}
				if(i == 1)//如果是一位数，右移两位最高两位清零
				{
					numh[2] = numh[0];
					numh[1] = '0';
					numh[0] = '0';
				}
				ud = atoi(numh);
				Set_Num_Angle(2,ud);
				RS232_1_Send_Data((u8 *)&"gyroscope control ud\n",21);
			}
			if(*USART1_RX_BUF == 'z')//蜂鸣器响200ms
			{
				alert_on();//开启计时器
				RS232_1_Send_Data((u8 *)"alert on\n",9);
			}
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
		while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);	//发送完成  
		USART_SendData(USART1,buf[t]);
	}	 
  
}

int isspace(int x)
{
if(x==' '||x=='\t'||x=='\n'||x=='\f'||x=='\b'||x=='\r')
  return 1;
else  
  return 0;
}

int isdigit(int x)
{
if(x<='9'&&x>='0')         
  return 1;
else 
  return 0;

}

int atoi(const char *nptr)
{
        int c;              /* current char */
        int total;         /* current total */
        int sign;           /* if '-', then negative, otherwise positive */

        /* skip whitespace */
        while ( isspace((int)(unsigned char)*nptr) )
            ++nptr;

        c = (int)(unsigned char)*nptr++;
        sign = c;           /* save sign indication */
        if (c == '-' || c == '+')
            c = (int)(unsigned char)*nptr++;    /* skip sign */

        total = 0;

        while (isdigit(c)) {
            total = 10 * total + (c - '0');     /* accumulate digit */
            c = (int)(unsigned char)*nptr++;    /* get next char */
        }

        if (sign == '-')
            return -total;
        else
            return total;   /* return result, negated if necessary */
}
