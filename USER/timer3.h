#ifndef TIMER3_H
#define TIMER3_H

#include "stm32f10x.h"
#include "stm32f10x_tim.h" //此句删除会引起main函数里START_TIME报错

#define START_TIME3  time3=0;RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 , ENABLE);TIM_Cmd(TIM3, ENABLE)
#define STOP_TIME3  TIM_Cmd(TIM3, DISABLE);RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 , DISABLE)
void TIM3_NVIC_Configuration(void);
void TIM3_Configuration(void);
void TIM3_COUNTS_Init(void);


#endif	/* TIME_TEST_H */

