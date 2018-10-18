/***************STM32F103C8T6*********************
 * 文件名  ：main.c
 * 描述    : 舵机控制，测量
 * 实验平台：STM32F103C8T6
 * 备注    ：天津大学舵机内部控制程序
 * 接口    ：
 * BUG统计 ：
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

volatile uint8_t aRxBuffer[100]={0x00};//可拓展定义（串口接收不定长）无符号字符
volatile uint8_t position[10]={0x00};
volatile uint8_t speed[10]={0x00};
volatile uint8_t step[10]={0x00};
volatile uint8_t RxCounter=0;
volatile uint8_t ReceiveState=0;
uint8_t  i2=0;
int Q = 0;
extern __IO u16 ADC_ConvertedValue[4];
volatile u32 time3;						 	// ms 计时变量
double tem;
int temr;
unsigned int kv = 2056;
int chazhi = 0;
char s=0;
u16 avg_temp = 0;
volatile unsigned long Dime3 = 1;	//当前时间0~65535，需要在每一次的控制结束后清0
unsigned int Dime = 25;						//规划时间，每50代表一秒
char* str_temp =(char*)position;//char型指针,使用在it。c里
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
  SystemInit();							//配置系统时钟为72M	
  USART1_Config();					//配置串口
  ADC1_Init();							//初始化adc
	LED_GPIO_Config();				//led初始化
	TIM3_NVIC_Configuration();//TIM3初始化，做计数器
	TIM3_Configuration();
	START_TIME3;
	TIM1_PWM_Init(); 						//TIM1 PWM波输出初始化，并使能TIM1 PWM输出，做定时器
	//ParaSet(2.25,0.01,0);      //PID参数设置
	
  while (1)
  {
		//HEX开始接收模式，可判别数据完整性，接收可处理可发回上位机，目前可直接接收0X800类数据类型
		if(ReceiveState==1)//如果接收到1帧数据
		{
			ReceiveState=0;
			i2=0;
			while(RxCounter--)// 把接收到数据发送回串口
			{
				//USART_SendData(USART1, aRxBuffer[i2++]);	//发送回去,0xed8
				while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
			}
				RxCounter=0;
		}		
		
			//速度要求较快的即时轨迹规划、控制代码
		/*	trail = theat(ADC_ConvertedValue[1],str_temp_dec,Dime,Dime3);//被追踪的轨迹，目标轨迹，最终时间，当前时间
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
			PidFunction(trail2,ADC_ConvertedValue[1],Dime3);//三个参数：目标值，当前输入，时间
			
		while(time3 == 1)//计数器，20ms,增加时间会使PID速度变慢
		{   
			//40-320代码，可以调整间隔
			/*i40to320++;
			if(i40to320<=350)
			{
				str_temp_dec = 520;//40度
			}
			else if((i40to320>350)&&(i40to320<710))
				str_temp_dec = 3520;//320度
			else
			{
				i40to320 = 0;
			}*/
			
			float_time = float_time + 0.01;
	
			Dime3++;//每一次循环时间值加一		
			//str_temp_dec = charhex_to_dec(str_temp);//指令转化1目标位置
			//int_speed = charhex_to_dec(str_speed);//指令转化2平均
			//int_step = charhex_to_dec(str_step);//指令转化3分多少步执行
			if(str_temp_dec!=0)//如果有指令
			{
				str_temp_dec = xianfu(str_temp_dec);	//指令限幅				
			}
			else
			{
				str_temp_dec = 2048;
			}
			//使每次一都从t=0规划
			if (Dime3 > Dime-1)//100*20=2000,2秒
			{
				Dime3 = Dime;
				if((last_str_temp_dec - str_temp_dec) == 0)//时间归0代码
				{
					Dime3 = Dime;
				}
				else
				{
					Dime3 = 0;
				}
			}
			last_str_temp_dec = str_temp_dec;//保存上一次的命令
			
			keepp++;
			rrr = ADC_ConvertedValue[0];
			if(keepp>6)//防止过快发送
			{
			/****************串口*********************/
			printf("1000");
    	printf(",%d",ADC_ConvertedValue[0]);				//MAX471
			printf(",%d",ADC_ConvertedValue[1]);				//电位器，当前位置
			printf(",%d",gaol);//目标位置,只要这句打印不为0，就会对数据产生干扰，不论是常数还是gaol
			tem = (float)ADC_ConvertedValue[3]*3.3/4096;//转化为电压值
			tem = (1.43 - tem)/0.0042+25;								//温度值
			temr = tem/1;
			printf(",%d",temr);
			printf(",%d",pwm);
			printf(",1000");
			//闪灯观察状态
			LED_Toggle(); 
			keepp =0;
		  }	
			time3 = 0;
		}		
	}
}	

