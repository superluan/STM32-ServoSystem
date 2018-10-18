#ifndef __PID_H
#define	__PID_H

#include "stm32f10x.h"
#include "adc.h"
#include "pwm_output.h"
void PidFunction(int,int,u32);
void ParaSet(double Kp, double Ki, double Kd);
void PID_Control(int setPoint);
int calcAngle(int a);
int isOverFlow(int a);
int xianfu(int);
int theat(int,int,int,int);
int ssin(int,int,float,float);
//int millis();
#endif /* __PID_H */
