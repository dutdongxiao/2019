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
	//GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);  //ʹ��USART2��GPIOAʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	//USART2_TX   GPIOA.2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.2

	//USART2_RX	  GPIOA.3
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   //��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);           //��ʼ��GPIOA.3  

	//USART2 NVIC����
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;        //��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		      //�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			        //IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	                           //����ָ���Ĳ�����ʼ��NVIC�Ĵ���

	 //USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;        //�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;           //һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;            //����żУ��λ
	//��Ӳ������������
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	       //�շ�ģʽ

	USART_Init(USART2, &USART_InitStructure);                     //��ʼ������2
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);               //�������ڽ����ж�
	USART_Cmd(USART2, ENABLE);                                  //ʹ�ܴ���2 

}
	

/**************************************************************************
�������ܣ�����2�����ж�
*************************************************************************/
u8 USART2_RX_BUF[USART2_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
u16 USART2_RX_STA=0;       //����״̬���	  
//time_t timecount;  
u8 i=0;
void USART2_IRQHandler(void)
{	
	u8 Res;
	char mes[40]="RECEIVE IS:";
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //�����ж�
	{
		Res =USART_ReceiveData(USART2);	//��ȡ���յ�������
		if((USART2_RX_STA&0x8000)==0)//����δ���
		{
			if(Res==0x24)
			{ 
				USART2_RX_STA|=0x8000;	//��������� 
				USART2_RX_BUF[USART2_RX_STA&0X3FFF]='\0' ;
			}
			else //��û�յ�0X0D
			{	
				USART2_RX_BUF[USART2_RX_STA&0X3FFF]=Res ;
				USART2_RX_STA++;
				if(USART2_RX_STA>(USART2_REC_LEN-1))
				USART2_RX_STA=0;//�������ݴ���,���¿�ʼ����	  		 
			}
		}
		if((USART2_RX_STA&0x8000)!=0)//�������
		{
			//�������յ�
			RS232_1_Send_Data((u8*)mes,14);
			RS232_1_Send_Data(USART2_RX_BUF,USART2_RX_STA&0x00FF);
			RS232_1_Send_Data((u8*)"\n",1);
			USART2_RX_STA=0;//����������ɣ���ʼ��������
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
		while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);	//�������  
		USART_SendData(USART2,buf[t]);
	}	 
 
	while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);		
//	RS232_RX_CNT=0;	  
}
/************************************************************
�ж�ָ��
ִ��ָ��
*********************************************************/
void USART2_carry(void)
{
	char mes1[40]="HUMAM HEAT";
	char mes2[40]="BARRIER";
	char mes3[40]="A PIT";
	
	for(i=0;i<(USART2_REC_LEN-1);i++)
	{
		//��ǰ
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
	
	//���
	if (USART2_RX_BUF[i]==0x32)
	{
				Car_backward(30);
				Delay_Ms(2000);
		   break;
	}
	//����
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
		
	//����
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

