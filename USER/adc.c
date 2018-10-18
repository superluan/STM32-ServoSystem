/***************STM32F103C8T6**********************
 * �ļ���  ��adc.c  12λADC
 * ����    : DMA��ʽ��ȡadc1
 * ʵ��ƽ̨��STM32F103C8T6
 * ��ע    ���޸ĵ�һͨ������ͨ����Ҫ�޸ı��ļ���gpio�ӿں�adc��ͨ������first to ninth
 * �ӿ�    ��ADC1��PA0��PA1,PA2
********************LIGEN*************************/

#include "adc.h"
#include "stm32f10x_tim.h"
#include "pwm_output.h"
#define ADC1_DR_Address    ((u32)0x4001244C)
__IO u16 ADC_ConvertedValue[4];                                                      //first

/*���ò���ͨ���˿� ʹ��GPIOʱ��	  ����ADC����PA0�˿��ź�*/
 void ADC1_GPIO_Config(void)
{ 
	GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2;                    //second
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		    //GPIO����Ϊģ������
  GPIO_Init(GPIOA, &GPIO_InitStructure);   
}

/*����ADC1�Ĺ���ģʽΪDMAģʽ  */
 void ADC1_Mode_Config(void)
{
	ADC_InitTypeDef ADC_InitStructure;//���ýṹ��
  DMA_InitTypeDef DMA_InitStructure;//���ýṹ��
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE); //ʹ��DMA1ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);	//ʹ��ADC1ʱ��
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //��Ƶ����6ʱ��Ϊ72M/6=12MHz�����ɳ���12MHZ�������������ڡ�
	
	//DMA����
  DMA_DeInit(DMA1_Channel1);  //ָ��DMAͨ��1
  DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;//����DMA�����ַΪADC��
  DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&ADC_ConvertedValue;	//����DMA�ڴ��ַ��ADCת�����ֱ�ӷ���õ�ַ
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; //����Ϊ����Ϊ���ݴ������Դ
  DMA_InitStructure.DMA_BufferSize = 4;	//DMA����������Ϊ4��                     
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//����DMA�������ַ������
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; //����DMA�ڴ����ģʽ    �ڴ��ַ����1 
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//���������ֳ�
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//�ڴ������ֳ�
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;//DMA�Ĵ���ģʽ��ѭ��ģʽ
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;//����DMA�����ȼ���
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;//����DMA������memory�еı����������
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);

	//ADC����
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; //ʹ�ö���ģʽ��ɨ��ģʽ
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;//AD����ɨ��                          //fifth
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; //ADC����ת��
//  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; //�������
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T3_TRGO;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//ʹ�������Ҷ���
  ADC_InitStructure.ADC_NbrOfChannel = 4;  // ���ù������еĳ��ȣ�˳�����ת��ADC��ͨ����Ŀ                     //sixth
  ADC_Init(ADC1, &ADC_InitStructure);//���ؿ⺯��
	ADC_TempSensorVrefintCmd(ENABLE); //�����ڲ��¶ȴ�����
	
  ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5); //ͨ��1��������55.5��ʱ������    //seventh
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_55Cycles5); //ͨ��1��������55.5��ʱ������    //eighth
	ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 3, ADC_SampleTime_55Cycles5); //ͨ��1��������55.5��ʱ������    //ninth
	ADC_RegularChannelConfig(ADC1, ADC_Channel_16,4, ADC_SampleTime_239Cycles5);//��һͨ���ɼ�оƬ�ڲ��¶�//239.5��ADCʱ�Ӳ�������
	
  ADC_DMACmd(ADC1, ENABLE);	 //ʹ��ADC��DMA
  ADC_Cmd(ADC1, ENABLE); //ʹ��ADC1

  ADC_ResetCalibration(ADC1);//ADC1У׼�Ĵ�����λ
  while(ADC_GetResetCalibrationStatus(ADC1));//�ȴ�У׼�Ĵ�����λ���
  ADC_StartCalibration(ADC1);
  while(ADC_GetCalibrationStatus(ADC1));//�ȴ�ADC1У׼���
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);  //��ʼת��
	DMA_Cmd(DMA1_Channel1, ENABLE);  //ʹ��DMAͨ��
}

/*��ʼ��ADC1 */
void ADC1_Init(void)
{
	ADC_DeInit(ADC1);
	ADC1_GPIO_Config();//ADC�˿ڳ�ʼ��
	ADC1_Mode_Config();//ADCģʽ���ã�����ADC��DMA
}

