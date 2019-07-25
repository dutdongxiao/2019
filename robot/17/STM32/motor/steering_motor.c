#include <steering_motor.h>
#include <tim2.h>
#include <tim4.h>
#include <delay.h>
#include <usart2.h>

//extern u16 last_pos_nod;//���������һ��λ��
int steer_lr_init = 0x0200;//��ʼλ��Ϊ�м�λ�ã�512
int steer_ud_init = 0x01EB;//��ʼλ��Ϊ�м�λ�ã�491
int steer_ud = 0x01EB;//�м�λ��
int steer_lr = 0x0200;//�м�λ��
extern int steer_flag_up;//����ʹ��
extern int steer_flag_down ;//����ʹ��
extern int steer_flag_right;//����ʹ��
extern int steer_flag_left;//����ʹ��

void steering_motor_init(void)
{
	//TIM2_Init(1999,719,2000 - last_pos_nod)
	uart2_init();
	steer_lr = steer_lr_init;//��ʼ���Ƕȱ���
	steer_ud = steer_ud_init;//��ʼ���Ƕȱ���
	Set_Num_Angle_smooth(1,steer_lr);
	Set_Num_Angle_smooth(2,steer_ud);
}

void down(void)
{
	if(steer_flag_down == 1)//���ʹ��Ϊ����ִ�ж���
	{
		if(steer_flag_up == 1)//�����������ʹ��Ϊ�ߣ������õͣ���ֹ��ͻ
		{
			steer_flag_up = 0;
		}
	  if(steer_ud >= 0x0060)//���ƶ�����Χ
	  {
	    steer_ud--;
			Set_Num_Angle_smooth(2,steer_ud);
			//Delay_ms(0);
	  }
		else
		{
			steer_flag_down = 0;//���������Χ����������ʧ��
		}
  }
}

void up(void)
{
	if(steer_flag_up == 1)
	{
		if(steer_flag_down == 1)
		{
			steer_flag_down = 0;
		}
	  if(steer_ud <= 0x036D)
	  {
	    steer_ud++;
			Set_Num_Angle_smooth(2,steer_ud);
		  //Delay_ms(0);
	  }
		else
		{
			steer_flag_up = 0;
		}
  }
}

void left(void)
{
	if(steer_flag_left == 1)
	{
		if(steer_flag_right == 1)
		{
			steer_flag_right = 0;
		}
	  if(steer_lr <= 0x03FF)//�Ƕ�Ϊ1023Ϊ���޽Ƕ�
		{
	    Set_Num_Angle_smooth(1,steer_lr++);
			//Delay_ms(0);
		}
		else
		{
			steer_flag_left = 0;
		}
  }
}

void right(void)
{
	if(steer_flag_right == 1)
	{
		if(steer_flag_left == 1)
		{
			steer_flag_left = 0;
		}
		if(steer_lr >= 0)//�Ƕ�Ϊ0�Ǽ��޽Ƕ�
		{
	    Set_Num_Angle_smooth(1,steer_lr--);
			//Delay_ms(0);
		}
		else
		{
			steer_flag_right = 0;
		}
  }
}


void Set_Num_Angle(u8 id,u32 arg)      //���ö����ת��Ŀ��Ƕ�
{
	u8 i;
  u8 sum = 0x00; 
  uint8_t dat[] = {0xFF,0xFF,0xFE,5,0x03,0x1E,0,0,0xFF};  //����һ�����ݰ�
	dat[2] = id;                          //����id
  dat[6] = (arg*0x3FF/300)&0xFF;        //����Ŀ��Ƕȵĵ��ֽ�
  dat[7] = (arg*0x3FF/300)>>8;          //����Ŀ��Ƕȵĸ��ֽ�
  for(i=2;i<8;i++)
  {
      sum+=dat[i];                    //����У���
  } 
  dat[8] = ~sum;
  RS232_2_Send_Data(dat,9);       //�����ݰ�����
}

void Set_Num_Angle_smooth(u8 id,u32 arg)      //���ö����ת��Ŀ��Ƕ�,ʹ��16���ƴ���
{
	u8 i;
  u8 sum = 0x00; 
  uint8_t dat[] = {0xFF,0xFF,0xFE,5,0x03,0x1E,0,0,0xFF};  //����һ�����ݰ�
	dat[2] = id;                          //����id
  dat[6] = arg&0xFF;        //����Ŀ��Ƕȵĵ��ֽ�
  dat[7] = arg>>8;          //����Ŀ��Ƕȵĸ��ֽ�
  for(i=2;i<8;i++)
  {
      sum+=dat[i];                    //����У���
  } 
  dat[8] = ~sum;
  RS232_2_Send_Data(dat,9);       //�����ݰ�����
}
