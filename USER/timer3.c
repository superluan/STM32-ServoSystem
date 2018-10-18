/***************STM32F103C8T6**********************
 * �ļ���  ��timer3.c
 * ����    : TIM3��ʱ��
 * ʵ��ƽ̨��STM32F103C8T6
 * ��ע    ��tim3��ʱ��ģʽ
 * �ӿ�    ��

********************LIGEN*************************/
#include "timer3.h"
#include "misc.h"

/* TIM3�ж����ȼ����� */
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


/*�ж�����Ϊ1ms*/
void TIM3_Configuration(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 , ENABLE);//����RCC
    TIM_DeInit(TIM3);
    TIM_TimeBaseStructure.TIM_Period = 3333-1;	//�Զ���װ�ؼĴ������ڵ�ֵ1MS1000-1
    /* �ۼ� TIM_Period��Ƶ�ʺ����һ�����»����ж� */
    TIM_TimeBaseStructure.TIM_Prescaler = 72-1;	//ʱ��Ԥ��Ƶ�� 72M/72
    TIM_TimeBaseStructure.TIM_ClockDivision = 0x00;//  
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ 
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
		TIM_TimeBaseStructure.TIM_RepetitionCounter =1;//201804111 
		TIM_SelectOutputTrigger(TIM3,TIM_TRGOSource_Update);//20180411
	
    TIM_ClearFlag(TIM3, TIM_FLAG_Update);	// �������жϱ�־ 
    TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
    TIM_Cmd(TIM3, ENABLE);	// ����ʱ��    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 , DISABLE);	//�ȹرյȴ�ʹ��  
}

