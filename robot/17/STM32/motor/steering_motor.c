#include <steering_motor.h>
#include <tim2.h>
#include <tim4.h>
#include <delay.h>
#include <usart2.h>

//extern u16 last_pos_nod;//俯仰舵机上一次位置
int steer_lr_init = 0x0200;//初始位置为中间位置，512
int steer_ud_init = 0x01EB;//初始位置为中间位置，491
int steer_ud = 0x01EB;//中间位置
int steer_lr = 0x0200;//中间位置
extern int steer_flag_up;//向上使能
extern int steer_flag_down ;//向下使能
extern int steer_flag_right;//向右使能
extern int steer_flag_left;//向左使能

void steering_motor_init(void)
{
	//TIM2_Init(1999,719,2000 - last_pos_nod)
	uart2_init();
	steer_lr = steer_lr_init;//初始化角度变量
	steer_ud = steer_ud_init;//初始化角度变量
	Set_Num_Angle_smooth(1,steer_lr);
	Set_Num_Angle_smooth(2,steer_ud);
}

void down(void)
{
	if(steer_flag_down == 1)//如果使能为高则执行动作
	{
		if(steer_flag_up == 1)//如果反方向动作使能为高，将其置低，防止冲突
		{
			steer_flag_up = 0;
		}
	  if(steer_ud >= 0x0060)//限制动作范围
	  {
	    steer_ud--;
			Set_Num_Angle_smooth(2,steer_ud);
			//Delay_ms(0);
	  }
		else
		{
			steer_flag_down = 0;//如果超过范围，将本动作失能
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
	  if(steer_lr <= 0x03FF)//角度为1023为极限角度
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
		if(steer_lr >= 0)//角度为0是极限角度
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


void Set_Num_Angle(u8 id,u32 arg)      //设置舵机旋转的目标角度
{
	u8 i;
  u8 sum = 0x00; 
  uint8_t dat[] = {0xFF,0xFF,0xFE,5,0x03,0x1E,0,0,0xFF};  //定义一个数据包
	dat[2] = id;                          //设置id
  dat[6] = (arg*0x3FF/300)&0xFF;        //设置目标角度的低字节
  dat[7] = (arg*0x3FF/300)>>8;          //设置目标角度的高字节
  for(i=2;i<8;i++)
  {
      sum+=dat[i];                    //计算校验和
  } 
  dat[8] = ~sum;
  RS232_2_Send_Data(dat,9);       //将数据包发出
}

void Set_Num_Angle_smooth(u8 id,u32 arg)      //设置舵机旋转的目标角度,使用16进制传参
{
	u8 i;
  u8 sum = 0x00; 
  uint8_t dat[] = {0xFF,0xFF,0xFE,5,0x03,0x1E,0,0,0xFF};  //定义一个数据包
	dat[2] = id;                          //设置id
  dat[6] = arg&0xFF;        //设置目标角度的低字节
  dat[7] = arg>>8;          //设置目标角度的高字节
  for(i=2;i<8;i++)
  {
      sum+=dat[i];                    //计算校验和
  } 
  dat[8] = ~sum;
  RS232_2_Send_Data(dat,9);       //将数据包发出
}
