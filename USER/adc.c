/***************STM32F103C8T6**********************
 * 文件名  ：adc.c  12位ADC
 * 描述    : DMA方式读取adc1
 * 实验平台：STM32F103C8T6
 * 备注    ：修改单一通道至多通道，要修改本文件的gpio接口和adc的通道数，first to ninth
 * 接口    ：ADC1：PA0，PA1,PA2
********************LIGEN*************************/

#include "adc.h"
#include "stm32f10x_tim.h"
#include "pwm_output.h"
#define ADC1_DR_Address    ((u32)0x4001244C)
__IO u16 ADC_ConvertedValue[4];                                                      //first

/*配置采样通道端口 使能GPIO时钟	  设置ADC采样PA0端口信号*/
 void ADC1_GPIO_Config(void)
{ 
	GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2;                    //second
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		    //GPIO设置为模拟输入
  GPIO_Init(GPIOA, &GPIO_InitStructure);   
}

/*配置ADC1的工作模式为DMA模式  */
 void ADC1_Mode_Config(void)
{
	ADC_InitTypeDef ADC_InitStructure;//引用结构体
  DMA_InitTypeDef DMA_InitStructure;//引用结构体
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE); //使能DMA1时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);	//使能ADC1时钟
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //分频因子6时钟为72M/6=12MHz，不可超过12MHZ，决定采样周期。
	
	//DMA设置
  DMA_DeInit(DMA1_Channel1);  //指定DMA通道1
  DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;//设置DMA外设地址为ADC用
  DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&ADC_ConvertedValue;	//设置DMA内存地址，ADC转换结果直接放入该地址
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; //外设为设置为数据传输的来源
  DMA_InitStructure.DMA_BufferSize = 4;	//DMA缓冲区设置为4；                     
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//设置DMA的外设地址不增加
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; //设置DMA内存递增模式    内存地址自增1 
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//外设数据字长
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//内存数据字长
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;//DMA的传输模式：循环模式
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;//设置DMA的优先级别
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;//设置DMA的两个memory中的变量互相访问
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);

	//ADC配置
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; //使用独立模式，扫描模式
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;//AD连续扫描                          //fifth
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; //ADC连续转换
//  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; //软件触发
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T3_TRGO;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//使用数据右对齐
  ADC_InitStructure.ADC_NbrOfChannel = 4;  // 设置规则序列的长度，顺序进行转换ADC的通道数目                     //sixth
  ADC_Init(ADC1, &ADC_InitStructure);//返回库函数
	ADC_TempSensorVrefintCmd(ENABLE); //开启内部温度传感器
	
  ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5); //通道1采样周期55.5个时钟周期    //seventh
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_55Cycles5); //通道1采样周期55.5个时钟周期    //eighth
	ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 3, ADC_SampleTime_55Cycles5); //通道1采样周期55.5个时钟周期    //ninth
	ADC_RegularChannelConfig(ADC1, ADC_Channel_16,4, ADC_SampleTime_239Cycles5);//这一通道采集芯片内部温度//239.5的ADC时钟采样周期
	
  ADC_DMACmd(ADC1, ENABLE);	 //使能ADC的DMA
  ADC_Cmd(ADC1, ENABLE); //使能ADC1

  ADC_ResetCalibration(ADC1);//ADC1校准寄存器复位
  while(ADC_GetResetCalibrationStatus(ADC1));//等待校准寄存器复位完成
  ADC_StartCalibration(ADC1);
  while(ADC_GetCalibrationStatus(ADC1));//等待ADC1校准完成
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);  //开始转换
	DMA_Cmd(DMA1_Channel1, ENABLE);  //使能DMA通道
}

/*初始化ADC1 */
void ADC1_Init(void)
{
	ADC_DeInit(ADC1);
	ADC1_GPIO_Config();//ADC端口初始化
	ADC1_Mode_Config();//ADC模式设置，包含ADC和DMA
}

