/*-------------------------------------------------------------------------------
�ļ����ƣ�key.c
�ļ����������ð�����ʼ������       
��    ע����
---------------------------------------------------------------------------------*/
#include "key.h"
#include <delay.h>
#include <led.h>
/*-------------------------------------------------------------------------------
�������ƣ�KEY_Init
�����������������ų�ʼ�����򿪶˿�ʱ�ӣ����ö˿����ţ��˿ڹ���Ƶ�ʣ��˿�����ģʽ 
�����������
���ز�������
��    ע����
---------------------------------------------------------------------------------*/

u8 KEY_scan(u8 mode)
{
	static u8 key_up = 1;
	if(mode == 1)
	{
		key_up = 1;
	}
	if(key_up && !S1)
	{
		Delay_ms(10);
		key_up = 0;
		if(!S1)
		{
			return 1;
		}
	}
	else
	{
		if(S1)
		{
			key_up = 1;
			return 0;
		}
	}
	return 0;
}


void KEY_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	//��PB��ʱ��
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	//��PA��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	//��PE��ʱ��
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
	//PE2,PE3,PE4��������	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4;
	//�˿��ٶ�
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	//�˿�ģʽ����Ϊ��������ģʽ
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	//��ʼ����Ӧ�Ķ˿�
  GPIO_Init(GPIOE, &GPIO_InitStructure);
	//PA0��������	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	//�˿��ٶ�
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	//�˿�ģʽ����Ϊ��������ģʽ
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	//��ʼ����Ӧ�Ķ˿�
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
}
/*----------------------�·��� ������̳��www.doflye.net--------------------------*/
