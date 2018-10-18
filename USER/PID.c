#include "PID.h"
#include "math.h"
#include "stm32f10x.h"
#include "usart1.h"
#define PI 3.141592
#define dead_band1 5
#define dead_band2 5
extern __IO u16 ADC_ConvertedValue[4];
unsigned long lastTime = 0;//�ϴ�ʱ��Ĭ��Ϊ0
double Output;
double etSum , lastErr =0;//�ϴ����Ϊ0
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
float OutputValue;//���ֱ�������
float MaxValue=20000.0;//�������޷�
float MinValue=-20000.0;//�����С�޷�
extern float float_time;

void PidFunction(int Aims,int Actual_value,u32 Dime)//�������Ŀ�ꡢ��ǰ��ʱ�䣬ÿ��ѭ��һ�ε�һ��ռ�ձ�
{   
    /*How long since we last calculated*/  
			now = Dime;                   						//��ǰʱ��
			timeChange = (double)(now - lastTime);   //��ǰʱ����ϴ�ʱ���ֵ��������ÿ�ζ���1��
    /*Compute all the working error variables*/    
//		if(abs(Aims - Actual_value)<=50)//������ֵ�������趨
//			et = 0;
//		else
			et = Aims - Actual_value;                  				 //����ֵ������ֵ�Ĳ�ֵ
			etSum += et;                     		 //��ֵ*ʱ�����˻��ۼ� = ���� ������0
			//dEt = (et - lastErr)/timeChange;      			 		//��ֵ-��һ�β�ֵ = ΢��
			dEt = (et - lastErr);      			 		//��ֵ-��һ�β�ֵ = ΢��

		if(et<3 && et>-3)//�������ADֵΪ6
		{
			kp = 0;
			ki = 0;
			kd = 0;
		}
		else//������6��Ҫ����
		{
			kp = 50;
			kd = 0.1;
/***********************���ֱ�������********************************************/
    if(et>55 || et<-55) //��ֹ���ֱ��ͣ������ʱ��Ҫ������PID
    {
				ki = 0;
    }
		else
		{
				ki = 0.00001;
		}
		}
		/*if(et<20 && et>-20) //�ֶ�PID
    {
				kp = 30; 
				//ki = 0;
				kd = 0;
    }
		else
		{
				kp = 30;//1.7�ɵ���ʹ��
				//ki = 0;
				kd = 0.1;//800
		}	*/
		/******************ģ��PID***********************/
//		if(et<55 && et>-55) //�ֶ�PID
//    {
//				kp = 0; 
//				ki = 0;
//				kd = 0;
//    }
//		else//�ȸ�С��Χ
//		{
//			kp = 5;
//			ki = 0;
//			kd = 0;
//				if(et>1000&&et<-1000)//�������з�Χ
//				{
//					kp = 2;
//					ki = 0;
//					kd = 0;
//						if(et>2000&&et<-2000)//�����Ǵ�Χ
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
			lastErr = et;                                   //�´�ѭ������ǰ������Ϊ��ȥ����
			//lastTime = now;                                 //�´�ѭ������ǰʱ���Ϊ��ȥʱ��
//		if((fabs(et)<=dead_band1)&&(fabs(lastErr-et) <dead_band2))//�����������Ȳ���
//		{ 
//      lastErr=0;
//      return;
//		}
		   //�޷�
    if( myspeed > MaxValue)
        myspeed = MaxValue;
    if (myspeed < MinValue)
        myspeed = MinValue;
			setMotorSpeed(myspeed); 												//�������PID������������++
		
//		if ( ( fabs(et)<=dead_band1)  &&  ( fabs(et-lastErr) <dead_band2)  )
//{ 
//      lastErr=0;
//      return;
//}

} 
//void ParaSet(double Kp, double Ki, double Kd)       //���ñ�����΢�֡�����
//{   
//    kp = Kp;   
//    ki = Ki;   
//    kd = Kd;
//}

int xianfu(int str_temp_dec)//ָ���޷���ֹ�ܷ�
{
	if(str_temp_dec>3800)
		str_temp_dec = 3800;
	if(str_temp_dec<300)
		str_temp_dec = 300;
	if((str_temp_dec>=300)&&(str_temp_dec<=3800))
		str_temp_dec = str_temp_dec;
	return str_temp_dec;
}

int theat(int theat0,int theatf,int tf,int t)//�Ƕȹ켣�滮�����뵱ǰ�Ƕȣ����սǶȣ�����ʱ�䣬��ǰʱ�䡣
{
	int theats;
	//theat0 = (theat0/4095)*360;//AD-->�Ƕ�
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
	return theats;//���ص�ǰʱ��t�¶�Ӧ�ĽǶ������
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
//void PID_Control(int setPoint) //ÿ�εĶ�ʱ������
//{ 
//			/**********�Ƕ�PID**************/    
//				unsigned long now = millis(); 
//				double timeChange = (double)(now - lastTime);  
//        angleLast = angle;                     //������һ�β���ֵ
//				angle = ADC_ConvertedValue[1]; 				 //ȡ��ֵ
//        degreeLast = degree;                   //������һ�εļ���Ƕ�ֵ
//        degree = calcAngle(angle);             //��ȡ�µļ���Ƕ�ֵ
//        velocity = (angle - angleLast);        //�õ��������ٶȲ�ֵ��΢��ֵ��
//        if(abs(angle-setPoint)<=4)             //�����뷴��ֵ��ľ���ֵ<4
//            error_p = 0;                       //p = 0,����Ϊ0
//        else
//        error_p = setPoint-angle;              //p = ����ֵ�뷴��ֵ�� 
//        error_d = velocity;                    //d =΢��ֵ
//        error_i = error_i + (error_p*timechange);
//         
//        myspeed = - (kp*error_p + ki*error_i + kd*error_d);
//         
//        setMotorSpeed(myspeed);                 //���PID����
//         
//        //printf(\"[%d %d %d %7d]\t\",angle,degree,myspeed,error_i);
//        error_i = isOverFlow(error_i);

//}
//		if(et>0)//Ŀ����ڵ�ǰ����ߣ�����ת
//		{
//			T = 2*et;//1�����ڣ���Ϊһ��sin��������
//			t =et;//�Ա���t
//			w = 2*PI/T; //����w
//			A = kp*(et+3000);//��ֵ��et*1���ȴ��С
//			for(l=0;l<t;l++)//��0->t,����0->T/2��
//			{
//			myspeed = A*sin(w*l);
//			TurnLeft(myspeed);
//			//printf("myspeed%d",myspeed);
//			}
//		}
//		else//Ŀ����ڵ�ǰ���ұߣ�����ת
//		{
//			T = -2*et;//�����˶�ʱ��,1������
//			t =-et;
//			w = 2*PI/T; //����w
//			A = kp*(et+3000);//��ֵ��et*1���ȴ��С
//			for(p=0;p<t;p++)//��0->t,����0->T/2��
//			{
//			myspeed = A*sin(w*p);
//			TurnRight(myspeed);
//			//printf("myspeed%d",myspeed);
//			}
//		}
