/*************************滤波法集合******************************************
**********************l-Dragon 2018.03.09************************************/
#include "Wave_filtering.h"
#include "adc.h"
#include "usart1.h"

#define N 12  
#define M 10
u16 i,j,k;
u16 temp;
u16 ArrDataBuffer[N];
u16 sum;
u16 Value;
extern __IO u16 ADC_ConvertedValue[4];

unsigned int value_buf[N];			//递推平均滤波法
unsigned int filter()
{
	unsigned int i;
  unsigned int value;
  int sum=0;
  value_buf[i++] = ADC_ConvertedValue[0];
  for(i=0;i<M;i++)
  {
    value_buf[i]=value_buf[i+1];
    sum += value_buf[i];
  }
  value = sum/M;
  return(value);
}

u16 Middle()  //中位值平均滤波法
{  
	for(i=0;i<N;i++)//一次采集N个数据，存入数组
		{  
			ArrDataBuffer[i]= ADC_ConvertedValue[0];  
			//Delayus1(50);  
		}  
	for(j=0;j<N-1;j++)//采样值由小到大排列
		{  
			for(k=0;k<N-j-1;k++)  
			{  
				if(ArrDataBuffer[k]>ArrDataBuffer[k+1])  
					{  
						temp=ArrDataBuffer[k];
						ArrDataBuffer[k]=ArrDataBuffer[k+1];
						ArrDataBuffer[k+1]=temp;
					} 
			} 
		}	  
	sum =ArrDataBuffer[1]+ArrDataBuffer[2]+ArrDataBuffer[3]+ArrDataBuffer[4]+ArrDataBuffer[5]+ArrDataBuffer[6]+ArrDataBuffer[7]+ArrDataBuffer[8]+ArrDataBuffer[9]+ArrDataBuffer[10];
	Value=sum/(N-2); 
	return(Value); 
}  

u16 ArithmeticalAverageValueFilter()  //算数平均滤波
{  
	u16 i;  
	u16 Value;  
	u16 sum;  
	sum=0;  
	for(i=0;i<M;i++)  
	{  
		sum = ADC_ConvertedValue[0]+sum;  
		Delayms1(1);    
	}  
		Value=sum/M;  
		return(Value);  
}  

u16 Gilde(uchar Data[])  //递推平均滤波
	{  
		uchar i,Value,sum;  
		sum=0;  
		Data[N]=ADC_ConvertedValue[0];  
		for(i=0;i<N;i++)  
		{  
			Data[i]=Data[i+1];//数据左移，扔掉低位。  
			sum+=Data[i];  
		}  
		Value=sum/N;  
		return(Value);  
} 
	


	
