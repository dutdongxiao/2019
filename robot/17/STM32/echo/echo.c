#include <echo.h>


char command_buffer[USART1_REC_LEN];  //ָ��壬�洢������Ҫ��¼��ָ��
u8 command_ptr = 0;//��¼ָ���ָ�룬ָ��ָ���ջ��
u8 command_exe = 0;//ִ��ָ���ָ�룬ָ������ִ�е�ָ��
u8 command_flag = 0;//��������ģʽ��־λ�� == 0 �������� == 1 ������¼�� == 2 ��������ѭ��
uint16_t command_time[USART1_REC_LEN];//ÿ��ָ��ִ��ʱ���ʱ���
u8 ti_flag = 0;//��û�н����жϵı�ʶ��
u32 time_sum = 0;

void trailing(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	ti_flag = 0;
	if(command_flag == 2)// �������ѭ������
	{
	  command_exe = 0;//��λָ��ִ��ָ��
		ti_flag = 0;//��λ�жϱ�ʶ��
		TIM_Cmd(TIM4,ENABLE);
		if(command_buffer[command_exe] == 'F')//�ֶ�װ���һ��ָ��
	  {
	     FORWARD;
	  	 RS232_1_Send_Data((u8 *)&"FORWARD\n",8);
	  }
	  if(command_buffer[command_exe] == 'L')//����
	  {
	     LEFT;
	     RS232_1_Send_Data((u8 *)&"LEFT\n",5);
	  }
	  if(command_buffer[command_exe] == 'B')//���
	  {
	    BACKWARD;
	    RS232_1_Send_Data((u8 *)&"BACK\n",5);
	  }
	  if(command_buffer[command_exe] == 'R')//����
	  {
	    RIGHT;
	    RS232_1_Send_Data((u8 *)&"RIGHT\n",6);
	  }
	  TIM4->CNT = 0;//��ʱ������
		command_exe += 1;
		TIM4->ARR = command_time[command_exe];	//���� 
		TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE); //ʹ��ָ����TIM4�ж�
		TIM_Cmd(TIM4,ENABLE);
	  while(command_exe < command_ptr)//���û��ȡ�����һ��ָֹͣ��,ѭ��ȡָ��,�ȴ���ʱ�����������жϷ������ִ����һ��ָ��
	  {
			if(ti_flag == 1)//����������ж�
			{
	  	  //TIM4->ARR = command_time[command_exe];//����װ����װ�ؼ�ʱ��
	  	  //TIM4->CNT = 0;//��ʱ������
				ti_flag = 0;//��ʶ������
				TIM_TimeBaseStructure.TIM_Period= command_time[command_exe];	//����
	      TIM_TimeBaseStructure.TIM_Prescaler= 34999;		//��Ƶ
	      TIM_TimeBaseStructure.TIM_ClockDivision=0;		
	      TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;	//	���ϴ���		
	      TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); 
		  	TIM_Cmd(TIM4,ENABLE);
			}
			/*TIM_TimeBaseStructure.TIM_Period= command_time[command_exe];	//����
	    TIM_TimeBaseStructure.TIM_Prescaler= 34999;		//��Ƶ
	    TIM_TimeBaseStructure.TIM_ClockDivision=0;		
	    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;	//	���ϴ���		
	    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); 
			TIM_Cmd(TIM4,ENABLE);*/
			//TIM4->CNT = 0;//��ʱ������
		  //TIM4->ARR = command_time[command_exe];//����װ����װ�ؼ�ʱ��
			//if(command_exe < command_ptr)
			//{
			//	break;
			//	TIM_Cmd(TIM4,DISABLE);//�رռ�ʱ��
			//}
			
  	}
		//Delay_ms(time_sum * 0.5);
		STOP;
		RS232_1_Send_Data((u8 *)&"STOP\n",5);
		TIM_Cmd(TIM4,DISABLE);//�رռ�ʱ��
		TIM4->CNT = 0;//��ʱ������
	  command_ptr = 0;//��λָ���ָ��
	  command_exe = 0;//��λָ��ִ��ָ��
	  command_flag = 0;//��λ����ģʽ��־λ
		RS232_1_Send_Data((u8 *)&"trailing complete\n",18);
	}
}

void echo_record(char dir)
{
	if(command_flag == 1)//����ڼ�¼ģʽ
	{
		if(dir != 'S')//�������ָֹͣ��
		{
		  command_buffer[command_ptr] = dir;//��¼ָ����Ϣ
		  command_ptr++;//�������Ĺ�
			TIM4->CNT = 0;//�����ʱ��
	  }
		else//�����ָֹͣ��
		{
			command_time[command_ptr - 1] = TIM_GetCounter(TIM4);//��¼ָ��ִ��ʱ�䣨��ʱָ��ָ����һ��ָ����λ�ã�
			time_sum += command_time[command_ptr - 1];
		}
	}
	if(command_flag == 2)//����������ѭ��
	{
		TIM4->CNT = 0;//�����ʱ��
	  command_buffer[command_ptr] = dir;//k����kill���ǽ�����
	}
}

void TIM4_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)//�ж��Ƿ���ARR����ж�,��������ж�
	{
		TIM_ClearITPendingBit(TIM4,TIM_IT_Update);
		if(command_flag == 2)//�����������ѭ��ģʽ
		{
			//ti_flag = 0;//�����ж�
			if(command_buffer[command_exe] == 'F')//��ǰ
	    {
	    	FORWARD;
	  	  RS232_1_Send_Data((u8 *)&"FORWARD\n",8);
	    }
	    if(command_buffer[command_exe] == 'L')//����
	    {
	    	LEFT;
	    	RS232_1_Send_Data((u8 *)&"LEFT\n",5);
	    }
	    if(command_buffer[command_exe] == 'B')//���
	    {
	    	BACKWARD;
	    	RS232_1_Send_Data((u8 *)&"BACK\n",5);
	    }
	    if(command_buffer[command_exe] == 'R')//����
	    {
	    	RIGHT;
	    	RS232_1_Send_Data((u8 *)&"RIGHT\n",6);
	    }
			//TIM_SetCompare1(TIM4,0);
			//while(TIM_GetCapture1(TIM4) != command_time[command_exe]);//��ʱָ��ִ��ʱ��
			command_exe++;//װ����һ��ָ��
			ti_flag = 1;
			//if(command_exe < command_ptr)
			//{
			  //TIM4->CNT = 0;//��ʱ������
			 // TIM4->ARR = command_time[command_exe];//����װ����װ�ؼ�ʱ��
				//ti_flag = 1;
			//}
			//else
			//{
			//	TIM_Cmd(TIM4,DISABLE);//�رռ�ʱ��
			//}
		}
		LED2_REV;
	}
}
