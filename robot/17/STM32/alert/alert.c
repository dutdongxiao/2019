#include <alert.h>

int flag_alert = 0;//������Ĭ�Ϲر�

void alert_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//��ʼ��Ҫ�õ������ж˿�
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//PB9�Ƿ�������������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOB,GPIO_Pin_9);//�ر�ʹ�ܶ˿�
	
	TIM2_Init(400,35999);//�ֱ���Ϊ0.5ms,200ms���
}


void alert(void)
{
	while(flag_alert == 1)//�������������
	{
		GPIO_SetBits(GPIOB,GPIO_Pin_9);//��һ��
		Delay_us(625);
		GPIO_ResetBits(GPIOB,GPIO_Pin_9);//��һ��
		Delay_us(625);
	}
}


void alert_on(void)
{
	flag_alert = 1;//����������
	TIM_Cmd(TIM2,ENABLE);//������ʱ��
}


void alert_off(void)
{
	flag_alert = 0;//�رշ�����
	GPIO_ResetBits(GPIOB,GPIO_Pin_9);//GPIO�õ�
}

void TIM2_IRQHandler (void)//200ms �������������
{
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)//�ж��Ƿ���ARR����ж�,��������ж�
	{
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
		flag_alert = 0;//�رշ�����
		RS232_1_Send_Data((u8 *)"alert off\n",10);
		TIM_Cmd(TIM2,DISABLE);
	}
}
