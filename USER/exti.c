/************************************
 * 文件名  ：exti.c
 * 描述    ：采用外部中断的方式进行电机测速  
 * 实验平台：MINI STM32开发板 基于STM32F103C8T6
 * 硬件连接： PB0作为外部中断输入接口 
 * 库版本  ：ST3.0.0 
 * 备注：

**********************************************************************************/
#include "exti.h"
#include "misc.h"


void EXTI0_GPIO_Config()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;//选择Pin0
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//输出频率最大50MHz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOB,&GPIO_InitStructure); 
}

void EXTI0_Config()
{
	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_ClearITPendingBit(EXTI_Line0);	//清除中断标志

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource0);//将EXTI0线与PB0连接

	EXTI_InitStructure.EXTI_Line = EXTI_Line0;//选择中断线路
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt; //设置为中断请求，非事件请求
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //低电平触发
	EXTI_InitStructure.EXTI_LineCmd = ENABLE; //外部中断使能
	EXTI_Init(&EXTI_InitStructure);
}

void EXTI0_NVIC_Config()
{
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);//打开GPIOB的端口时钟
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//选择中断分组           
        
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;//选择中断通道0，Pin几就是通道几
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //抢占式中断优先级为0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;//响应中断优先级为0     
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能中断                               
	NVIC_Init(&NVIC_InitStructure);             
}

void EXTI0_Init()
{
	EXTI0_GPIO_Config();
	EXTI0_NVIC_Config();
	EXTI0_Config();
}

