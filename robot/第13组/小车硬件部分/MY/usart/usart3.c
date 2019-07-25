#include "usart3.h"
#include "TIM3.h"
#include "delay.h"
#include "led.h"
#include "string.h"

	extern int flag_heart;
	extern int flag_one;
	extern int flag_two;
  extern int flag_three;
  extern int flag_four ;
	extern int flag_five ;
	extern int flag_shake;
	extern int flag_node;
void uart3_init(u32 bound)
{  	 
	//GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//ʹ��UGPIOBʱ��
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);	//ʹ��USART1ʱ��

	//UART3_TX  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PB10
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOB, &GPIO_InitStructure);
  //UART3_RX	  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PB11
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  //Usart3 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
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
  USART_Init(USART3, &USART_InitStructure);     //��ʼ������3
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
  USART_Cmd(USART3, ENABLE);                    //ʹ�ܴ���3 
}

/**************************************************************************
�������ܣ�����3�����ж�
��ڲ�������
����  ֵ����
**************************************************************************/
u8 USART3_RX_BUF[USART3_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
u16 USART3_RX_STA=0;       //����״̬���	  
time_t timecount;      
void USART3_IRQHandler(void)
{	
	u8 Res;
	/*
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
	{
		LED2_REV;
		Res = USART_ReceiveData(USART3);	
		judge(Res);
	}
	*/
	//char mes[40]="���յ���������";
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //�����ж�(�Զ�����m6ͨ�� ���յ������ݱ�����0x0d 0x0a��β���س�����)
	{
		//LED2_ON;
		Res =USART_ReceiveData(USART3);	//��ȡ���յ�������
		if((USART3_RX_STA&0x8000)==0)//����δ��ɣ���־λ��15λ��Ϊ��
		{
			if(Res==0x24)							 //���յ���Ԫ��
			{ 
				USART3_RX_STA|=0x8000;	 //��������ˣ�����־λ��15λ�ø� 
				USART3_RX_BUF[USART3_RX_STA&0X3FFF]='\0' ; 
			}
			else //��û�յ�0X0D(��Ԫ��)
			{	
				USART3_RX_BUF[USART3_RX_STA&0X3FFF]=Res ;
				USART3_RX_STA++;
				if(USART3_RX_STA>(USART3_REC_LEN-1))
				USART3_RX_STA=0;//�������ݴ���,���¿�ʼ����	  		 
			}
		}
		if((USART3_RX_STA&0x8000)!=0)//�������
		{
			//�������յ�
//			RS232_1_Send_Data((u8*)mes,14);
			judge_data(USART3_RX_BUF,USART3_RX_STA&0x00FF);
			RS232_1_Send_Data(USART3_RX_BUF,USART3_RX_STA&0x00FF);
//			RS232_1_Send_Data((u8*)"\n",1);
			USART3_RX_STA=0;//����������ɣ���ʼ��������
		}
	}
}
/**********************************************************
*	���ܣ���������
*	������buf ���ͻ������׵�ַ
*		  len �����͵��ֽ���
**********************************************************/
int compare(u8* buf,const char * str)
{
	int i;
	for(i=0;i<strlen(str);i++)
	{
		if(buf[i] != str[i])
			return 0;
	}
	return 1;
}
void judge_data(u8 *buf,u8 len)
{
  if(len == 3)
		{
			if(compare(buf,"zan"))
				flag_node = 1;
			if(compare(buf,"one"))
				flag_one = 1;
			if(compare(buf,"two"))
				flag_two = 1;
		}
    if(len == 4)
		{
			if(compare(buf,"diss"))
				flag_shake = 1;
			if(compare(buf,"four"))
				flag_four = 1;
			if(compare(buf,"five"))
				flag_five = 1;
		}
		if(len == 5)
		{
			if(compare(buf,"three"))
				flag_three = 1;
			if(compare(buf,"heart"))
			{
				flag_heart = 1;
			//	LED2_REV;
			}
		}
}
void RS232_1_Send_Data(u8 *buf,u8 len)
{
	  u8 t;
  	for(t=0;t<len;t++)		//ѭ����������
	{		   
		while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);	//�������  
		USART_SendData(USART3,buf[t]);
	}	 
 
	while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);		//�ȴ����һ��Bit�������
//	RS232_RX_CNT=0;	  
}
