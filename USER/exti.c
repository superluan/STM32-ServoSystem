/************************************
 * �ļ���  ��exti.c
 * ����    �������ⲿ�жϵķ�ʽ���е������  
 * ʵ��ƽ̨��MINI STM32������ ����STM32F103C8T6
 * Ӳ�����ӣ� PB0��Ϊ�ⲿ�ж�����ӿ� 
 * ��汾  ��ST3.0.0 
 * ��ע��

**********************************************************************************/
#include "exti.h"
#include "misc.h"


void EXTI0_GPIO_Config()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;//ѡ��Pin0
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//���Ƶ�����50MHz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
	GPIO_Init(GPIOB,&GPIO_InitStructure); 
}

void EXTI0_Config()
{
	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_ClearITPendingBit(EXTI_Line0);	//����жϱ�־

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource0);//��EXTI0����PB0����

	EXTI_InitStructure.EXTI_Line = EXTI_Line0;//ѡ���ж���·
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt; //����Ϊ�ж����󣬷��¼�����
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //�͵�ƽ����
	EXTI_InitStructure.EXTI_LineCmd = ENABLE; //�ⲿ�ж�ʹ��
	EXTI_Init(&EXTI_InitStructure);
}

void EXTI0_NVIC_Config()
{
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);//��GPIOB�Ķ˿�ʱ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//ѡ���жϷ���           
        
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;//ѡ���ж�ͨ��0��Pin������ͨ����
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //��ռʽ�ж����ȼ�Ϊ0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;//��Ӧ�ж����ȼ�Ϊ0     
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//ʹ���ж�                               
	NVIC_Init(&NVIC_InitStructure);             
}

void EXTI0_Init()
{
	EXTI0_GPIO_Config();
	EXTI0_NVIC_Config();
	EXTI0_Config();
}

