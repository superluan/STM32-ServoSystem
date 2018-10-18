/***************STM32F103C8T6**********************
 * 文件名  ：timer3.c
 * 描述    : TIM3定时器
 * 实验平台：STM32F103C8T6
 * 备注    ：tim3定时器模式
 * 接口    ：

********************LIGEN*************************/
#include "timer3.h"
#include "misc.h"

/* TIM3中断优先级配置 */
void TIM3_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);  													
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;	  
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}


/*中断周期为1ms*/
void TIM3_Configuration(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 , ENABLE);//设置RCC
    TIM_DeInit(TIM3);
    TIM_TimeBaseStructure.TIM_Period = 3333-1;	//自动重装载寄存器周期的值1MS1000-1
    /* 累计 TIM_Period个频率后产生一个更新或者中断 */
    TIM_TimeBaseStructure.TIM_Prescaler = 72-1;	//时钟预分频数 72M/72
    TIM_TimeBaseStructure.TIM_ClockDivision = 0x00;//  
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式 
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
		TIM_TimeBaseStructure.TIM_RepetitionCounter =1;//201804111 
		TIM_SelectOutputTrigger(TIM3,TIM_TRGOSource_Update);//20180411
	
    TIM_ClearFlag(TIM3, TIM_FLAG_Update);	// 清除溢出中断标志 
    TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
    TIM_Cmd(TIM3, ENABLE);	// 开启时钟    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 , DISABLE);	//先关闭等待使用  
}

