#include "delay.h"

static u16 SysTick_each_us_CNT ;	// SysTick��ʱ1us����ļ�������
static u16 SysTick_each_ms_CNT ;	// SysTick��ʱ1ms����ļ�������

// ��ʼ��SysTick��ʱ��
void delay_Init(void)
{
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);	// SysTick��ʱ��ѡΪ�ⲿʱ�ӵ�8��Ƶ��HCLK/8��
	SysTick_each_us_CNT = SystemCoreClock / 8 / 1000000;	// �õ�1us�ļ�������
	SysTick_each_ms_CNT = SysTick_each_us_CNT * 1000;		// �õ�1ms�ļ�������
}


u8 delay_us(u32 delay_us_Num)
{		
	u8  NOP_CNT = 4 ;
	u32 T_SysTick_CTR ;
	if( delay_us_Num == 0 )		// ����������Ϊ0
	{ return 0 ; }				// ��������
	if( delay_us_Num == 1 )
	{
		while( --NOP_CNT )		// NOP_CNT = 4 ;
		{
			__NOP();			// NOP��һ��ָ�����ڵĿղ���
		}
		return 0 ;				// ��������
	}
	if( delay_us_Num>1 && delay_us_Num<1000 )
	{ delay_us_Num -- ; }
	SysTick->LOAD = (u32)SysTick_each_us_CNT * delay_us_Num ;			// ���˴κ����ļ�������д����װ�ؼĴ���
	SysTick->VAL = 0x00; 						// ��SysTick->VAL��0���Ӷ�ʹ��SysTick->LOAD�е�ֵװ��SysTick->VAL
	SysTick->CTRL |= (u32)0x01 ;				// ʹ��SysTick��ʱ������������ʼ�ݼ�
	do{
		T_SysTick_CTR = SysTick->CTRL ;			// ��SysTick->CTRL�е�ֵ���浽T_SysTick_CTR������
	}while( (T_SysTick_CTR & 0x01) && !(T_SysTick_CTR & (1<<16) ) );	// ��SysTick->CTRL�����λΪ1���ҵ�16λΪ0ʱ������ѭ��
	SysTick->CTRL &= ~ ( (u32)0x01 ) ;			// ʧ��SysTick��ʱ��
	SysTick->VAL = 0X00;      					// ��0������
	return 0 ;	// ��������
}

u8 delay_ms(u16 delay_ms_Num)
{	 		  	  
	u32 T_SysTick_CTR ;
	
	
	if( delay_ms_Num == 0 )		// ����������Ϊ0
	{ return 0 ; }				// ��������

	SysTick->LOAD = (u32)SysTick_each_ms_CNT * delay_ms_Num ;			// ���˴κ����ļ�������д����װ�ؼĴ�����
	
	SysTick->VAL = 0x00; 						// ��SysTick->VAL��0���Ӷ�ʹ��SysTick->LOAD�е�ֵװ��SysTick->VAL
	SysTick->CTRL |= (u32)0x01 ;				// ʹ��SysTick��ʱ������������ʼ�ݼ�
	do{
		T_SysTick_CTR = SysTick->CTRL ;			// ��SysTick->CTRL�е�ֵ���浽T_SysTick_CTR������
		
	}while( (T_SysTick_CTR & 0x01) && !(T_SysTick_CTR & (1<<16) ) );	// ��SysTick->CTRL�����λΪ1���ҵ�16λΪ0ʱ������ѭ��
	
	
	SysTick->CTRL &= ~ ( (u32)0x01 ) ;			// ʧ��SysTick��ʱ��
	
	SysTick->VAL = 0X00;      					// ��ռ�����
	
	return 0 ;	// ��������
}
void delay_nms(u16 time)
{
	u16 i=0;
	while(time--)
	{
		i=2000;
		while(i--);
	}
}
void Delay_Ms(u16 time)  
{ 
    uint16_t i,j;
    for(i=0;i<time;i++)
          for(j=0;j<10260;j++);
}
void delay_nus(u16 time)  
{ 
    uint16_t i,j;
    for(i=0;i<time;i++)
          for(j=0;j<9;j++);
}
