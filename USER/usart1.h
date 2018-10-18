#ifndef __USART1_H
#define	__USART1_H

#include "stm32f10x.h"
#include "stdio.h"
#include "string.h"
unsigned char UART1Test(void);
void USART1_Config(void);
int fputc(int ch, FILE *f);
void Delayus1(int Time) ;
void Delayms1(int time);
int str2int(char *str);

#endif /* __USART1_H */
