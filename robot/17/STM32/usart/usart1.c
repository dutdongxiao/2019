#include <usart1.h>

extern int steer_flag_up;//����ʹ��
extern int steer_flag_down;//����ʹ��
extern int steer_flag_right;//����ʹ��
extern int steer_flag_left;//����ʹ��

void uart1_init(u32 bound)
{  	 
	  //GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��UGPIOBʱ��
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);	//ʹ��USART1ʱ��

	//UART3_TX  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);
  	//UART3_RX	  
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  	GPIO_Init(GPIOA, &GPIO_InitStructure);

  	//Usart1 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
    
	//USART ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
  	USART_Init(USART1, &USART_InitStructure);     //��ʼ������3
  	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
  	USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ���3 
}

/**************************************************************************
�������ܣ�����3�����ж�
��ڲ�������
����  ֵ����
**************************************************************************/
u8 USART1_RX_BUF[USART1_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
extern char command_buffer[USART1_REC_LEN];  //ָ��壬�洢������Ҫ��¼��ָ��
extern u8 command_exe;//ִ��ָ���ָ�룬ָ������ִ�е�ָ��
extern u8 command_flag;//��������ģʽ��־λ�� == 0 �������� == 1 ������¼�� == 2 ��������ѭ��
extern u8 command_time[USART1_REC_LEN];//ÿ��ָ��ִ��ʱ���ʱ���

u16 USART1_RX_STA=0;       //����״̬���	  
time_t timecount;
void USART1_IRQHandler(void)
{	
	u8 Res;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�(�Զ�����m6ͨ�� ���յ������ݱ�����0x0d 0x0a��β���س�����)
	{
		Res =USART_ReceiveData(USART1);	//��ȡ���յ�������
		if((USART1_RX_STA&0x8000)==0)//����δ���
		{
			
			if(Res==0x24)
			{ 
				USART1_RX_STA|=0x8000;	//��������� 
				USART1_RX_BUF[USART1_RX_STA&0X3FFF]='\0' ;
			}
			else //��û�յ�0X0D
			{	
				USART1_RX_BUF[USART1_RX_STA&0X3FFF]=Res ;
				USART1_RX_STA++;
				if(USART1_RX_STA>(USART1_REC_LEN-1))
				{
				  USART1_RX_STA=0;//�������ݴ���,���¿�ʼ����	 		
				}
			}
		}
		if((USART1_RX_STA&0x8000)!=0)//�������
		{
			//�������յ�
			USART1_RX_STA=0;//����������ɣ���ʼ��������
			if(*USART1_RX_BUF == 'F')//��ǰ
			{
				if(command_flag == 1)
				{
					TIM_Cmd(TIM4,ENABLE);//������ʱ��
					echo_record('F');//��¼ָ����Ϣ
				}
				if(command_flag == 2)//������ڽ��л���ѭ��
				{
					TIM_Cmd(TIM4,DISABLE);//�رն�ʱ��
				  command_flag = 0;//�˳�����ѭ��ģʽ
				}
				FORWARD;
				RS232_1_Send_Data((u8 *)&"FORWARD\n",8);
			}
			if(*USART1_RX_BUF == 'L')//����
			{
				if(command_flag == 1)
				{
					TIM_Cmd(TIM4,ENABLE);//������ʱ��
					echo_record('L');//��¼ָ����Ϣ
				}
				if(command_flag == 2)//������ڽ��л���ѭ��
				{
					TIM_Cmd(TIM4,DISABLE);//�رն�ʱ��
				  command_flag = 0;//�˳�����ѭ��ģʽ
				}
				LEFT;
				RS232_1_Send_Data((u8 *)&"LEFT\n",5);
			}
			if(*USART1_RX_BUF == 'B')//���
			{
				if(command_flag == 1)
				{
					TIM_Cmd(TIM4,ENABLE);//������ʱ��
					echo_record('B');//��¼ָ����Ϣ
				}
				if(command_flag == 2)//������ڽ��л���ѭ��
				{
					TIM_Cmd(TIM4,DISABLE);//�رն�ʱ��
				  command_flag = 0;//�˳�����ѭ��ģʽ
				}
				BACKWARD;
				RS232_1_Send_Data((u8 *)&"BACK\n",5);
			}
			if(*USART1_RX_BUF == 'R')//����
			{
				if(command_flag == 1)
				{
					TIM_Cmd(TIM4,ENABLE);//������ʱ��
					echo_record('R');//��¼ָ����Ϣ
				}
				if(command_flag == 2)//������ڽ��л���ѭ��
				{
					TIM_Cmd(TIM4,DISABLE);//�رն�ʱ��
				  command_flag = 0;//�˳�����ѭ��ģʽ
				}
				RIGHT;
				RS232_1_Send_Data((u8 *)&"RIGHT\n",6);
			}
			if(*USART1_RX_BUF == 'S')//ͣ��
			{
				if(command_flag == 1)
				{
					TIM_Cmd(TIM4,ENABLE);//������ʱ��
					echo_record('S');//��¼ָ����Ϣ
				}
				if(command_flag == 2)//������ڽ��л���ѭ��
				{
					TIM_Cmd(TIM4,DISABLE);//�رն�ʱ��
				  command_flag = 0;//�˳�����ѭ��ģʽ
				}
				STOP;
				RS232_1_Send_Data((u8 *)&"STOP\n",5);
			}
			if(*USART1_RX_BUF == 'u')//����ͷ����
			{
				steer_flag_up = 1;
				steer_flag_down = 0;
				RS232_1_Send_Data((u8 *)&"up\n",3);
			}
			if(*USART1_RX_BUF == 'd')//����ͷ����
			{
				steer_flag_down = 1;
				steer_flag_up = 0;
				RS232_1_Send_Data((u8 *)&"down\n",5);
			}
			if(*USART1_RX_BUF == 'l')//����ͷ����
			{
				steer_flag_left = 1;
				steer_flag_right = 0;
				RS232_1_Send_Data((u8 *)&"left\n",5);
			}
			if(*USART1_RX_BUF == 'r')//����ͷ����
			{
				steer_flag_right = 1;
				steer_flag_left = 0;
				RS232_1_Send_Data((u8 *)&"right\n",6);
			}
			if(*USART1_RX_BUF == 's')//����ͷʧ��
			{
				steer_flag_up = 0;//����
        steer_flag_down = 0;//����
        steer_flag_right = 0;//����
        steer_flag_left = 0;//����
				RS232_1_Send_Data((u8 *)&"stop\n",5);
			}
			if(*USART1_RX_BUF == 'o')//�����λ
			{
				steer_flag_up = 0;//����
        steer_flag_down = 0;//����
        steer_flag_right = 0;//����
        steer_flag_left = 0;//����
				steering_motor_init();
				RS232_1_Send_Data((u8 *)&"reset\n",6);
			}
			if(*USART1_RX_BUF == 'e')//����������¼
			{
				command_flag = 1;
				RS232_1_Send_Data((u8 *)&"echo record\n",12);
			}
			if(*USART1_RX_BUF == 'E')//��������ѭ��
			{
				command_flag = 2;//�رջ���ѭ��
				echo_record('K');//��¼ָ����Ϣ
				RS232_1_Send_Data((u8 *)&"echo trailing\n",14);
				//trailing();     ������ӵĻ��ᵼ���ж���ռ���ȼ�����Ӧ���ȼ���ͬ��tim����жϵȴ�usart�ж���ɵ����
			}
			if(*USART1_RX_BUF == 'a')//�����������Ҷ���Ƕ� 
			{
				u8 lr = 0;//���Ҷ���Ƕ�
				u8 i = 0, j = 0;//��¼һ����λ��
				char numh[3],numl[3];//�洢����Ƕȵĸߵ�λ
				while(USART1_RX_BUF[1 + i] != '%')
				{
					numh[i] = USART1_RX_BUF[1 + i];
					i++;
				}
				if(i == 2)//�������λ������������һλ���λ����
				{
					numh[2] = numh[1];
					numh[1] = numh[0];
					numh[0] = '0';
				}
				if(i == 1)//�����һλ����������λ�����λ����
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
				if(j == 2)//�������λ������������һλ���λ����
				{
					numl[2] = numl[1];
					numl[1] = numl[0];
					numl[0] = '0';
				}
				if(j == 1)//�����һλ����������λ�����λ����
				{
					numl[2] = numl[0];
					numl[1] = '0';
					numl[0] = '0';
				}
				lr = atoi(numh) + atoi(numl);
				Set_Num_Angle(1,lr);
				RS232_1_Send_Data((u8 *)&"gyroscope control lr\n",21);
			}
			if(*USART1_RX_BUF == 'A')//���¶���Ƕ�
			{
				u8 ud = 0;//���¶���Ƕ�
				u8 i = 0;//��¼һ����λ��
				char numh[3];//�洢����Ƕȵĸߵ�λ
				while(USART1_RX_BUF[2 + i] != '%')
				{
					numh[i] = USART1_RX_BUF[1 + i];
					i++;
				}
				if(i == 2)//�������λ������������һλ���λ����
				{
					numh[2] = numh[1];
					numh[1] = numh[0];
					numh[0] = '0';
				}
				if(i == 1)//�����һλ����������λ�����λ����
				{
					numh[2] = numh[0];
					numh[1] = '0';
					numh[0] = '0';
				}
				ud = atoi(numh);
				Set_Num_Angle(2,ud);
				RS232_1_Send_Data((u8 *)&"gyroscope control ud\n",21);
			}
			if(*USART1_RX_BUF == 'z')//��������200ms
			{
				alert_on();//������ʱ��
				RS232_1_Send_Data((u8 *)"alert on\n",9);
			}
		}
	}
}
/**********************************************************
*	���ܣ���������
*	������buf ���ͻ������׵�ַ
*		  len �����͵��ֽ���
**********************************************************/
void RS232_1_Send_Data(u8 *buf,u8 len)
{
	u8 t;
	
  	for(t=0;t<len;t++)		//ѭ����������
	{		   
		while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);	//�������  
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
