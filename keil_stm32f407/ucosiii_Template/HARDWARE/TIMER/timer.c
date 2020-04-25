#include "timer.h"
//////////////////////////////////////////////////////////////////////////////////	 
//���ܣ���ʱ���жϺ���	   
//���ߣ�zyf
//��������:2020/04/23
//�汾��V1.0								  
////////////////////////////////////////////////////////////////////////////////// 	 

///*��Ϊϵͳ��ʼ��SystemInit�������ʼ��APB1����ʱ��Ϊ4��Ƶ��42M������TIM2~TIM7��TIM12~TIM14��ʱ��ΪAPB1��ʱ�ӵ�������84M*/
//STM3 ��ͨ�� TIMx (TIM2~TIM5 �� TIM9~TIM14)��ʱ���жϳ�ʼ��
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//��ʱ�����ʱ����㷽��:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=��ʱ������Ƶ��,��λ:Mhz


void tim2_1s_init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;		 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);		        //ʹ��TIM2ʱ��
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;	
	TIM_TimeBaseInitStruct.TIM_CounterMode   = TIM_CounterMode_Up;	//���ϼ���ģʽ
	TIM_TimeBaseInitStruct.TIM_Period        = 9999;   	            //�Զ���װ��ֵ
	TIM_TimeBaseInitStruct.TIM_Prescaler     = 8399;                //ʱ��Ԥ��Ƶ��.STM32F4XX APB1��84MHZ,����72MHZ��STM32F1XX��72MHZ
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);		            //��ʼ��TIM2
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);		 	                //����ʱ��2�����ж�
	TIM_Cmd(TIM2,ENABLE);                                           //ʹ�ܶ�ʱ��2
}

void tim2_1s_NVIC_init(void)
{	
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel      = TIM2_IRQn;               //��ʱ��2�ж�
	NVIC_InitStruct.NVIC_IRQChannelCmd   = ENABLE;	
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority  =1;          //��ռ���ȼ�1
	NVIC_InitStruct.NVIC_IRQChannelSubPriority   = 4; 	            //�����ȼ�4
	NVIC_Init(&NVIC_InitStruct);
}

//��ʱ��2�жϷ�����
void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET) //����ж�
	{
//		LED1=!LED1;//DS1��ת
	}
	TIM_ClearITPendingBit(TIM2,TIM_IT_Update);  //����жϱ�־λ
}





void tim3_500ms_init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;		 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);		
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;	
	TIM_TimeBaseInitStruct.TIM_CounterMode   = TIM_CounterMode_Up;	
	TIM_TimeBaseInitStruct.TIM_Period        = 4999;   	
	TIM_TimeBaseInitStruct.TIM_Prescaler     = 8399;	
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct);		 
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);		 	
	TIM_Cmd(TIM3,ENABLE);
}

void tim3_500ms_NVIC_init(void)
{	
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel      = TIM3_IRQn;  
	NVIC_InitStruct.NVIC_IRQChannelCmd   = ENABLE;	
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority  =1; 
	NVIC_InitStruct.NVIC_IRQChannelSubPriority   = 3;   	
	NVIC_Init(&NVIC_InitStruct);
}

void TIM3_IRQHandler(void)
{ 	
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //����ж�
	{
//		LED1=!LED1;//DS1��ת
	}
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //����жϱ�־λ	
}




void tim4_250ms_init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;		 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);		
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;	
	TIM_TimeBaseInitStruct.TIM_CounterMode   = TIM_CounterMode_Up;	
	TIM_TimeBaseInitStruct.TIM_Period        = 2499;   	
	TIM_TimeBaseInitStruct.TIM_Prescaler     = 8399;	
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStruct);		 
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);		 	
	TIM_Cmd(TIM4,ENABLE);
}

void tim4_250ms_NVIC_init(void)
{	
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel      = TIM4_IRQn;  
	NVIC_InitStruct.NVIC_IRQChannelCmd   = ENABLE;	
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority  = 1;  
	NVIC_InitStruct.NVIC_IRQChannelSubPriority   = 2; 
	NVIC_Init(&NVIC_InitStruct);
}

void TIM4_IRQHandler(void)
{ 	
	if(TIM_GetITStatus(TIM4,TIM_IT_Update)==SET) //����ж�
	{
//		LED1=!LED1;//DS1��ת
	}
	TIM_ClearITPendingBit(TIM4,TIM_IT_Update);  //����жϱ�־λ		
}





void tim5_100ms_init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;		 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);		
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;	
	TIM_TimeBaseInitStruct.TIM_CounterMode   = TIM_CounterMode_Up;	
	TIM_TimeBaseInitStruct.TIM_Period        = 999;    	
  TIM_TimeBaseInitStruct.TIM_Prescaler     = 8399;	
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseInitStruct);		 
	TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);		 	
	TIM_Cmd(TIM5,ENABLE);
}

void tim5_100ms_NVIC_init()
{	
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel      = TIM5_IRQn;  
	NVIC_InitStruct.NVIC_IRQChannelCmd   = ENABLE;	
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority  = 0;	
	NVIC_InitStruct.NVIC_IRQChannelSubPriority   = 7; 	
	NVIC_Init(&NVIC_InitStruct);
}

void TIM5_IRQHandler(void)
{ 	
	if(TIM_GetITStatus(TIM5,TIM_IT_Update)==SET) //����ж�
	{
//		LED1=!LED1;//DS1��ת
	}
	TIM_ClearITPendingBit(TIM5,TIM_IT_Update);  //����жϱ�־λ	
}





void tim6_50ms_init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;		 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);		
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;	
	TIM_TimeBaseInitStruct.TIM_CounterMode   = TIM_CounterMode_Up;	
	TIM_TimeBaseInitStruct.TIM_Period        = 499,  	
	TIM_TimeBaseInitStruct.TIM_Prescaler     = 8399;	
	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseInitStruct);		 
	TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);		 	
	TIM_Cmd(TIM6,ENABLE);
}

void tim6_50ms_NVIC_init(void)
{	
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel      = TIM6_DAC_IRQn;  
	NVIC_InitStruct.NVIC_IRQChannelCmd   = ENABLE;	
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority  = 0;	
	NVIC_InitStruct.NVIC_IRQChannelSubPriority   = 6; 	
	NVIC_Init(&NVIC_InitStruct);
}


void TIM6_DAC_IRQHandler(void)
{ 	
	if(TIM_GetITStatus(TIM6,TIM_IT_Update)==SET) //����ж�
	{
//		LED1=!LED1;//DS1��ת
	}
	TIM_ClearITPendingBit(TIM6,TIM_IT_Update);  //����жϱ�־λ	
}




void tim7_10ms_init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;		 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);		
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;	
	TIM_TimeBaseInitStruct.TIM_CounterMode   = TIM_CounterMode_Up;	
 	TIM_TimeBaseInitStruct.TIM_Period        = 99;    
	TIM_TimeBaseInitStruct.TIM_Prescaler     = 8399;	
	TIM_TimeBaseInit(TIM7, &TIM_TimeBaseInitStruct);		 
	TIM_ITConfig(TIM7, TIM_IT_Update, ENABLE);		 	
	TIM_Cmd(TIM7,ENABLE);
}

void tim7_10ms_NVIC_init(void)
{	
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel      = TIM7_IRQn;  
	NVIC_InitStruct.NVIC_IRQChannelCmd   = ENABLE;	
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority  = 0;	
	NVIC_InitStruct.NVIC_IRQChannelSubPriority   = 5; 	
	NVIC_Init(&NVIC_InitStruct);
}

void TIM7_IRQHandler(void)
{ 	
	if(TIM_GetITStatus(TIM7,TIM_IT_Update)==SET) //����ж�
	{
//		LED1=!LED1;//DS1��ת
	}
	TIM_ClearITPendingBit(TIM7,TIM_IT_Update);  //����жϱ�־λ	
}



void TimerInit(void)
{
	tim2_1s_init();
	tim2_1s_NVIC_init();
	
	tim3_500ms_init();
	tim3_500ms_NVIC_init();
	
	tim4_250ms_init();
	tim4_250ms_NVIC_init();

	tim5_100ms_init();
	tim5_100ms_NVIC_init();

	tim6_50ms_init();
	tim6_50ms_NVIC_init();
	
	tim7_10ms_init();
	tim7_10ms_NVIC_init();
	
}

