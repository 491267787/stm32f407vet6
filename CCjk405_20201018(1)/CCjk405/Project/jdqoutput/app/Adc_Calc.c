/****************************************************************************************
Filename: Adc_Calc.c
Author: 
Datetime: 
Version: v0.01
****************************************************************************************/
#include "typedefs.h"
//#include "malloc.h"

#include "delay.h"
//#include "adc.h"
#include "main.h"
#include "uart.h"
#include "rtc.h"
#include "Adc_Calc.h"
#include <math.h>

#define JY_RES_DEFAULT  9999
#define JY_RES_VAILD    0x7fff

ST_JYCTRL   gJyCtrl;
ST_SAMPDATA gSampSysData;
static uint8 gJY_STEP[2]  = {0};
static uint32 sJyticks[2]  = {0};
static const float Iso_Vdcposk = 0.09649, Iso_Vdcposb = 0.03229,Iso_Vdcnegk = 0.09565, Iso_Vdcnegb = 0.10995;   //�̵����ڲ�
static const float Iso_Vdc1posk = 0.1923, Iso_Vdc1posb = -393.8462;//�̵������

#define  AdcCalc_PER  5        //��������
#define  AdcCalc_NUM  10       //�ɼ�����

static uint16 sAdcCalcValue[AdcCalc_PER] = {0};
static uint16 sAdcSamptmp[AdcCalc_NUM][AdcCalc_PER] = {0};

//��ȡ������������õ���Чֵ
ST_SAMPDATA *AdcCalc_GetValue(void)
{
	gSampSysData.temprature = sAdcCalcValue[0]; 
  gSampSysData.cc1 = sAdcCalcValue[1];
  gSampSysData.visopos = sAdcCalcValue[2]*Iso_Vdcposk*10 + 10*Iso_Vdcposb;;
  gSampSysData.visoneg = sAdcCalcValue[3]*Iso_Vdcnegk*10 + 10*Iso_Vdcnegb;   
  gSampSysData.vdciso[0] = gSampSysData.visopos + gSampSysData.visoneg;
  gSampSysData.vdciso[1] = sAdcCalcValue[4]*Iso_Vdc1posk*10 + 10*Iso_Vdc1posb;
	gSampSysData.visodiff = abs(gSampSysData.visopos - gSampSysData.visoneg);
	return &gSampSysData;
	
}

void AdcCalc_GetGunTempr(uint8 channel)
{
	gSampSysData.guntemprature[channel] = AdcCalc_GetValue()->temprature;
}


//�泵���ʱ����ĸ��ѹ�����20V ʱ��Ϊ�о�Ե����
uint8 AdcCalc_GetISOStatus(void)
{
	if(200 < AdcCalc_GetValue()->visodiff)
	{
		return 1;
	}
	return 0;
	
}


/*****************************************************************??
�� ���Եص���R1 �����Եص�ѹU1�����Եص���R2�����Եص�ѹU2  R1=9000K//Rx   R2 = 9000K
���Եؼ̵���Ͷ��  �����Եص�ѹU1' ,���Եص�ѹU2',R1' = (4500K)//Rx  R2= 9000K

R1/R2 = u1 / u2
R1'/R2 = u1' /U2    -------> Rx = 9000 * (u1*u2'-u2*u1') / (2*u2*u1' -u1*u2')
R1 = 9000K//Rx
R1'= 4500K// Rx

                   -------->Ry = 9000 *(u2*u1'-u1*u2')/(2u1*u2'-u2*u1')
���˲����迹 = 18031.597K
******************************************************************/


/******************************����ֵ����1
���Եؼ̵���Ͷ��  �����Եص�ѹU1' ,���Եص�ѹU2',      R1' = (4500K)//Rx      R2'= 10500K//Ry
���Եؼ̵���Ͷ��  �����Եص�ѹU1'' ,���Եص�ѹU2'',    R1'' = (10500K)//Rx    R2''= (4500K)//Ry
R1'/R2' = u1' / u2'
R1''/R2' = u1'' /U2''    -------> Rx = [ 4500*10500(u1'*u2'' - u1''*u2') ] / [4500*u1''*u2' - 6000*u2'*u2'' - 10500*u1'*u2'']


*******************************/

#if 0
/*****************************************************************
�����Եص���R1,���Եص�ѹU1,���Եص���R2,���Եص�ѹU2.  R1=10500K//Rx   R2 = 10500K//Ry
��ƽ�������Եؼ̵���Ͷ��,���Եص�ѹU1',���Եص�ѹU2'.     R1'= (4500K)//Rx  R2= R2 = 10500K//Ry

R1/R2 = u1 / u2
R1'/R2 = u1' /U2    -------> k = (u1'*u2*10500) / (u1*u2'*4500)
					-------> Rx = (4500*k - 10500) / (1-k)
                    -------->Ry = 
					
					
******************************************************************/
#endif

void RY1_ON(void)
{
	GPIO_ResetBits(GPIOC,RELAY2);
	for(int i = 0 ; i < 1000; i++ );
	GPIO_SetBits(GPIOC,RELAY1);
  for(int i = 0 ; i < 1000; i++ );
}

void RY2_ON(void)
{
	GPIO_ResetBits(GPIOC,RELAY1);
	for(int i = 0 ; i < 1000; i++ );
	GPIO_SetBits(GPIOC,RELAY2);
  for(int i = 0 ; i < 1000; i++ );
}
void RY_OFF(void)
{
	GPIO_ResetBits(GPIOC,RELAY2);
	for(int i = 0 ; i < 1000; i++ );
	GPIO_ResetBits(GPIOC,RELAY1);
  for(int i = 0 ; i < 1000; i++ );
}

#define CHK_MS 200  /*1s*/
#if 0
/*��Ե����ֵ��������*/
uint8  Datadeal_CalcDcJy(uint8 seq)
{
    static uint16 stepcnt[2]= {0};
    static fp32 sURxpos[2] = {0}; //u1
    static fp32 sURxneg[2]  = {0}; //u2
    static fp32 sURypos[2]  = {0}; //u1'
    static fp32 sURyneg[2]  = {0}; //u2'
    static uint8 onoffflag[2]  = {0};
    uint8 rtn = RTN_JYCONTINUE;
    uint16 temp,temp16;
    fp32 ftemp,ftemp1,ftemp2,fRsvVal;

//gSampSysData.vdc[seq] = 750;   //����ʱ����75V�Ĺ̶���ѹ������

    switch(gJY_STEP[seq])
    {
      case 0:
        if((seq == 0) && (0 == onoffflag[0])) {
			RY_OFF();
            onoffflag[0] = 1;
		}
        if((seq == 0) && (JY_START == gJyCtrl.sbit.b1Allow )) {
            gJY_STEP[0] = 1;
            onoffflag[0] = 0;
//            gJyCtrl.sbit.bchkTm = 10; 
        }else {
			break;
        } /*step==1 ��ֱ������step1*/
      case 1:
        if(seq == 0){
          RY1_ON();
//          gJyCtrl.sbit.bchkTm = 8; 
		}
        sJyticks[seq] = GetSystemTick();
        gJY_STEP[seq] = 2;
        stepcnt[seq] = 0;
        sURxpos[seq] = 0;
        sURxneg[seq] = 0;
        break;
      case 2:
        if((GetSystemTick() - sJyticks[seq] > (CHK_MS/2))) 
        {
            if( gSampSysData.vdc[seq] > gSampSysData.visoneg[seq] ){
			    stepcnt[seq]++;
			    sURxpos[seq] += (gSampSysData.vdc[seq] - gSampSysData.visoneg[seq]);//u1'
			    sURxneg[seq] += gSampSysData.visoneg[seq];//u2'
            }
        }else {
           break;
        }
        if((GetSystemTick() - sJyticks[seq] > CHK_MS)){
			sURxpos[seq] = sURxpos[seq] /(stepcnt[seq]); //u1'
			sURxneg[seq] = sURxneg[seq] /(stepcnt[seq]);//u2'
			stepcnt[seq] = 0;
            if(seq == 0){
				RY2_ON();
                gJyCtrl.sbit.bchkTm = 6; 
			}
			sJyticks[seq] = GetSystemTick();
			gJY_STEP[seq] = 3;
          
            sURypos[seq] = 0;
            sURyneg[seq] = 0;
            //stepcnt = 0;
        }
        break;
      case 3:
        if(GetSystemTick() - sJyticks[seq] > (CHK_MS/2)) 
        {
            if( gSampSysData.vdc[seq] > gSampSysData.visoneg[seq] )
            {
				stepcnt[seq]++;
				sURypos[seq] += (gSampSysData.vdc[seq] - gSampSysData.visoneg[seq]);//u1''
				sURyneg[seq] += gSampSysData.visoneg[seq];//u2''
            }
        }else{
            break;
        }

		if( (GetSystemTick() - sJyticks[seq] > CHK_MS) ) {
			sURypos[seq] = sURypos[seq] /(stepcnt[seq]); //u1''
			sURyneg[seq] = sURyneg[seq] /(stepcnt[seq]); //u2''
			stepcnt[seq] = 0;
			if(seq == 0){
				RY_OFF();
				gJyCtrl.sbit.bchkTm = 3; 
			}

            /****************************************************************************************/

            if(gSampSysData.vdc[seq] > 250) {
                //Ry ���Ե�
			    ftemp1 = fabs(10500*3 *sURxneg[seq]/sURxpos[seq] - 10500*3*sURyneg[seq]/sURypos[seq]);
                ftemp2 = 4 - 3*sURxneg[seq]/sURxpos[seq] + 7 * sURyneg[seq]/sURypos[seq];
                ftemp = ftemp1 / ftemp2;
                fRsvVal = ftemp;

                ftemp1 = ftemp * 18032 / (18032 - ftemp);
                ftemp = ftemp1;

                if( ftemp < 102 ) {
				    ftemp =ftemp ;
                }else if( ftemp < 500 ) {
				    ftemp =ftemp *1.12 - 10;
                }else {
                    ftemp =ftemp *1.28 - 70.5;
			    }

                if( ftemp > gSampSysData.ResSeting) {
				    temp = JY_RES_DEFAULT;
			    }else if(ftemp <= 0){
                    temp = JY_RES_VAILD;
                }else{
				    temp = (uint16)(ftemp*10); 
                    temp16 = (temp % 10) > 5 ? 1: 0;
                    temp = temp / 10;
                    temp += temp16;
			    }
                gSampSysData.Resneg[seq] = temp; 

                //Rx ���Ե�
                ftemp1 = 300 * 105 * fRsvVal;
                ftemp2 = 3*10500*sURxneg[seq]/sURxpos[seq] -7 * fRsvVal+ 3*sURxneg[seq]*fRsvVal / sURxpos[seq];
                ftemp = ftemp1 / ftemp2;
                if( ftemp < 210 ) {
                    ftemp = ftemp ;
                }else if ( ftemp < 500 ){
                    ftemp = ftemp * 1.113 - 14.9;
                }else{
                    ftemp = ftemp * 1.18 - 51.2;
                }
         
                if( ftemp > gSampSysData.ResSeting) {
					temp = JY_RES_DEFAULT;
				}else if(ftemp <= 0){
                    temp = JY_RES_VAILD;
                }else{
					temp = (uint16)(ftemp*10); 
                    temp16 = (temp % 10) > 5 ? 1: 0;
                    temp = temp / 10;
                    temp += temp16;
				}
                gSampSysData.Respos[seq] = temp; 
            }else {
			    gSampSysData.Respos[seq] = JY_RES_DEFAULT;
            }
    
            /****************************************************************************************/
            /*��λ����*/
			sURxpos[seq] = 0;
            sURxneg[seq] = 0;
            sURypos[seq] = 0;
            sURyneg[seq] = 0;
            sJyticks[seq] = GetSystemTick();
            gJY_STEP[seq] = 0;
            if(seq == 0){
                gJyCtrl.sbit.b1Allow = JY_STOP;
                gJyCtrl.sbit.bchkTm = 0; 
            }else if( seq == 1) {
                gJyCtrl.sbit.b2Allow = JY_STOP;
                gJyCtrl.sbit.bchkTm = 0; 
            }else {
                ;
            }
            rtn = RTN_JYEND;
        }
        break; 
      case 4: //����
        break;
      default:
        break;
    }

    return rtn;
}

#endif











void ADC_Debug(void)
{
    static uint32 sU32tick = 0;
    uint16 temp[2];	
    if( (GetSystemTick() - sU32tick > 5*CHK_MS) ) 
    {

			AdcCalc_GetValue();
	    sU32tick = GetSystemTick();
		  printf("tempr0 = %d,tempr1 = %d,cc1 = %d---",gSampSysData.guntemprature[0],gSampSysData.guntemprature[1],gSampSysData.cc1);
		  printf("realx+ = %d---",gSampSysData.visopos);       
      printf("realy- = %d---",gSampSysData.visoneg);        
      printf("realZ+ = %d ,realZ- = %d---",gSampSysData.vdciso[0],gSampSysData.vdciso[1]);
		  printf("diff = %d---",gSampSysData.visodiff);
	}
}





void init_JYdata(void)
{
    memset(&gJyCtrl,0,sizeof(gJyCtrl));
    memset(&gSampSysData,0,sizeof(gSampSysData));
    gSampSysData.Respos = JY_RES_DEFAULT;
    gSampSysData.Resneg= JY_RES_DEFAULT;
    gSampSysData.ResSeting = 50;
}





void DMA_filter(void)
{	
    register uint16 sum=0;	
	u8 count=0,i=0,j=0;	
	for(;i<AdcCalc_PER;i++)	
	{		
	    while(j<AdcCalc_NUM)		
		{		
		    if(sAdcSamptmp[j][i]<0)
			{
			}				
			else			
			{			
			    sum+=sAdcSamptmp[j][i];					
				count++;			
			}		  
			j++;		
		}		
		sAdcCalcValue[i]=sum/count;		
		sum=0;
		count=0;
		j=0;	
	}


}


//����һ��DMA����
//DMA_Streamx:DMA������,DMA1_Stream0~7/DMA2_Stream0~7 
//ndtr:���ݴ�����  
void DMA2_Stream0_IRQHandler(void) 
{	
  if(DMA_GetFlagStatus(DMA2_Stream0, DMA_IT_TCIF0) == SET)  	
	{		
	    DMA_filter();		
		DMA_ClearFlag(DMA2_Stream0, DMA_IT_TCIF0); 	
	}
}


void DMA_GPIO_config()
{	
    GPIO_InitTypeDef  GPIO_InitStructure;		
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC, ENABLE);    //ʹ��GPIOB /GPIOCʱ��		

	//�ȳ�ʼ��ADC1ͨ��IO�� 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;	  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;                                   //ģ������  GPIO_Init(GPIOA, &GPIO_InitStructure);                                       
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//����������
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2;	  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;                                   //ģ������  GPIO_Init(GPIOA, &GPIO_InitStructure);                                       
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//����������
	GPIO_Init(GPIOC, &GPIO_InitStructure);    
	
}


/*
PB0:ADC1 Channel8 pin as analog input                          gun temp
PB1:ADC1 Channel9 pin as analog input                          gun cc1
PC0:ADC1 Channel10 pin as analog input                        iso vdc+(ĸ�����Եص�ѹ)
PC1:ADC1 Channel11 pin as analog input                        iso vdc-(ĸ�߸��Եص�ѹ)
PC2:ADC1 Channel12 pin as analog input                        iso vdc1(�Ӵ�������ѹ)
*/

void DMA_ADC_config()
{
	ADC_CommonInitTypeDef ADC_CommonInitStructure;	
	ADC_InitTypeDef       ADC_InitStructure;		
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);                  //ʹ��ADC1ʱ��  	
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,ENABLE);	               //ADC1��λ	
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,DISABLE);                  //��λ���� 	
	
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;                       //����ģʽ  
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;   //���������׶�֮����ӳ�x��ʱ��  
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_1;               //DMAʹ�ܣ�DMA������Ҫ����ʹ�ܣ�  
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;                    //Ԥ��Ƶ4��Ƶ��ADCCLK=PCLK2/4=84/4=21Mhz,ADCʱ����ò�Ҫ����36Mhz   
	ADC_CommonInit(&ADC_CommonInitStructure);                                //��ʼ��
	
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;                         //12λģʽ  
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;                                    //ɨ�裨����DMA����Ҫ����ɨ�裩  
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;                             //��������ת��������DMA����Ҫ��������ת����  
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;    //��ֹ������⣬ʹ��������� 
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;                         //�Ҷ���	  
	ADC_InitStructure.ADC_NbrOfConversion = 5;                                     //�м���ͨ�������д�� ��DMA������Ҫ����Ϊͨ������ 
	ADC_Init(ADC1, &ADC_InitStructure);                                          //ADC��ʼ��
	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1,  ADC_SampleTime_144Cycles);   //res[0]	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_9, 2,  ADC_SampleTime_144Cycles);   //res[1]	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 3,  ADC_SampleTime_144Cycles);   //res[2]
	ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 4,  ADC_SampleTime_144Cycles);   //res[3]	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 5,  ADC_SampleTime_144Cycles);   //res[4]
	
	ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);
	ADC_DMACmd(ADC1, ENABLE);	
	ADC_Cmd(ADC1, ENABLE);                                                    //����ADת����

	ADC_SoftwareStartConv(ADC1);                      //����DMA
}


/*
DMAx�ĸ�ͨ����������Ĵ�����ʽ�ǹ̶���,���Ҫ���ݲ�ͬ��������޸ĴӴ洢��->����ģʽ/8λ���ݿ��/�洢������ģʽ
DMA_Streamx:DMA������,DMA1_Stream0~7/DMA2_Stream0~7
chx:DMAͨ��ѡ��,@ref DMA_channel DMA_Channel_0~DMA_Channel_7
par:�����ַ
mar:�洢����ַ
ndtr:���ݴ����� 
*/
void DMA_config(DMA_Stream_TypeDef *DMA_Streamx, u32 chx, u32 par, u32 mar, u16 ndtr)
{	
    DMA_InitTypeDef  DMA_InitStructure;	
	NVIC_InitTypeDef NVIC_InitStructure;		
	if((u32)DMA_Streamx>(u32)DMA2)                                                 //�õ���ǰstream������DMA2����DMA1	
	{	  
	    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);                        //DMA2ʱ��ʹ�� 			
	}
	else
	{	  
	    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);                        //DMA1ʱ��ʹ�� 	
	}  
	DMA_DeInit(DMA_Streamx);		
	while (DMA_GetCmdStatus(DMA_Streamx) != DISABLE){}                             //�ȴ�DMA������ 	 

	/* ���� DMA Stream */  
	DMA_InitStructure.DMA_Channel = chx;                                           //ͨ��ѡ��  
	DMA_InitStructure.DMA_PeripheralBaseAddr = par;                                //DMA�����ַ  
	DMA_InitStructure.DMA_Memory0BaseAddr = mar;                                   //DMA �洢��0��ַ  
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;                        //�洢��������ģʽ  
	DMA_InitStructure.DMA_BufferSize = ndtr;                                       //���ݴ�����  
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;               //���������ģʽ  
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                        //�洢������ģʽ  
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;    //�������ݳ���:16λ  
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;            //�洢�����ݳ���:16λ  
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;                                //ʹ����ͨģʽ   
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;                          //�е����ȼ�  
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;           
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;  
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;                    //�洢��ͻ�����δ���  
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;            //����ͻ�����δ���  
	DMA_Init(DMA_Streamx, &DMA_InitStructure);                                     //��ʼ��

    DMA_ClearFlag(DMA2_Stream0,DMA_IT_TC);
    DMA_ITConfig(DMA2_Stream0,DMA_IT_TC,ENABLE);		
	
	NVIC_InitStructure.NVIC_IRQChannel=DMA2_Stream0_IRQn; 	
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01;                     //��ռ���ȼ�	
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x01;                            //��Ӧ���ȼ�	
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;	
	NVIC_Init(&NVIC_InitStructure);	
	
	while (DMA_GetCmdStatus(DMA2_Stream0) != DISABLE){}	
	DMA_Cmd(DMA2_Stream0, ENABLE);

    
}



void MYDMA_Config(DMA_Stream_TypeDef *DMA_Streamx, u32 chx, u32 par, u32 mar, u16 ndtr)
{
    DMA_GPIO_config();	
	  DMA_config(DMA_Streamx, chx, par, mar, ndtr);
    DMA_ADC_config();
} 

void sys_ADC1_Init(void)            //ADC1��ʼ��
{
	MYDMA_Config(DMA2_Stream0, DMA_Channel_0, (u32)&ADC1->DR, (u32)sAdcSamptmp, AdcCalc_NUM*AdcCalc_PER);
}


//void Task_JYDataDeal(void *p_arg)
//{
//	init_JYdata();
//    MYDMA_Config(DMA2_Stream0, DMA_Channel_0, (u32)&ADC1->DR, (u32)sIsoVoltmp, Iso_NUM*Iso_PER);
//	
//	
//	while(1)
//	{
//        ADC_Debug();

//#if 0	 
//	/*��Ե����ֵ1��������*/
//	Datadeal_CalcDcJy(0);
//#endif

//    	Delay5Ms(1);
//	}

//}