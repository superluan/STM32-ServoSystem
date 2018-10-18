/***************STM32F103C8T6**********************
 * 文件名  ：usart1.c
 * 描述    ：将printf函数重定向到USART1。        
 * 实验平台：STM32F103C8T6
 * 硬件连接：------------------------
 *          | PA9  - USART1(Tx)      |
 *          | PA10 - USART1(Rx)      |
 *           ------------------------
 * 库版本  ：ST3.0.0  *

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
	/* 使能 USART1 时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE); 
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); //复用打开
	/* USART1 使用IO端口配置 */    
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);    
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//浮空输入
  GPIO_Init(GPIOA, &GPIO_InitStructure);   //初始化GPIOA
	  
	/* USART1 工作模式配置 */
	USART_InitStructure.USART_BaudRate = 115200;	//波特率设置：115200
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//数据位数设置：8位
	USART_InitStructure.USART_StopBits = USART_StopBits_1; 	//停止位设置：1位
	USART_InitStructure.USART_Parity = USART_Parity_No ;  //是否奇偶校验：无
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//硬件流控制模式设置：没有使能
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//接收与发送都使能
	USART_Init(USART1, &USART_InitStructure);  //初始化USART1
	//USART_Cmd(USART1, ENABLE);// USART1使能
	
		/* 打开空闲中断 */
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);
	/* 打开接收中断 */
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
	/* 配置NVIC优先级组 */  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	
	/* 允许UART1中断 */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;	 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	USART_ClearFlag(USART1,USART_FLAG_TC);
	
	USART_Cmd(USART1, ENABLE);// 打开串口1
}

// 发送一个字节
void USART1_Send_byte(uint8_t val)
{
	USART_SendData(USART1, val);	
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);	//等待发送完成	
}

// 接收一个字节
uint8_t USART1_Recv_byte(void)
{
	while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
	return USART_ReceiveData(USART1);
}

 /* 描述  ：重定向c库函数printf到USART1*/ 
int fputc(int ch, FILE *f)
{
	//while(USART_GetFlagStatus(USART1, USART_FLAG_TC) != SET);
/* 将Printf内容发往串口 */
  USART_SendData(USART1, (unsigned char) ch);
  while (!(USART1->SR & USART_FLAG_TXE));
  return (ch);
}

//串口接收处理函数位置
 /** 函数名：Delay
 * 描述  ：1us,1ms延时函数
 * 输入  ：Time
 * 输出  ：无	
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


 /*发送一个字节数据*/
 void UART1SendByte(unsigned char SendData)
{	   
				USART_ClearFlag(USART1,USART_FLAG_TC);  
				//在发送第一个数据前加此句，解决第一个数据不能正常发送的问题
        USART_SendData(USART1,SendData);
        while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);	    
}  

/*接收一个字节数据*/
unsigned char UART1GetByte(unsigned char* GetData)
{   	   
        if(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET)
        {  return 0;//没有收到数据 
		}
        *GetData = USART_ReceiveData(USART1); 
        return 1;//收到数据
}

/*接收一个数据，马上返回接收到的这个数据*/
unsigned char UART1Test(void)
{ 	
 		 while(UART1GetByte(&words))
        {
         USART_SendData(USART1,words);
        }       
				return words;
}
