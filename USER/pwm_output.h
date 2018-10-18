#ifndef __PWM_OUTPUT_H
#define	__PWM_OUTPUT_H

#include "stm32f10x.h"

void TIM1_PWM_Init(void);
void Servo_out (void);
void Servo_outer (void);
void setMotorSpeed(int);
void TurnLeft(int);
void TurnRight(int);
int 	constrain(int,int,int);
void test_moto(int,int);
void keep(void);
#endif /* __PWM_OUTPUT_H */

