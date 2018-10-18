/***************STM32F103C8T6**********************
 * �ļ���  ��usart1.c
 * ����    ����printf�����ض���USART1��        
 * ʵ��ƽ̨��STM32F103C8T6
 * Ӳ�����ӣ�------------------------
 *          | PA9  - USART1(Tx)      |
 *          | PA10 - USART1(Rx)      |
 *           ------------------------
 * ��汾  ��ST3.0.0  *

********************LIGEN*************************/

#include "usart1.h"
#include "stdarg.h"
#include "misc.h"

unsigned char words = 0; 

void USART1_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure; 
	/* ʹ�� USART1 ʱ��*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE); 
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); //���ô�
	/* USART1 ʹ��IO�˿����� */    
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //�����������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);    
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//��������
  GPIO_Init(GPIOA, &GPIO_InitStructure);   //��ʼ��GPIOA
	  
	/* USART1 ����ģʽ���� */
	USART_InitStructure.USART_BaudRate = 115200;	//���������ã�115200
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//����λ�����ã�8λ
	USART_InitStructure.USART_StopBits = USART_StopBits_1; 	//ֹͣλ���ã�1λ
	USART_InitStructure.USART_Parity = USART_Parity_No ;  //�Ƿ���żУ�飺��
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//Ӳ��������ģʽ���ã�û��ʹ��
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//�����뷢�Ͷ�ʹ��
	USART_Init(USART1, &USART_InitStructure);  //��ʼ��USART1
	//USART_Cmd(USART1, ENABLE);// USART1ʹ��
	
		/* �򿪿����ж� */
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);
	/* �򿪽����ж� */
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
	/* ����NVIC���ȼ��� */  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	
	/* ����UART1�ж� */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;	 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	USART_ClearFlag(USART1,USART_FLAG_TC);
	
	USART_Cmd(USART1, ENABLE);// �򿪴���1
}

// ����һ���ֽ�
void USART1_Send_byte(uint8_t val)
{
	USART_SendData(USART1, val);	
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);	//�ȴ��������	
}

// ����һ���ֽ�
uint8_t USART1_Recv_byte(void)
{
	while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
	return USART_ReceiveData(USART1);
}

 /* ����  ���ض���c�⺯��printf��USART1*/ 
int fputc(int ch, FILE *f)
{
	//while(USART_GetFlagStatus(USART1, USART_FLAG_TC) != SET);
/* ��Printf���ݷ������� */
  USART_SendData(USART1, (unsigned char) ch);
  while (!(USART1->SR & USART_FLAG_TXE));
  return (ch);
}

//���ڽ��մ�����λ��
 /** ��������Delay
 * ����  ��1us,1ms��ʱ����
 * ����  ��Time
 * ���  ����	
 */
void Delayus1(int Time)    
{
   unsigned char i;
   for ( ; Time>0; Time--)
     for ( i = 0; i < 72; i++ );
}

void Delayms1(int time)
{
	for(;time>0; time--)
	{
		Delayus1(1000);
	}
}


 /*����һ���ֽ�����*/
 void UART1SendByte(unsigned char SendData)
{	   
				USART_ClearFlag(USART1,USART_FLAG_TC);  
				//�ڷ��͵�һ������ǰ�Ӵ˾䣬�����һ�����ݲ����������͵�����
        USART_SendData(USART1,SendData);
        while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);	    
}  

/*����һ���ֽ�����*/
unsigned char UART1GetByte(unsigned char* GetData)
{   	   
        if(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET)
        {  return 0;//û���յ����� 
		}
        *GetData = USART_ReceiveData(USART1); 
        return 1;//�յ�����
}

/*����һ�����ݣ����Ϸ��ؽ��յ����������*/
unsigned char UART1Test(void)
{ 	
 		 while(UART1GetByte(&words))
        {
         USART_SendData(USART1,words);
        }       
				return words;
}
