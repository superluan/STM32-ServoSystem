/***************STM32F103C8T6*********************
 * �ļ���  ��main.c
 * ����    : ������ƣ�����
 * ʵ��ƽ̨��STM32F103C8T6
 * ��ע    ������ѧ����ڲ����Ƴ���
 * �ӿ�    ��
 * BUGͳ�� ��
********************LIGEN*************************/
#include "stm32f10x.h"
#include "adc.h" 
#include "led.h"
#include "usart1.h"
#include "timer3.h"
#include "pwm_output.h"
#include "Wave_filtering.h"
#include "PID.h"
#include "stdlib.h"
#include "str2dec.h"
#define N 12 

volatile uint8_t aRxBuffer[100]={0x00};//����չ���壨���ڽ��ղ��������޷����ַ�
volatile uint8_t position[10]={0x00};
volatile uint8_t speed[10]={0x00};
volatile uint8_t step[10]={0x00};
volatile uint8_t RxCounter=0;
volatile uint8_t ReceiveState=0;
uint8_t  i2=0;
int Q = 0;
extern __IO u16 ADC_ConvertedValue[4];
volatile u32 time3;						 	// ms ��ʱ����
double tem;
int temr;
unsigned int kv = 2056;
int chazhi = 0;
char s=0;
u16 avg_temp = 0;
volatile unsigned long Dime3 = 1;	//��ǰʱ��0~65535����Ҫ��ÿһ�εĿ��ƽ�������0
unsigned int Dime = 25;						//�滮ʱ�䣬ÿ50����һ��
char* str_temp =(char*)position;//char��ָ��,ʹ����it��c��
char* str_speed =(char*)speed;
char* str_step =(char*)step;
int 	str_temp_dec = 2048;
int 	int_speed = 0;
int 	int_step = 0;
int   last_str_temp_dec = 2048;
int gaol = 0;
volatile int  pwm = 0;
int trail;
int trail2;
int i40to320;
int keepp=0;
int max471;
int rrr;
int max4710=0;
float pi = 3.14;
volatile float float_time=0.0;


int main(void)
{  
  SystemInit();							//����ϵͳʱ��Ϊ72M	
  USART1_Config();					//���ô���
  ADC1_Init();							//��ʼ��adc
	LED_GPIO_Config();				//led��ʼ��
	TIM3_NVIC_Configuration();//TIM3��ʼ������������
	TIM3_Configuration();
	START_TIME3;
	TIM1_PWM_Init(); 						//TIM1 PWM�������ʼ������ʹ��TIM1 PWM���������ʱ��
	//ParaSet(2.25,0.01,0);      //PID��������
	
  while (1)
  {
		//HEX��ʼ����ģʽ�����б����������ԣ����տɴ���ɷ�����λ����Ŀǰ��ֱ�ӽ���0X800����������
		if(ReceiveState==1)//������յ�1֡����
		{
			ReceiveState=0;
			i2=0;
			while(RxCounter--)// �ѽ��յ����ݷ��ͻش���
			{
				//USART_SendData(USART1, aRxBuffer[i2++]);	//���ͻ�ȥ,0xed8
				while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
			}
				RxCounter=0;
		}		
		
			//�ٶ�Ҫ��Ͽ�ļ�ʱ�켣�滮�����ƴ���
		/*	trail = theat(ADC_ConvertedValue[1],str_temp_dec,Dime,Dime3);//��׷�ٵĹ켣��Ŀ��켣������ʱ�䣬��ǰʱ��
			trail2 = ssin(520,3520,pi*2,float_time);
		if(int_speed==0)
		{
			gaol = trail;	
		}
		else
		{
			gaol = trail2;
		}*/
			trail2 = ssin(520,3520,pi*2,float_time);
			gaol = 0;
			PidFunction(trail2,ADC_ConvertedValue[1],Dime3);//����������Ŀ��ֵ����ǰ���룬ʱ��
			
		while(time3 == 1)//��������20ms,����ʱ���ʹPID�ٶȱ���
		{   
			//40-320���룬���Ե������
			/*i40to320++;
			if(i40to320<=350)
			{
				str_temp_dec = 520;//40��
			}
			else if((i40to320>350)&&(i40to320<710))
				str_temp_dec = 3520;//320��
			else
			{
				i40to320 = 0;
			}*/
			
			float_time = float_time + 0.01;
	
			Dime3++;//ÿһ��ѭ��ʱ��ֵ��һ		
			//str_temp_dec = charhex_to_dec(str_temp);//ָ��ת��1Ŀ��λ��
			//int_speed = charhex_to_dec(str_speed);//ָ��ת��2ƽ��
			//int_step = charhex_to_dec(str_step);//ָ��ת��3�ֶ��ٲ�ִ��
			if(str_temp_dec!=0)//�����ָ��
			{
				str_temp_dec = xianfu(str_temp_dec);	//ָ���޷�				
			}
			else
			{
				str_temp_dec = 2048;
			}
			//ʹÿ��һ����t=0�滮
			if (Dime3 > Dime-1)//100*20=2000,2��
			{
				Dime3 = Dime;
				if((last_str_temp_dec - str_temp_dec) == 0)//ʱ���0����
				{
					Dime3 = Dime;
				}
				else
				{
					Dime3 = 0;
				}
			}
			last_str_temp_dec = str_temp_dec;//������һ�ε�����
			
			keepp++;
			rrr = ADC_ConvertedValue[0];
			if(keepp>6)//��ֹ���췢��
			{
			/****************����*********************/
			printf("1000");
    	printf(",%d",ADC_ConvertedValue[0]);				//MAX471
			printf(",%d",ADC_ConvertedValue[1]);				//��λ������ǰλ��
			printf(",%d",gaol);//Ŀ��λ��,ֻҪ����ӡ��Ϊ0���ͻ�����ݲ������ţ������ǳ�������gaol
			tem = (float)ADC_ConvertedValue[3]*3.3/4096;//ת��Ϊ��ѹֵ
			tem = (1.43 - tem)/0.0042+25;								//�¶�ֵ
			temr = tem/1;
			printf(",%d",temr);
			printf(",%d",pwm);
			printf(",1000");
			//���ƹ۲�״̬
			LED_Toggle(); 
			keepp =0;
		  }	
			time3 = 0;
		}		
	}
}	

