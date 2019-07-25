//3youqian 0zuoqian 2zuohou   1youhou
#include "stm32f10x.h"
#include "usart1.h"
#include "led.h"
#include "delay.h"
u8 flag=0;
u8 f=0;
#define rate 600
#define   GPIO_IN     GPIOB
#define   GPIO_PWM     GPIOA

#define   GPIO_Pin_IN1    GPIO_Pin_1
#define   GPIO_Pin_IN2    GPIO_Pin_2
#define   GPIO_Pin_IN3    GPIO_Pin_5
#define   GPIO_Pin_IN4    GPIO_Pin_6
#define   IN4_H       GPIO_SetBits(GPIO_IN,GPIO_Pin_IN1); 
#define   IN4_L       GPIO_ResetBits(GPIO_IN,GPIO_Pin_IN1);

#define   IN1_H       GPIO_SetBits(GPIO_IN,GPIO_Pin_IN2);
#define   IN1_L       GPIO_ResetBits(GPIO_IN,GPIO_Pin_IN2);

#define   IN3_H       GPIO_SetBits(GPIO_IN,GPIO_Pin_IN3);
#define   IN3_L       GPIO_ResetBits(GPIO_IN,GPIO_Pin_IN3);

#define   IN2_H       GPIO_SetBits(GPIO_IN,GPIO_Pin_IN4);
#define   IN2_L       GPIO_ResetBits(GPIO_IN,GPIO_Pin_IN4);

#define   PWM_A_OFF   1001
#define   PWM_B_OFF   1001
#define	TRIG_PORT      GPIOB		//TRIG       
#define	ECHO_PORT      GPIOB		//ECHO 
#define	TRIG_PIN       GPIO_Pin_8   //TRIG       
#define	ECHO_PIN       GPIO_Pin_9	//ECHO  

#define DEBUG_USARTx USART1
 #define DEBUG_USART_CLK RCC_APB2Periph_USART1

 #define DEBUG_USART_BAUDRATE 115200

 #define DEBUG_USART_GPIO_CLK (RCC_APB2Periph_GPIOA)

 
#define DEBUG_USART_TX_GPIO_PORT GPIOA
 #define DEBUG_USART_TX_GPIO_PIN GPIO_Pin_9
 #define DEBUG_USART_RX_GPIO_PORT GPIOA
 #define DEBUG_USART_RX_GPIO_PIN GPIO_Pin_10
 
 #define DEBUG_USART_IRQ USART1_IRQn
 #define DEBUG_USART_IRQHandler USART1_IRQHandler

u8 Res=0;
void TimCounterInit(int rate_1,int rate_2);
void Gpio_PWM_Init(void);
void Gpio_M_Init(void);
void NVIC_Configuration(void); 
void follow(void);
void TurnLeft(void);
void Right();
void Left();
void TurnRight(void);
void straight(void);
void sternway(void);
void stop(void);
int arr,psc;
u32 now_arg=117;     //shiji 88
void delay_ms(int i)
{
	while(i--)
	{int j=12000;
		while(j--)
			;		
	}
}





void  UART4_Configuration(void)//duoji
{
       RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC , ENABLE); //??UART3??GPIOB???
       RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE); //??????
	GPIO_InitTypeDef GPIO_InitStructure;
      // Configure USART3 Tx (PB.10) as alternate function push-pull 
      GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
      GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
      GPIO_Init(GPIOC,   &GPIO_InitStructure);
      // Configure USART3 Rx (PB.11) as input floating 
      GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_11;
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;          
      GPIO_Init(GPIOC, &GPIO_InitStructure);
	USART_InitTypeDef USART_InitStructure;
 USART_InitStructure.USART_BaudRate = 1000000;
      USART_InitStructure.USART_WordLength = USART_WordLength_8b;
      USART_InitStructure.USART_StopBits = USART_StopBits_1;
      USART_InitStructure.USART_Parity = USART_Parity_No;
      USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
      USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
      USART_Init(UART4, &USART_InitStructure);
     
      USART_Cmd(UART4, ENABLE);
			USART_ITConfig(UART4, USART_IT_RXNE,ENABLE);
			USART_ClearFlag(UART4,USART_FLAG_TC);
}
void UART4_Send_Char(uint8_t ch)
 {
 /* ????????? USART */
 USART_SendData(UART4,ch);
 
 /* ??????????? */
 while (USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET);
 }

void Set_Num_1(void) //?????ID??1
{
    UART4->CR1 &= ~0x4;
    UART4_Send_Char(0xFF);
    UART4_Send_Char(0XFF);
    UART4_Send_Char(0XFE);
   UART4_Send_Char(0X04);
    UART4_Send_Char(0X03);
   UART4_Send_Char(0X03);
    UART4_Send_Char(0X01);
    UART4_Send_Char(0XF6);
   UART4->CR1 |= 0x4;
    delay_ms(2);
}
void Set_Num_arg(u8 id,u32 arg)      //???????????
{
	LED2_OFF;
	delay_ms(1000);
	LED2_ON;
    static u8 i = 0; 
    u8 sum = 0x00; 
    uint8_t dat[] = {0xFF,0xFF,0,5,0x03,0x1E,0,0,0xFF};  //???????
    dat[2] = id;                          //??ID?
    dat[6] = (arg*0x3FF/300)&0xFF;        //dibawei
    dat[7] = (arg*0x3FF/300)>>8;          //??????????
    for(i=2;i<8;i++)
    {
        sum+=dat[i];                    //?????
    } 
    dat[8] = ~sum;                      
    
    for(i=0;i<9;i++)
    {
        UART4_Send_Char(dat[i]);       //??????
    }
}



void stop()
{
		
        
        /* TIM???? */
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, DISABLE);

        /* ??????? */
                                             //??TIM

}

void TurnLeft()
{  
         IN1_L IN2_L
         IN3_H IN4_H
         arr=4000;
         psc=35;        
         TimCounterInit(arr,psc);
}
void TurnRight()
{  

         IN1_H IN2_H
         IN3_L IN4_L
         arr=4000;
         psc=35;        
         TimCounterInit(arr,psc);

}

void Right()
{  

         IN1_H IN2_H
         IN3_H IN4_H
         arr=2000;
         psc=35;        
         TimCounterInit(arr,psc);

}
void Left()
{
         IN1_L IN2_L
         IN3_L IN4_L
          arr=2000;
         psc=35;      
        TimCounterInit(arr,psc);

}
void straight(void)
{
         IN1_L IN2_H
         IN3_L IN4_H
        arr=2000;
         psc=35;      
        TimCounterInit(arr,psc);
}

void sternway(void)
{
        IN1_H IN2_L     
        IN3_H IN4_L        
      arr=2000;
         psc=35;      
        TimCounterInit(arr,psc);
}
void Gpio_PWM_Init(void)//PWM
{
        GPIO_InitTypeDef GPIO_InitStructure;
  //??????TIM2???1(PA0)???????,??????????
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

        GPIO_InitStructure.GPIO_Pin        = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;              //??led1
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;          //?????50MHZ
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;         //?????????
  GPIO_Init(GPIO_PWM, &GPIO_InitStructure);           //???led1???
}

void Gpio_M_Init(void)
{
        GPIO_InitTypeDef GPIO_InitStructure;
  
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

        GPIO_InitStructure.GPIO_Pin        = GPIO_Pin_IN1|GPIO_Pin_IN2|GPIO_Pin_IN3|GPIO_Pin_IN4;              //??led1
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;          //?????50MHZ
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;         
  GPIO_Init(GPIO_IN, &GPIO_InitStructure);           //???led1???
}


void TimCounterInit(int arr,int psc)        //rate???????
{
        TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
        TIM_OCInitTypeDef  TIM_OCInitStructure;
        
        /* TIM???? */
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

        /* ??????? */
        TIM_TimeBaseStructure.TIM_Period = arr;        //??? 1000;???0~65535
        TIM_TimeBaseStructure.TIM_Prescaler = psc;          //????,?????????,??????                 
        TIM_TimeBaseStructure.TIM_ClockDivision = 0;                //????:????????
        TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;        //????,????
        TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;        //?????

        TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);                //???TIM
        
        /* PWM1 Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;//??????
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//??????
	
				TIM_OCInitStructure.TIM_Pulse = rate; //?????,0?????,1001?????.???0~65535
				TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;//??????;???????/???
        TIM_OC1Init(TIM2, &TIM_OCInitStructure);//????x,????
        TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);//????x??????        
        
				
				
				
        TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;//??????
        TIM_OCInitStructure.TIM_Pulse = rate; //?????,0?????,1001?????.???0~65535
        TIM_OC2Init(TIM2, &TIM_OCInitStructure);//????x,????
        TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);//????x??????
				
				   TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;//??????
        TIM_OCInitStructure.TIM_Pulse = rate; //?????,0?????,1001?????.???0~65535
        TIM_OC3Init(TIM2, &TIM_OCInitStructure);//????x,????
        TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);//????x??????
				
				   TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;//??????
        TIM_OCInitStructure.TIM_Pulse = rate; //?????,0?????,1001?????.???0~65535
        TIM_OC4Init(TIM2, &TIM_OCInitStructure);//????x,????
        TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);//????x??????
        TIM_Cmd(TIM2, ENABLE);                                                //??TIM
}
void uart1_init(u32 bound)    //shumeipai tongxin 
{  	 
	  //GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//使能UGPIOB时钟
  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);	//使能USART1时钟

	//UART3_TX  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);
  	//UART3_RX	  
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  	GPIO_Init(GPIOA, &GPIO_InitStructure);

  	//Usart1 NVIC 配置
  	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
    
	//USART 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
  	USART_Init(USART1, &USART_InitStructure);     //初始化串口3
  	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启串口接受中断
  	USART_Cmd(USART1, ENABLE);                    //使能串口3 
}

/**************************************************************************
函数功能：串口3接收中断
入口参数：无
返回  值：无
**************************************************************************/
u8 USART1_RX_BUF[USART1_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
u16 USART1_RX_STA=0;       //接收状态标记	  
time_t timecount;      
void USART1_IRQHandler(void)
{	
	

	delay_ms(200);

	flag=1;

	char mes[40]="我收到的数据是";
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(自定义与m6通信 接收到的数据必须是0x0d 0x0a结尾，回车换行)
	{
		Res =USART_ReceiveData(USART1);	//读取接收到的数据
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
		if((USART1_RX_STA&0x8000)==0)//接收未完成
		{
			if(Res==0x24)
			{ 
				USART1_RX_STA|=0x8000;	//接收完成了 
				USART1_RX_BUF[USART1_RX_STA&0X3FFF]='\0' ;
			}
			else //还没收到0X0D
			{	
				USART1_RX_BUF[USART1_RX_STA&0X3FFF]=Res ;
				USART1_RX_STA++;
				if(USART1_RX_STA>(USART1_REC_LEN-1))
				USART1_RX_STA=0;//接收数据错误,重新开始接收	  		 
			}
		}
		if((USART1_RX_STA&0x8000)!=0)//接收完成
		{
			//数据已收到
			RS232_1_Send_Data((u8*)mes,14);
			RS232_1_Send_Data(USART1_RX_BUF,USART1_RX_STA&0x00FF);
			RS232_1_Send_Data((u8*)"\n",1);
			USART1_RX_STA=0;//接收数据完成，开始后续处理

		}
	}
}
void sTurnLeft()
{  
         IN1_L IN2_L
         IN3_H IN4_H
         arr=40000;
         psc=35;        
         TimCounterInit(arr,psc);
}
void sTurnRight()
{  

         IN1_H IN2_H
         IN3_L IN4_L
         arr=40000;
         psc=35;        
         TimCounterInit(arr,psc);

}
void move(u8 Res)
{
	
switch(Res)
			{
				case '5':{now_arg+=1;Set_Num_arg(1,now_arg) ; break;}//down
				case '6':{ now_arg-=1;Set_Num_arg(1,now_arg) ;  break;}//up
				case '1':{straight();break;}
					case '2':{sternway();break;}
						case '3':{TurnLeft();break;}
							case '4':{TurnRight();break;}
							case '7':{Left();break;}
								case '8':{Right();break;}
	case 'l':{sTurnLeft();break;}
	case 'r':{sTurnRight();break;}
				case '0':{stop(); break;}
	case 'f':{follow(); break;}
				default : break;
			}

}
/**********************************************************
*	功能：发送数据
*	参数：buf 发送缓冲区首地址
*		  len 待发送的字节数
**********************************************************/
void RS232_1_Send_Data(u8 *buf,u8 len)
{
	u8 t;
  	for(t=0;t<len;t++)		//循环发送数据
	{		   
		while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);	//发送完成  
		USART_SendData(USART1,buf[t]);
	}	 
 
	while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);		
//	RS232_RX_CNT=0;	  
}



unsigned short int UltrasonicWave_Distance;      //计算出的距离    
vu32 averge_distance;
void UltrasonicWave_CalculateTime(void)
{
   UltrasonicWave_Distance=TIM_GetCounter(TIM4)*34/2000;
}
void UltrasonicWave_StartMeasure(void)
{

	while(1)
	{
		if(f==1)
		{
		f=0;
			return;
		}

  GPIO_SetBits(TRIG_PORT,TRIG_PIN); 		  //送>10US的高电平
delay_ms(1);                   //延时20US
  GPIO_ResetBits(TRIG_PORT,TRIG_PIN);

		delay_ms(100);
		
	}
	
	
}

void Auto_run(void)	                                   //检测障碍
{		

	UltrasonicWave_StartMeasure();

	if(UltrasonicWave_Distance<=30)
	{
	     stop();//小车停止

	 }
	
	 else
	{
	
	  straight();
	
	}
}
void TIM4_Int_Init(u16 arr,u16 psc)
{

		
      TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
      NVIC_InitTypeDef NVIC_InitStructure;
	
      RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //????
      //???TIM4???
      TIM_TimeBaseStructure.TIM_Period = arr; //???????????????????????????
      TIM_TimeBaseStructure.TIM_Prescaler =psc; //??????TIMx???????????
     TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //??????:TDTS = Tck_tim
     TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM??????
     TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //??????????TIMx???????
     TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE ); //?????TIM4??,??????


     TIM_Cmd(TIM4, ENABLE); //??TIMx
}
 

 void EXTI_Key_Config(void)
 {

GPIO_InitTypeDef GPIO_InitStructure;
 EXTI_InitTypeDef EXTI_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
 
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;;
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_Init(GPIOB, &GPIO_InitStructure); 
	 
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
 
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_Init(GPIOB, &GPIO_InitStructure); 
 
	 GPIO_ResetBits(GPIOB,GPIO_Pin_8);
	 
 GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource9);
 EXTI_ClearITPendingBit(EXTI_Line9);
 EXTI_InitStructure.EXTI_Line = EXTI_Line9; 
 EXTI_InitStructure.EXTI_LineCmd=ENABLE;
 EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
 
 EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;

 EXTI_Init(&EXTI_InitStructure);
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
 
 NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
 
 NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//qiangzhan
 
 NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;//zhiding
 
 NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	
 NVIC_Init(&NVIC_InitStructure);
 }
 
 
 
 
 
 
 void EXTI9_5_IRQHandler(void)
 {


 if (EXTI_GetITStatus(EXTI_Line9) != RESET) {

	 TIM_Cmd(TIM4, ENABLE);                                             //开启时钟
  while(GPIO_ReadInputDataBit(ECHO_PORT,ECHO_PIN)!=RESET);	                 //等待低电平
  TIM_Cmd(TIM4, DISABLE);			                                       //定时器2失能
  UltrasonicWave_CalculateTime();									 //计算距离
  TIM_SetCounter(TIM4,0);
 
 EXTI_ClearITPendingBit(EXTI_Line9);
	 f=1;
}
 }
void follow()
{
	
	
	flag=0;
TIM4_Int_Init(65535,71);
EXTI_Key_Config();



	while(1)
	{
		delay_ms(500);
	Auto_run();
if(flag==1)
	return;
	}

}
 
int main(void)
{
	LED_Init();
uart1_init(9600);//shumeipai
UART4_Configuration();//duoji
	Gpio_PWM_Init(); 
Gpio_M_Init();	
Set_Num_1();
	Set_Num_arg(1,now_arg);
	

	
//follow();

	while(1)
	{
		if(flag==1)
		{
			flag=0;
			move(Res);
		}

	}	
return 0;
}