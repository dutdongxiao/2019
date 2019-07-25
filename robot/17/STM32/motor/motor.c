#include <motor.h>

/*************************************
             ʹ��ST��PC10
��ǰ��PC0 ��ǰ��PC1 ����PC2 �Һ��PC3
**************************************/
extern int l_speed;
extern int r_speed;
extern int flag_stop;//��⵽�ϰ��Ժ��ֹͣ��־,0����û���ϰ�

u16 const speed = 1500;

void turn_left(void)
{
	TIM_Cmd(TIM3, ENABLE);
	GPIO_SetBits(GPIOC,GPIO_Pin_0);
	GPIO_SetBits(GPIOC,GPIO_Pin_2);
	GPIO_SetBits(GPIOC,GPIO_Pin_1);
	GPIO_SetBits(GPIOC,GPIO_Pin_3);//ȫ�ַ�ת
}

void turn_right(void)
{
	TIM_Cmd(TIM3, ENABLE);
	GPIO_ResetBits(GPIOC,GPIO_Pin_0);
	GPIO_ResetBits(GPIOC,GPIO_Pin_2);
	GPIO_ResetBits(GPIOC,GPIO_Pin_1);
	GPIO_ResetBits(GPIOC,GPIO_Pin_3);//ȫ����ת
}

void backward(void)
{
	TIM_Cmd(TIM3, ENABLE);
	GPIO_SetBits(GPIOC,GPIO_Pin_0);
	GPIO_SetBits(GPIOC,GPIO_Pin_2);//��ǰ����ַ�ת
	GPIO_ResetBits(GPIOC,GPIO_Pin_1);
	GPIO_ResetBits(GPIOC,GPIO_Pin_3);//��ǰ�Һ�����ת
}

void stop(void)
{
	TIM_Cmd(TIM3, DISABLE);
}

void keep_true(void)
{
	char *mes = "Obstacles ahead\n";
	if(flag_stop == 0)//���û���ϰ�
	{
	  TIM_Cmd(TIM3, ENABLE);
	  GPIO_ResetBits(GPIOC,GPIO_Pin_0);
	  GPIO_ResetBits(GPIOC,GPIO_Pin_2);//��ǰ�������ת
	  GPIO_SetBits(GPIOC,GPIO_Pin_1);
	  GPIO_SetBits(GPIOC,GPIO_Pin_3);//��ǰ�Һ��ַ�ת
	}
	else
	{
		RS232_1_Send_Data((u8 *)mes,16);
	}
}

void motor_init(void)

{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	TIM3_PWM_Init();
	TIM3_Init(499,35,1);
	//��ʼ��Ҫ�õ������ж˿�
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);//ʹ������ת�����I/O��ʹ��I/O
	GPIO_SetBits(GPIOC,GPIO_Pin_10);//����ʹ�ܶ˿�
	STOP;
}

void TIM3_IRQHandler(void)
{
	int Capture;
	if (TIM_GetITStatus(TIM3,TIM_IT_CC1)!=RESET)  //���TIM3CC1�жϷ������
	{
		Capture = TIM_GetCapture1(TIM3);
		TIM_ClearITPendingBit(TIM3,TIM_IT_CC1);  //���TIM3�����жϱ�־ 
    TIM_SetCompare3(TIM3, Capture + l_speed );//����TIM3�Զ���װ�ؼĴ���ֵ
	if (TIM_GetITStatus(TIM3,TIM_IT_CC2)!=RESET)  //���TIM3CC2�жϷ������
	{
		Capture = TIM_GetCapture1(TIM3);
		TIM_ClearITPendingBit(TIM3,TIM_IT_CC2);  //���TIM3�����жϱ�־ 
    TIM_SetCompare3(TIM3, Capture + r_speed );//����TIM3�Զ���װ�ؼĴ���ֵ
	}
	if (TIM_GetITStatus(TIM3,TIM_IT_CC3)!=RESET)  //���TIM3CC3�жϷ������
	{
		Capture = TIM_GetCapture1(TIM3);
		TIM_ClearITPendingBit(TIM3,TIM_IT_CC3);  //���TIM3�����жϱ�־ 
    TIM_SetCompare3(TIM3, Capture + l_speed );//����TIM3�Զ���װ�ؼĴ���ֵ
	}
	if (TIM_GetITStatus(TIM3,TIM_IT_CC4)!=RESET)  //���TIM3CC4�жϷ������
	{
		Capture = TIM_GetCapture1(TIM3);
		TIM_ClearITPendingBit(TIM3,TIM_IT_CC4);  //���TIM3�����жϱ�־ 
    TIM_SetCompare3(TIM3, Capture + r_speed );//����TIM3�Զ���װ�ؼĴ���ֵ
	}
	}
}
