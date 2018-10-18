/***************STM32F103C8T6**********************
 *��ͨ��1��4����Ӱ��Uart1���˿ڸ��ñ�
 * �ļ���  ��main.c
 * ����    : ��·PWM
 * ʵ��ƽ̨��STM32F103C8T6
 * ��ע    ��ʹ�ö�ʱ��������pwm��
 * �ӿ�    ��TIM1, CH1-PA8, CH4-PA11
 ��ΪTIM1��PA8,PA11�˿�

****************STM32F103C8T6**********************/
#include "pwm_output.h"
#include "stm32f10x_tim.h"
#include "usart1.h"
#include "math.h"
extern __IO u16 ADC_ConvertedValue[4];
extern int  pwm;

/* ����TIM1�������PWMʱ�õ���I/O */
static void TIM1_GPIO_Config(void) 
{
  GPIO_InitTypeDef GPIO_InitStructure;
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); //���ô�
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);//GPIOBʱ��ʹ��//��ʱ��1ʱ��ʹ�� 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8 | GPIO_Pin_11;//�̶���ӳ��IO��
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;					//�����������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/*����TIM1�����PWM�źŵ�ģʽ�������ڡ����ԡ�ռ�ձ� */
static void TIM1_Mode_Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	/* PWM�źŵ�ƽ����ֵ */
	u16 CCR1_Val = 0;        
	u16 CCR4_Val = 0;
	//u16 CCR3_Val = 50000;


 /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 3333 - 1;//��ʱ������,20ms����20000.����0.02s��Ƶ��50HZ
  TIM_TimeBaseStructure.TIM_Prescaler = 72 - 1;//ʱ��Ԥ��Ƶ
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//������ʽ�����ϼ���
	//TIM_TimeBaseStructure.TIM_RepetitionCounter =1 ;//ʹ�üĴ���20180411
	//TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1; //��������
  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

  /* PWM1 Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;//ѡ��ʱ��ģʽ2�������Ƚ�ģʽʲôʱ������Ч��ƽ
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//�Ƚ����ʹ��
  TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
  TIM_OCInitStructure.TIM_Pulse = CCR1_Val;//�ⲿ�Ƚ�ֵ������ռ�ձ�
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;//����
  TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;//���ͬ����Highʱ��������ࡣ
  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;
	
  TIM_OC1Init(TIM1, &TIM_OCInitStructure);
	
	TIM_OCInitStructure.TIM_Pulse = CCR4_Val;//�ⲿ�Ƚ�ֵ������ռ�ձ�
	TIM_OC4Init(TIM1, &TIM_OCInitStructure);
	
  TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
  TIM_ARRPreloadConfig(TIM1, ENABLE);
	
	
	//TIM_SelectOutputTrigger(TIM1,TIM_TRGOSource_Update);//�趨Timer1����������ź�20180411

  TIM_Cmd(TIM1, ENABLE);
  TIM_CtrlPWMOutputs(TIM1, ENABLE);	

}

/*���úͳ�ʼ��TIM1 */
void TIM1_PWM_Init(void)
{
	TIM1_GPIO_Config();
	TIM1_Mode_Config();	
}

//������ʱ ����ȷ��
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

void Servo_outer (void)//��FORѭ��
{
	TIM1->CCR1 = 10000;//�Ըñ����ĳ�ʼ�� ֵΪ0��pwm���ߵ�ƽ����ʱ�䣬2ms.
	TIM1->CCR2 = 0;
}
void Servo_out (void)//��FORѭ��
{
	unsigned int i;
	TIM1->CCR1 = 0;//�Ըñ����ĳ�ʼ�� ֵΪ0��pwm���ߵ�ƽ����ʱ�䣬2ms.
	TIM1->CCR2 = 0;
	
/*	while(1)
	{*/
		for(i=0;i<=100;i++)
		{
			TIM1->CCR1 = 200*i;
			TIM1->CCR2 = 0;//ÿ�θߵ�ƽ����ʱ�����ӣ�����Ϊ���ȶ����Σ����޸ģ����˴���LED�ĳ�������û�в��þ�ȷ��ʱ����ƣ�������
			Delay_ms(20);//10MS�ӳ�
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
			TIM1->CCR2 = 200*i;//ÿ�θߵ�ƽ����ʱ�����ӣ�����Ϊ���ȶ����Σ����޸ģ����˴���LED�ĳ�������û�в��þ�ȷ��ʱ����ƣ�������
			Delay_ms(20);//10MS�ӳ�
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
    //mySpeed = constrain(mySpeed,-41665,41665);  //�����ٶ������ޣ�����ת������ٶ�
		if(mySpeed == 0)
			mySpeed = 1;
		if(mySpeed>0)
			{                               
        output = 1650;                  		//����������ϵͳ���зŴ�
        TurnLeft(output);//˳ʱ��
			}
		else
			{
        output = 1650;
				TurnRight(output);//��ʱ��
			}
}

void TurnLeft(int output)
{
				pwm =  output;
				TIM1->CCR1 = pwm;//���2300�Ż��˶������Կ�����һ������ڵ�����
				TIM1->CCR4 =  0;
}
void TurnRight(int output)
{
				pwm =  output;//ռ�ձ������⣬������Leftһ������
				TIM1->CCR1 = 0;
				TIM1->CCR4 = pwm;
}

void keep()
{
				TIM1->CCR1 = 0;
				TIM1->CCR4 = 0;
}

int constrain(int myspeed,int min,int max)//�����ٶ�
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
				//���Ե���õĺ���
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
