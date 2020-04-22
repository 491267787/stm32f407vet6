#include "dma.h"																	   	  
#include "delay.h"		 
//////////////////////////////////////////////////////////////////////////////////	 
//���ܣ�ADC+DMA��ͨ���ɼ���������	   
//���ߣ�zyf
//��������:2020/04/16
//�汾��V1.0								  
////////////////////////////////////////////////////////////////////////////////// 	 
 
//���ɼ�����adcֵ���������б���
 
#define   DMA_PER      3        //��������
#define   DMA_NUM      10        //�ɼ�����

static u16 DMA_Adc1Aver[DMA_PER];    //DMA_AdcAver[0]:pc0   DMA_AdcAver[1]:pc1  DMA_AdcAver[2]:pc2 
static u16 DMA_Adc1[DMA_NUM][DMA_PER]; 
 
u16 *DMA_GetAdcAver(void)
{
	return DMA_Adc1Aver; 
}


void DMA_GPIO_config()
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);                          //ʹ��GPIOCʱ��
	
	//�ȳ�ʼ��ADC1ͨ��IO��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 ;
	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;                                   //ģ������
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIOC, &GPIO_InitStructure);    
}

//adc��ʼ������
void DMA_ADC_config()
{
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_InitTypeDef       ADC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);                           //ʹ��ADC1ʱ��
 
 
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,ENABLE);	                           //ADC1��λ
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,DISABLE);                           //��λ����
 
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;                       //����ģʽ
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;  //���������׶�֮����ӳ�x��ʱ��
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_1;               //DMAʹ�ܣ�DMA������Ҫ����ʹ�ܣ�
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;                    //Ԥ��Ƶ4��Ƶ��ADCCLK=PCLK2/4=84/4=21Mhz,ADCʱ����ò�Ҫ����36Mhz 
  ADC_CommonInit(&ADC_CommonInitStructure);                                      //��ʼ�� 
	
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;                         //12λģʽ
  ADC_InitStructure.ADC_ScanConvMode =ENABLE;                                    //ɨ�裨����DMA����Ҫ����ɨ�裩
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;                             //��������ת��������DMA����Ҫ��������ת����
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;    //��ֹ������⣬ʹ���������
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;                         //�Ҷ���	
  ADC_InitStructure.ADC_NbrOfConversion = 3;                                    //�м���ͨ�������д�� ��DMA������Ҫ����Ϊͨ������
  ADC_Init(ADC1, &ADC_InitStructure);                                            //ADC��ʼ��
	
	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1,  ADC_SampleTime_480Cycles);  //res[0]-PC0
	ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 2,  ADC_SampleTime_480Cycles);  //res[1]-PC1
	ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 3,  ADC_SampleTime_480Cycles); //res[2]-PC2
 
	ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);
	ADC_DMACmd(ADC1, ENABLE);
	ADC_Cmd(ADC1, ENABLE);                                                         //����ADת����

  ADC_SoftwareStartConv(ADC1);//ʹADC1���������������	
}

//DMAx�ĸ�ͨ������
//����Ĵ�����ʽ�ǹ̶���,���Ҫ���ݲ�ͬ��������޸�
//�Ӵ洢��->����ģʽ/8λ���ݿ��/�洢������ģʽ
//DMA_Streamx:DMA������,DMA1_Stream0~7/DMA2_Stream0~7
//chx:DMAͨ��ѡ��,@ref DMA_channel DMA_Channel_0~DMA_Channel_7
//par:�����ַ
//mar:�洢����ַ
//ndtr:���ݴ�����  
void DMA_Config(DMA_Stream_TypeDef *DMA_Streamx,u32 chx,u32 par,u32 mar,u16 ndtr)
{ 
  NVIC_InitTypeDef NVIC_InitStructure;
	DMA_InitTypeDef  DMA_InitStructure;
	
	if((u32)DMA_Streamx>(u32)DMA2)//�õ���ǰstream������DMA2����DMA1
	{
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);//DMA2ʱ��ʹ�� 
		
	}else 
	{
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);//DMA1ʱ��ʹ�� 
	}
  DMA_DeInit(DMA_Streamx);
	
	while (DMA_GetCmdStatus(DMA_Streamx) != DISABLE){}//�ȴ�DMA������ 
	
  /* ���� DMA Stream */
  DMA_InitStructure.DMA_Channel = chx;  //ͨ��ѡ��
  DMA_InitStructure.DMA_PeripheralBaseAddr = par;//DMA�����ַ
  DMA_InitStructure.DMA_Memory0BaseAddr = mar;//DMA �洢��0��ַ
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;//����ģʽ���洢��
  DMA_InitStructure.DMA_BufferSize = ndtr;//���ݴ����� (��Ҫ��2�����ݣ�һ��10������˴���2*10)
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//���������ģʽ
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//�洢������ģʽ
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//�������ݳ���:16λ
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//�洢�����ݳ���:16λ
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;// ʹ��ѭ��ģʽ 
  DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;//�е����ȼ�
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//�洢��ͻ�����δ���
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//����ͻ�����δ���
  DMA_Init(DMA_Streamx, &DMA_InitStructure);//��ʼ��DMA Stream
	
  DMA_ClearFlag(DMA2_Stream0,DMA_IT_TC);
	DMA_ITConfig(DMA2_Stream0,DMA_IT_TC,ENABLE);
	
	  //�ж����ȼ�NVIC����
  NVIC_InitStructure.NVIC_IRQChannel=DMA2_Stream0_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01;                     //��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x01;                            //��Ӧ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	while (DMA_GetCmdStatus(DMA2_Stream0) != DISABLE){}	
  DMA_Cmd(DMA_Streamx, ENABLE);	
} 


//��������ú��������õ��Ⱥ�˳�򣬷���ᵼ�½��յ����ݴ�λ
void MYDMA_Config(void)
{ 
	DMA_GPIO_config();
	DMA_Config(DMA2_Stream0, DMA_Channel_0, (u32)&ADC1->DR,(u32)DMA_Adc1,DMA_PER*DMA_NUM);
	DMA_ADC_config();
}

void sys_ADC1_Config(void)
{
	MYDMA_Config();
}



static void DMA_filter(void)
{
	register u16 sum=0;
	u8 count=0,i=0,j=0;
	for(;i<DMA_PER;i++)
	{
		while(j<DMA_NUM)
		{
			if(DMA_Adc1[j][i]<0)
      {
			}
			else
			{
			  sum+=DMA_Adc1[j][i];
				count++;
			}
		  j++;
		}
		DMA_Adc1Aver[i]=sum/count;
		sum=0;
		count=0;
		j=0;
	}
}


//DMA���俪������
//��dma���������ʱ�������жϺ���
//DMA_Streamx:DMA������,DMA1_Stream0~7/DMA2_Stream0~7 
//ndtr:���ݴ�����  
void DMA2_Stream0_IRQHandler(void) 
{
	if (DMA_GetFlagStatus(DMA2_Stream0, DMA_IT_TCIF0) == SET)  
	{
		DMA_filter();
		DMA_ClearFlag(DMA2_Stream0, DMA_IT_TCIF0); 
	}

}



















