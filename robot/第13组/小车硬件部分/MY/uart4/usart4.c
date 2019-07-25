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
	//GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	//ʹ��UGPIOBʱ��
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);	//ʹ��USART1ʱ��

	//UART3_TX  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PB10
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOC, &GPIO_InitStructure);
  //UART3_RX	  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PB11
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  //Usart3 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
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
  USART_Init(UART4, &USART_InitStructure);     //��ʼ������3
  USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
  USART_Cmd(UART4, ENABLE);                    //ʹ�ܴ���3 
	LED2_ON;
	Delay_ms(2000);
	LED2_OFF;
}

/**************************************************************************
�������ܣ�����3�����ж�
��ڲ�������
����  ֵ����
**************************************************************************/
u8 UART4_RX_BUF[UART4_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
u16 UART4_RX_STA=0;       //����״̬���	  
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
	//char mes[40]="���յ���������";
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)  //�����ж�(�Զ�����m6ͨ�� ���յ������ݱ�����0x0d 0x0a��β���س�����)
	{
		LED2_ON;
		Res =USART_ReceiveData(UART4);	//��ȡ���յ�������
		if((UART4_RX_STA&0x8000)==0)//����δ��ɣ���־λ��15λ��Ϊ��
		{
			if(Res==0x24)							 //���յ���Ԫ��
			{ 
				UART4_RX_STA|=0x8000;	 //��������ˣ�����־λ��15λ�ø� 
				UART4_RX_BUF[UART4_RX_STA&0X3FFF]='\0' ; 
			}
			else //��û�յ�0X0D(��Ԫ��)
			{	
				UART4_RX_BUF[UART4_RX_STA&0X3FFF]=Res ;
				UART4_RX_STA++;
				if(UART4_RX_STA>(UART4_REC_LEN-1))
				UART4_RX_STA=0;//�������ݴ���,���¿�ʼ����	  		 
			}
		}
		if((UART4_RX_STA&0x8000)!=0)//�������
		{
			judge_data_4(UART4_RX_BUF,UART4_RX_STA&0x00FF);
			//�������յ�
//			RS232_1_Send_Data((u8*)mes,14);
			  //RS232_4_Send_Data(UART4_RX_BUF,UART4_RX_STA&0x00FF);
//			RS232_1_Send_Data((u8*)"\n",1);
			UART4_RX_STA=0;//����������ɣ���ʼ��������
		}
	}*/
}
/**********************************************************
*	���ܣ���������
*	������buf ���ͻ������׵�ַ
*		  len �����͵��ֽ���
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
  	for(t=0;t<len;t++)		//ѭ����������
	{		   
		while(USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET);	//�������  
		USART_SendData(UART4,buf[t]);
	}	 
 
	while(USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET);		//�ȴ����һ��Bit�������
//	RS232_RX_CNT=0;	  
}
