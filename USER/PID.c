#include "PID.h"
#include "math.h"
#include "stm32f10x.h"
#include "usart1.h"
#define PI 3.141592
#define dead_band1 5
#define dead_band2 5
extern __IO u16 ADC_ConvertedValue[4];
unsigned long lastTime = 0;//上次时间默认为0
double Output;
double etSum , lastErr =0;//上次误差为0
double kp ;
double ki ;
double kd ;
double dEt;
double et;
double t;
double A = 0;
double T;
double w;
int ang[5];
int angleLast;
int angle;
int degreeLast;
int degree;
int velocity;
int error_p;
int error_d;
int error_i;
int myspeed;
int l;
int p;
double timeChange;
unsigned long now;
extern double abs(double __x);
extern unsigned long Dime3;
float OutputValue;//积分饱和抑制
float MaxValue=20000.0;//输出最大限幅
float MinValue=-20000.0;//输出最小限幅
extern float float_time;

void PidFunction(int Aims,int Actual_value,u32 Dime)//输入的是目标、当前、时间，每次循环一次调一次占空比
{   
    /*How long since we last calculated*/  
			now = Dime;                   						//当前时间
			timeChange = (double)(now - lastTime);   //当前时间和上次时间差值，理论上每次都是1；
    /*Compute all the working error variables*/    
//		if(abs(Aims - Actual_value)<=50)//误差绝对值，死区设定
//			et = 0;
//		else
			et = Aims - Actual_value;                  				 //反馈值与输入值的差值
			etSum += et;                     		 //差值*时间间隔乘积累加 = 积分 理论是0
			//dEt = (et - lastErr)/timeChange;      			 		//差值-上一次差值 = 微分
			dEt = (et - lastErr);      			 		//差值-上一次差值 = 微分

		if(et<3 && et>-3)//允许误差AD值为6
		{
			kp = 0;
			ki = 0;
			kd = 0;
		}
		else//误差大于6就要调整
		{
			kp = 50;
			kd = 0.1;
/***********************积分饱和抑制********************************************/
    if(et>55 || et<-55) //防止积分饱和，大误差时不要积分项PID
    {
				ki = 0;
    }
		else
		{
				ki = 0.00001;
		}
		}
		/*if(et<20 && et>-20) //分段PID
    {
				kp = 30; 
				//ki = 0;
				kd = 0;
    }
		else
		{
				kp = 30;//1.7可单独使用
				//ki = 0;
				kd = 0.1;//800
		}	*/
		/******************模糊PID***********************/
//		if(et<55 && et>-55) //分段PID
//    {
//				kp = 0; 
//				ki = 0;
//				kd = 0;
//    }
//		else//先给小范围
//		{
//			kp = 5;
//			ki = 0;
//			kd = 0;
//				if(et>1000&&et<-1000)//发现是中范围
//				{
//					kp = 2;
//					ki = 0;
//					kd = 0;
//						if(et>2000&&et<-2000)//发现是大范围
//						{
//							kp = 1.7;
//							ki = 0;
//							kd = 0;
//						}	
//				}	
//		}	
    /**********************************************************************/
    /*Compute PID Output*/                        
    //myspeed = -(kp * et + ki * etSum + kd * dEt);//PID
			//myspeed = kp * et + ki * etSum;//PI
		  myspeed = kp*et + ki*etSum + kd * dEt + ki * etSum;	//PID
    /*Remember some variables for next time*/  
			lastErr = et;                                   //下次循环：当前比例成为过去比例
			//lastTime = now;                                 //下次循环：当前时间成为过去时间
//		if((fabs(et)<=dead_band1)&&(fabs(lastErr-et) <dead_band2))//两个死区，先不加
//		{ 
//      lastErr=0;
//      return;
//		}
		   //限幅
    if( myspeed > MaxValue)
        myspeed = MaxValue;
    if (myspeed < MinValue)
        myspeed = MinValue;
			setMotorSpeed(myspeed); 												//计算出的PID输出到电机函数++
		
//		if ( ( fabs(et)<=dead_band1)  &&  ( fabs(et-lastErr) <dead_band2)  )
//{ 
//      lastErr=0;
//      return;
//}

} 
//void ParaSet(double Kp, double Ki, double Kd)       //设置比例、微分、积分
//{   
//    kp = Kp;   
//    ki = Ki;   
//    kd = Kd;
//}

int xianfu(int str_temp_dec)//指令限幅防止跑飞
{
	if(str_temp_dec>3800)
		str_temp_dec = 3800;
	if(str_temp_dec<300)
		str_temp_dec = 300;
	if((str_temp_dec>=300)&&(str_temp_dec<=3800))
		str_temp_dec = str_temp_dec;
	return str_temp_dec;
}

int theat(int theat0,int theatf,int tf,int t)//角度轨迹规划，输入当前角度，最终角度，最终时间，当前时间。
{
	int theats;
	//theat0 = (theat0/4095)*360;//AD-->角度
	//theatf = (theatf/4095)*360;
	theats = theat0 + (3.0/(tf*tf))*(theatf-theat0)*(t*t) - (2.0/(tf*tf*tf))*(theatf-theat0)*(t*t*t);
//	if ((theats>theatf)||(theats<theatf))
//	{
//		theats = theatf;
//	}
//	else
//	{
//		theats =theats;
//	}
	return theats;//返回当前时间t下对应的角度输出。
}

int ssin(int theat0,int theatf,float tf,float t)
{
	int asin;
	int et = (theatf-theat0)/2;
	float float_et= tf - t;
	
	if(float_et<=0)
	{
		float_time =0.0;
	}
	else
	{
		float_time = float_time;
	}
	asin = et*sin(t)+(theatf-theat0)/2+theat0;
	return asin;
}
//int calcAngle(int a)
//{
//	return a;
//}

//int isOverFlow(int a)
//{
//	return a;
//}
//void PID_Control(int setPoint) //每次的定时器调整
//{ 
//			/**********角度PID**************/    
//				unsigned long now = millis(); 
//				double timeChange = (double)(now - lastTime);  
//        angleLast = angle;                     //保存上一次采样值
//				angle = ADC_ConvertedValue[1]; 				 //取均值
//        degreeLast = degree;                   //保存上一次的计算角度值
//        degree = calcAngle(angle);             //获取新的计算角度值
//        velocity = (angle - angleLast);        //得到采样角速度差值（微分值）
//        if(abs(angle-setPoint)<=4)             //输入与反馈值差的绝对值<4
//            error_p = 0;                       //p = 0,比例为0
//        else
//        error_p = setPoint-angle;              //p = 输入值与反馈值差 
//        error_d = velocity;                    //d =微分值
//        error_i = error_i + (error_p*timechange);
//         
//        myspeed = - (kp*error_p + ki*error_i + kd*error_d);
//         
//        setMotorSpeed(myspeed);                 //电机PID控制
//         
//        //printf(\"[%d %d %d %7d]\t\",angle,degree,myspeed,error_i);
//        error_i = isOverFlow(error_i);

//}
//		if(et>0)//目标点在当前点左边，须左转
//		{
//			T = 2*et;//1个周期，定为一个sin函数周期
//			t =et;//自变量t
//			w = 2*PI/T; //计算w
//			A = kp*(et+3000);//幅值，et*1，先大后小
//			for(l=0;l<t;l++)//从0->t,即从0->T/2。
//			{
//			myspeed = A*sin(w*l);
//			TurnLeft(myspeed);
//			//printf("myspeed%d",myspeed);
//			}
//		}
//		else//目标点在当前点右边，须右转
//		{
//			T = -2*et;//本次运动时间,1个周期
//			t =-et;
//			w = 2*PI/T; //计算w
//			A = kp*(et+3000);//幅值，et*1，先大后小
//			for(p=0;p<t;p++)//从0->t,即从0->T/2。
//			{
//			myspeed = A*sin(w*p);
//			TurnRight(myspeed);
//			//printf("myspeed%d",myspeed);
//			}
//		}
