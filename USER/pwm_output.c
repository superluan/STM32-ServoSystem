/***************STM32F103C8T6**********************
 *用通道1，4不会影响Uart1见端口复用表
 * 文件名  ：main.c
 * 描述    : 两路PWM
 * 实验平台：STM32F103C8T6
 * 备注    ：使用定时器来产生pwm波
 * 接口    ：TIM1, CH1-PA8, CH4-PA11
 改为TIM1用PA8,PA11端口

****************STM32F103C8T6**********************/
#include "pwm_output.h"
#include "stm32f10x_tim.h"
#include "usart1.h"
#include "math.h"
extern __IO u16 ADC_ConvertedValue[4];
extern int  pwm;

/* 配置TIM1复用输出PWM时用到的I/O */
static void TIM1_GPIO_Config(void) 
{
  GPIO_InitTypeDef GPIO_InitStructure;
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); //复用打开
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);//GPIOB时钟使能//定时器1时钟使能 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8 | GPIO_Pin_11;//固定的映射IO口
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;					//复用推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/*配置TIM1输出的PWM信号的模式，如周期、极性、占空比 */
static void TIM1_Mode_Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	/* PWM信号电平跳变值 */
	u16 CCR1_Val = 0;        
	u16 CCR4_Val = 0;
	//u16 CCR3_Val = 50000;


 /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 3333 - 1;//定时器周期,20ms。共20000.周期0.02s，频率50HZ
  TIM_TimeBaseStructure.TIM_Prescaler = 72 - 1;//时钟预分频
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//计数方式：向上计数
	//TIM_TimeBaseStructure.TIM_RepetitionCounter =1 ;//使用寄存器20180411
	//TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1; //死区控制
  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

  /* PWM1 Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;//选择定时器模式2，计数比较模式什么时候是无效电平
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//比较输出使能
  TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
  TIM_OCInitStructure.TIM_Pulse = CCR1_Val;//外部比较值，决定占空比
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;//极性
  TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;//输出同向，若High时，输出反相。
  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;
	
  TIM_OC1Init(TIM1, &TIM_OCInitStructure);
	
	TIM_OCInitStructure.TIM_Pulse = CCR4_Val;//外部比较值，决定占空比
	TIM_OC4Init(TIM1, &TIM_OCInitStructure);
	
  TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
  TIM_ARRPreloadConfig(TIM1, ENABLE);
	
	
	//TIM_SelectOutputTrigger(TIM1,TIM_TRGOSource_Update);//设定Timer1的输出触发信号20180411

  TIM_Cmd(TIM1, ENABLE);
  TIM_CtrlPWMOutputs(TIM1, ENABLE);	

}

/*配置和初始化TIM1 */
void TIM1_PWM_Init(void)
{
	TIM1_GPIO_Config();
	TIM1_Mode_Config();	
}

//粗略延时 不精确！
void Delay_us(unsigned int i)
{
	unsigned char t = 0;
	for(;i>0;i--)
	{
		for(t=0;t<10;t++)
		{
		}
	}
}

void Delay_ms(unsigned int i)
{
	for(;i>0;i--)
	{
		Delay_us(1000);
	}
}

void Servo_outer (void)//无FOR循环
{
	TIM1->CCR1 = 10000;//对该变量的初始化 值为0，pwm波高电平持续时间，2ms.
	TIM1->CCR2 = 0;
}
void Servo_out (void)//有FOR循环
{
	unsigned int i;
	TIM1->CCR1 = 0;//对该变量的初始化 值为0，pwm波高电平持续时间，2ms.
	TIM1->CCR2 = 0;
	
/*	while(1)
	{*/
		for(i=0;i<=100;i++)
		{
			TIM1->CCR1 = 200*i;
			TIM1->CCR2 = 0;//每次高电平持续时间增加，表现为不稳定波形，待修改，！此处是LED的程序，所以没有采用精确的时间控制，不可用
			Delay_ms(20);//10MS延迟
		}
	  //Delay_ms(1000);
		for(i=0;i<=100;i++)
		{
			TIM1->CCR1 = 200*(100-i);
			TIM1->CCR2 = 0;
			Delay_ms(20);
		}
		//Delay_ms(1000);
		
		for(i=0;i<=100;i++)
		{
			TIM1->CCR1 = 0;
			TIM1->CCR2 = 200*i;//每次高电平持续时间增加，表现为不稳定波形，待修改，！此处是LED的程序，所以没有采用精确的时间控制，不可用
			Delay_ms(20);//10MS延迟
		}
	  //Delay_ms(1000);
		for(i=0;i<=100;i++)
		{
			TIM1->CCR1 = 0;
			TIM1->CCR2 = 200*(100-i);
			Delay_ms(20);
		}

}

void setMotorSpeed(int mySpeed)
{
		unsigned  int output = 0;
    //mySpeed = constrain(mySpeed,-41665,41665);  //限制速度上下限，限制转动最高速度
		if(mySpeed == 0)
			mySpeed = 1;
		if(mySpeed>0)
			{                               
        output = 1650;                  		//这个看具体的系统进行放大
        TurnLeft(output);//顺时针
			}
		else
			{
        output = 1650;
				TurnRight(output);//逆时针
			}
}

void TurnLeft(int output)
{
				pwm =  output;
				TIM1->CCR1 = pwm;//最低2300才会运动，可以考虑在一定误差内抵消。
				TIM1->CCR4 =  0;
}
void TurnRight(int output)
{
				pwm =  output;//占空比有问题，不能像Left一样增量
				TIM1->CCR1 = 0;
				TIM1->CCR4 = pwm;
}

void keep()
{
				TIM1->CCR1 = 0;
				TIM1->CCR4 = 0;
}

int constrain(int myspeed,int min,int max)//限制速度
{
if (myspeed>max)
	myspeed = max;
else if (myspeed<min)
	myspeed = min;
else
	myspeed = myspeed;
return myspeed;
}

void test_moto(int chazhi,int kv )
{
				//测试电机用的函数
			chazhi = kv - ADC_ConvertedValue[1];
			if (chazhi > 100)
			{
				TIM1->CCR1 = chazhi*10;
				TIM1->CCR4 =  0;
			}
			else if (chazhi < -100)
			{
				TIM1->CCR1 = 0;
				TIM1->CCR4 = (0-chazhi)*10;
			}
			else
			{
				TIM1->CCR1 = 0;
				TIM1->CCR4 = 0;
			}
}
