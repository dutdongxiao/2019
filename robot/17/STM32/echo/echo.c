#include <echo.h>


char command_buffer[USART1_REC_LEN];  //指令缓冲，存储所有需要记录的指令
u8 command_ptr = 0;//记录指令缓冲指针，指向指令缓冲栈顶
u8 command_exe = 0;//执行指令缓冲指针，指向正在执行的指令
u8 command_flag = 0;//开启回声模式标志位， == 0 不开启， == 1 开启记录， == 2 开启回声循迹
uint16_t command_time[USART1_REC_LEN];//每条指令执行时间的时间戳
u8 ti_flag = 0;//有没有进入中断的标识符
u32 time_sum = 0;

void trailing(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	ti_flag = 0;
	if(command_flag == 2)// 如果回声循迹开启
	{
	  command_exe = 0;//复位指令执行指针
		ti_flag = 0;//复位中断标识符
		TIM_Cmd(TIM4,ENABLE);
		if(command_buffer[command_exe] == 'F')//手动装入第一个指令
	  {
	     FORWARD;
	  	 RS232_1_Send_Data((u8 *)&"FORWARD\n",8);
	  }
	  if(command_buffer[command_exe] == 'L')//向左
	  {
	     LEFT;
	     RS232_1_Send_Data((u8 *)&"LEFT\n",5);
	  }
	  if(command_buffer[command_exe] == 'B')//向后
	  {
	    BACKWARD;
	    RS232_1_Send_Data((u8 *)&"BACK\n",5);
	  }
	  if(command_buffer[command_exe] == 'R')//向右
	  {
	    RIGHT;
	    RS232_1_Send_Data((u8 *)&"RIGHT\n",6);
	  }
	  TIM4->CNT = 0;//计时器清零
		command_exe += 1;
		TIM4->ARR = command_time[command_exe];	//周期 
		TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE); //使能指定的TIM4中断
		TIM_Cmd(TIM4,ENABLE);
	  while(command_exe < command_ptr)//如果没有取到最后一个停止指令,循环取指令,等待计时器溢出后进入中断服务程序执行下一条指令
	  {
			if(ti_flag == 1)//如果进入了中断
			{
	  	  //TIM4->ARR = command_time[command_exe];//重新装载重装载计时器
	  	  //TIM4->CNT = 0;//计时器清零
				ti_flag = 0;//标识符清零
				TIM_TimeBaseStructure.TIM_Period= command_time[command_exe];	//周期
	      TIM_TimeBaseStructure.TIM_Prescaler= 34999;		//分频
	      TIM_TimeBaseStructure.TIM_ClockDivision=0;		
	      TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;	//	向上触发		
	      TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); 
		  	TIM_Cmd(TIM4,ENABLE);
			}
			/*TIM_TimeBaseStructure.TIM_Period= command_time[command_exe];	//周期
	    TIM_TimeBaseStructure.TIM_Prescaler= 34999;		//分频
	    TIM_TimeBaseStructure.TIM_ClockDivision=0;		
	    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;	//	向上触发		
	    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); 
			TIM_Cmd(TIM4,ENABLE);*/
			//TIM4->CNT = 0;//计时器清零
		  //TIM4->ARR = command_time[command_exe];//重新装载重装载计时器
			//if(command_exe < command_ptr)
			//{
			//	break;
			//	TIM_Cmd(TIM4,DISABLE);//关闭计时器
			//}
			
  	}
		//Delay_ms(time_sum * 0.5);
		STOP;
		RS232_1_Send_Data((u8 *)&"STOP\n",5);
		TIM_Cmd(TIM4,DISABLE);//关闭计时器
		TIM4->CNT = 0;//计时器清零
	  command_ptr = 0;//复位指令缓冲指针
	  command_exe = 0;//复位指令执行指针
	  command_flag = 0;//复位回声模式标志位
		RS232_1_Send_Data((u8 *)&"trailing complete\n",18);
	}
}

void echo_record(char dir)
{
	if(command_flag == 1)//如果在记录模式
	{
		if(dir != 'S')//如果不是停止指令
		{
		  command_buffer[command_ptr] = dir;//记录指令信息
		  command_ptr++;//编译器的锅
			TIM4->CNT = 0;//清零计时器
	  }
		else//如果是停止指令
		{
			command_time[command_ptr - 1] = TIM_GetCounter(TIM4);//记录指令执行时间（此时指针指向下一个指令存放位置）
			time_sum += command_time[command_ptr - 1];
		}
	}
	if(command_flag == 2)//如果进入回声循迹
	{
		TIM4->CNT = 0;//清零计时器
	  command_buffer[command_ptr] = dir;//k代表kill，是结束符
	}
}

void TIM4_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)//判断是否是ARR溢出中断,如果发生中断
	{
		TIM_ClearITPendingBit(TIM4,TIM_IT_Update);
		if(command_flag == 2)//如果开启回声循迹模式
		{
			//ti_flag = 0;//进入中断
			if(command_buffer[command_exe] == 'F')//向前
	    {
	    	FORWARD;
	  	  RS232_1_Send_Data((u8 *)&"FORWARD\n",8);
	    }
	    if(command_buffer[command_exe] == 'L')//向左
	    {
	    	LEFT;
	    	RS232_1_Send_Data((u8 *)&"LEFT\n",5);
	    }
	    if(command_buffer[command_exe] == 'B')//向后
	    {
	    	BACKWARD;
	    	RS232_1_Send_Data((u8 *)&"BACK\n",5);
	    }
	    if(command_buffer[command_exe] == 'R')//向右
	    {
	    	RIGHT;
	    	RS232_1_Send_Data((u8 *)&"RIGHT\n",6);
	    }
			//TIM_SetCompare1(TIM4,0);
			//while(TIM_GetCapture1(TIM4) != command_time[command_exe]);//延时指令执行时间
			command_exe++;//装入下一条指令
			ti_flag = 1;
			//if(command_exe < command_ptr)
			//{
			  //TIM4->CNT = 0;//计时器清零
			 // TIM4->ARR = command_time[command_exe];//重新装载重装载计时器
				//ti_flag = 1;
			//}
			//else
			//{
			//	TIM_Cmd(TIM4,DISABLE);//关闭计时器
			//}
		}
		LED2_REV;
	}
}
