/**********************************************************************************************************
*  �ļ����� main.c
*	 ��Ȩ��   
*  ������   Ӧ�ó���������
***********************************************************************************************************/
#include <includes.h>
#include "uart.h"
#include "rtc.h"
#include "ChgData.h"
#include "main.h"
#include "Screen.h"
#include "gpio.h"
#include "adc.h"
#include "MCP2515.h"
#include "fm24lc64.h"
#include "hwdevopt.h"
#include "wdg.h"
#include "ff_Lock.h"       /* FATFS */
#include "SST_FLASH.h"

#include "Adebug.h"  //patli 20191015
#include "timer.h"
#include "flash_if_for_upgrade.h"
#include "typedefs.h"

#include "stm32f4xx_dma.h"
#include "Adc_Calc.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Extern functions ------------------------------------------------------- */
extern void Wdg_feeddog(void);
extern void SystemTickInit(void);
/*��ʼ��ϵͳ��������¼*/
extern void Rd_InitRecord(void);
extern void MainCtrl_Init(void);
extern void fm24lc_init(void);
extern void sst_init(void);

/* Extern functions ------------------------------------------------------- */ 
extern void TaskDcMeter(void *p_arg);
extern void Task_IsoCheck(void *p_arg);

extern void TaskCardReader(void *p_arg);
extern void Task_CdModule(void *p_arg);
extern void Task_Screen_Main(void *p_arg);
extern void TaskAGunBmsComm(void *p_arg);
extern void TaskBGunBmsComm(void *p_arg);

extern void TaskAGunMainCtrl(void *p_arg);
//extern void TaskBGunMainCtrl(void *p_arg);

extern void Task_RealCheck(void *p_arg);
extern void Task_BackComm(void *p_arg);
#ifdef USB_HOST_SUPPORT
extern void TaskUSB(void *p_arg);
#endif

extern void Task_Screen_init();

static  OS_STK	AppTaskStartStk[APP_START_STK_SIZE];

#ifdef CARD_ON
static OS_STK  Stk_TaskCard[APP_CARD_STK_SIZE];
#endif
//static OS_STK  Stk_TaskScreenMain[APP_SCREEN_MAIIN_SIZE];

#if (PRODUCTS_LEVEL != FACTORY_UPGRADE_VERSION)		 /*���������汾ģʶ*/
static  OS_STK  Stk_TaskDcMet[APP_DCMET_STK_SIZE];

#ifndef DC_AC_ISO_REALCHECK
static  OS_STK  Stk_TaskIsoChk[APP_ISO_STK_SIZE];


#ifdef AC_METER
extern void TaskAcMeter(void *p_arg);
static  OS_STK  Stk_TaskAcMet[APP_ACMET_STK_SIZE];
#endif
#endif

static OS_STK  Stk_TaskDlMod[APP_DLMOD_STK_SIZE];

static OS_STK  Stk_TaskAGunBmsComm[APP_BMS_STK_SIZE];
#ifndef A_B_GUN_TOGETER
static OS_STK  Stk_TaskBGunBmsComm[APP_BMS_STK_SIZE];
#endif

#ifndef GUN_BMS_CTRL
static OS_STK  Stk_TaskAGunMainCtrl[APP_MAINCTRL_STK_SIZE];
//static OS_STK  Stk_TaskBGunMainCtrl[APP_MAINCTRL_STK_SIZE];
#endif

#endif

#ifndef DC_AC_ISO_REALCHECK
static OS_STK  Stk_TaskRealChk[APP_REALCHK_STK_SIZE];
#endif
static OS_STK  Stk_TaskBackComm[APP_BACK_STK_SIZE];

#ifdef USB_HOST_SUPPORT
static  OS_STK  Stk_TaskUSBH[APP_USBH_STK_SIZE];
#endif


static FATFS fatfs = { 0 };

// ========================================================================================================
// void sysSTM32F40xAssertClocks(void)
// ����: 	ϵͳʱ�Ӽ��
// ����ֵ: ��
// ========================================================================================================
static void sysSTM32F40xAssertClocks(void)
{
	RCC_ClocksTypeDef RCC_Clocks = { 0 };
	
	RCC_GetClocksFreq(&RCC_Clocks);
	
	if (RCC_Clocks.SYSCLK_Frequency != SystemCoreClock)
	{
		while (1);
	}
}

// ========================================================================================================
// void RCC_Configuration(void)
// ����: 	 ����ʱ��ϵͳ SYSCLK=HCLK=PCLK2=72M	PCLK1=36M 	�Լ�����ʱ��
// ����ֵ: ��
// ========================================================================================================
static void RCC_Configuration(void)
{																	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE); 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE); 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
	
	sysSTM32F40xAssertClocks();
}

// ========================================================================================================
// void sysNVICGroupInit(void)
// ����: 	ϵͳ�жϷ���
// ����ֵ: ��
// ========================================================================================================
static void sysNVICGroupInit(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);   //patli 20191217
}


#if 0
// ========================================================================================================
// void sys_ADC1_Init(void)
// ����: 	mp2515�ӿڳ�ʼ��
// ����ֵ: ��
// ========================================================================================================
#define N 50 //ÿͨ����50��
#define M 12 //Ϊ12��ͨ��

u16 AD_Value[N][M]; //�������ADCת�������Ҳ��DMA��Ŀ���ַ
u16 After_filter[M]; //���������ƽ��ֵ֮��Ľ��
u16 AdcValue[5];

void DMA_Configuration(void)
{

	DMA_InitTypeDef DMA_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);/*DMA2��ʱ��ʹ��*/
	while(DMA_GetCmdStatus(DMA2_Stream0)!=DISABLE);/*�ȴ�DMA��������*/

	
	DMA_InitStructure.DMA_Channel = DMA_Channel_0;/*DMAͨ��0*/
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)ADC1_BASE+0x4C;/*�����ַ*/
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)AdcValue;/*��ȡ����ַ*/
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;/*��������赽�ڴ�*/
	DMA_InitStructure.DMA_BufferSize = 5;/*���ݴ��������Ϊ1*/
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;/*��ַ������*/
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;/*��ַ����*/
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;/*���ݳ��Ȱ���*/
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;/*���ݳ��Ȱ���*/
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;/*�����ȼ�*/
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;/*ѭ��ģʽ*/
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;/*��ֹFIFO*/
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;/*FIFO��ֵ*/
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;/*���δ���*/
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;/*���δ���*/
	DMA_Init(DMA2_Stream0,&DMA_InitStructure);/**/
	DMA_Cmd(DMA2_Stream0,ENABLE);//����DMA����

}

static void sys_ADC1_Init(void)
{
	ADC_InitTypeDef       ADC_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	GPIO_InitTypeDef      GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);  
  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	
	ADC_DeInit();
	DMA_Configuration();
	/* Configure ADC1 Channel8 pin as analog input ****gun temp**************************/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure); 

	
	/* Configure ADC1 Channel9 pin as analog input ******gun cc1************************/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);


	/* Configure ADC1 Channel10 pin as analog input ***********iso vdc+*******************/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
   	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  	GPIO_Init(GPIOC, &GPIO_InitStructure); 
		
	/* Configure ADC1 Channel11 pin as analog input *********iso vdc-*********************/
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC, &GPIO_InitStructure); 
			  
	/* Configure ADC1 Channel12 pin as analog input **********gun vdc diff********************/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC, &GPIO_InitStructure); 


	/* ADC Common Init **********************************************************/
	ADC_CommonStructInit(&ADC_CommonInitStructure);
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
  	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
  	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
  	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
  	ADC_CommonInit(&ADC_CommonInitStructure);
	
	/* ADC1 Init ****************************************************************/
	ADC_StructInit(&ADC_InitStructure);
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
  	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
  	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  	ADC_InitStructure.ADC_NbrOfConversion = 5;  //patli 20200402 1;
  	ADC_Init(ADC1, &ADC_InitStructure);
	
	/* ADC1 regular channel6 configuration **************************************/
	ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_56Cycles);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_9, 2, ADC_SampleTime_56Cycles);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 3, ADC_SampleTime_56Cycles);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 4, ADC_SampleTime_56Cycles);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 5, ADC_SampleTime_56Cycles);
	
	
	// ����ADC��DMA֧�֣�Ҫʵ��DMA���ܣ������������DMAͨ���Ȳ�����
//	ADC_DMACmd(ADC1, ENABLE);
	
	/* Enable ADC1 */
  	ADC_Cmd(ADC1, ENABLE);

//	ADC_ResetCalibration(ADC1); //��λָ����ADC1��У׼�Ĵ���
	
//	while (ADC_GetSoftwareStartConvStatus(ADC1) != RESET);
	ADC_SoftwareStartConv(ADC1);
	
//	while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);

	ADC_DMARequestAfterLastTransferCmd(ADC1,ENABLE);//Դ���ݱ仯ʱ����DMA����
	
	
	ADC_DMACmd(ADC1,ENABLE);//ʹ��ADC����
	

}
#endif

// ========================================================================================================
// void FileSystem32_Init(void)
// ����: 	mp2515�ӿڳ�ʼ��
// ����ֵ: ��
// ========================================================================================================
static void FileSystem32_Init(void)
{
	uint8 res = 0;

	FIL   filRecordFat = { 0 };
	
	res = f_mount(1, &fatfs);         //���ش洢��

	res = f_open(&filRecordFat, "1:Record.dat", FA_READ);  //�򿪼�¼�ļ��Ƿ����
	f_close(&filRecordFat); //�ر��ļ�
	
	if (FR_OK != res)  //�򿪲��ɹ�����¼�ļ�������
	{
		f_mkfs(1, 1, SST_BLOCK_SIZE);    //��ʽ��FLASH, �̷�; 1, ����Ҫ������, 1����Ϊ1����
	
		f_open(&filRecordFat, "1:Record.dat", FA_CREATE_ALWAYS | FA_WRITE);  //������¼�ļ�
		f_close(&filRecordFat); //�ر��ļ�
	}				
}

// ==============================================================================
// void DeviceInit(void)
// �������豸��ʼ������ 
// ==============================================================================
static void DeviceInit(void)
{
	sysNVICGroupInit();
	
	fm24lc_init();
	
	sst_init();	
	
	InPut_OutPut_Init();
	
	sys_ADC1_Init();            //ADC1��ʼ��
	
	sysSerialHwInit();          // ����Ӳ����ʼ��

	sysRtcHwInit();             // RTC��ʼ��
	
#if (PRODUCTS_LEVEL != FACTORY_UPGRADE_VERSION)	   /*���������汾ģʶ*/

	FileSystem32_Init();	
	
#endif

#if (WATCHDOG_ENABLE == WDG_ENABLE )		
#if (CFG_IWDG_EN)
	sysIWdgInit();
#endif

#if (CFG_WWDG_EN)
	sysWWdgInit();
#endif 
#endif 
	
	DelaySec(2);	//patli 20200215 for dev stable

#ifndef STM32F407	
	Rd_InitRecord();
#endif

}

#if (OS_TASK_STAT_STK_CHK_EN)
#define STK_CHECK_STK_SIZE 300
static  OS_STK  StkCheckTaskStk[STK_CHECK_STK_SIZE];
extern uint8 charge_id[16];

// ����ջ��
static void StkCheckTask(void *p_arg)
{ 
	INT8U i;   
	char str[40];	
	OS_STK_DATA StackBytes;   
	
	OSTimeDlyHMSM(0,0,1,0);   
	
	while (DEF_TRUE)	 
	{	   
#if 1
			OSTaskStkChk(APP_TASK_DCMETER_PRIO,&StackBytes); 
//			memset(str, 0, sizeof(str));

#if 1			
			printf("DCMETER Free:%u,Used:%u\r\n",(INT16U)StackBytes.OSFree,(INT16U)StackBytes.OSUsed); 
//			An_Print(str);
//		if(StackBytes.OSFree < APP_DCMET_STK_SIZE*0.2)			
//			setbit(task_run_status, APP_TASK_DCMETER_PRIO);
		
		OSTaskStkChk(APP_TASK_DLMOD_PRIO,&StackBytes);	 
//			memset(str, 0, sizeof(str));
			printf( "DLMOD Free:%u,Used:%u\r\n",(INT16U)StackBytes.OSFree,(INT16U)StackBytes.OSUsed);  
//			An_Print(str);
//		if(StackBytes.OSFree < APP_DLMOD_STK_SIZE*0.2)			
//			setbit(task_run_status, APP_TASK_DLMOD_PRIO);
		
			OSTaskStkChk(APP_TASK_AGUNBMS_PRIO,&StackBytes);   
//			memset(str, 0, sizeof(str));
			printf("AGUNBMS Free:%u,Used:%u\r\n",(INT16U)StackBytes.OSFree,(INT16U)StackBytes.OSUsed);  
//			An_Print(str);

		
			OSTaskStkChk(APP_TASK_SCREEN_PRIO,&StackBytes);   
//			memset(str, 0, sizeof(str));
			printf("SCREEN Free:%u,Used:%u\r\n",(INT16U)StackBytes.OSFree,(INT16U)StackBytes.OSUsed);  
//			An_Print(str);
//		if(StackBytes.OSFree < APP_SCREEN_MAIIN_SIZE*0.2)			
//			setbit(task_run_status, APP_TASK_SCREEN_PRIO);
		
			OSTaskStkChk(APP_TASK_BACK_PRIO,&StackBytes);	
			printf("BACK Free:%u,Used:%u\r\n",(INT16U)StackBytes.OSFree,(INT16U)StackBytes.OSUsed);  
//			An_Print(str);
//		if(StackBytes.OSFree < APP_BACK_STK_SIZE*0.2)			
//			setbit(task_run_status, APP_TASK_BACK_PRIO);
		
#endif			
 			printf("dev run time = %d\r\n",DevRunSec);  
#endif		


		OSTimeDlyHMSM(0,0,23,0);
	}
}

#endif

#if (PRODUCTS_LEVEL == FACTORY_UPGRADE_VERSION)	   /*���������汾ģʶ*/
uint8 application_status = JUDGE_TO_USER_PROGRAM;
#endif
// ====================================================================================================
// static  void  AppTaskStart (void *p_arg)
// ����   : ����ϵͳ�������� 
// ����   : p_arg   δ��
// ����ֵ : ��
// ====================================================================================================	
static void AppTaskStart(void *p_arg)
{		
	uint32 watiticks = 0;
	p_arg = p_arg;
	uint8 cnt = 0, timecnt = 3;
	uint8 tmcnt = 0, err;
	
	SystemTickInit();
	
  	MainCtrl_Init();
	
#if (PRODUCTS_LEVEL == FACTORY_UPGRADE_VERSION)	
if(application_status != JUDGE_TO_USB_PROGRAM)
#endif	
	DeviceInit();


#ifdef AUNICE_DEBUG  //patli 20191015
		adebug_init();
#endif
	
#ifdef BMS_USE_TIMER
		TimerInit();
#endif

#if (OS_TASK_STAT_STK_CHK_EN)

#ifndef STM32F407	

	OSTaskCreateExt(TaskDcMeter,   	(void *)0, &Stk_TaskDcMet[APP_DCMET_STK_SIZE - 1],  APP_TASK_DCMETER_PRIO, APP_TASK_DCMETER_PRIO, Stk_TaskDcMet, APP_DCMET_STK_SIZE, NULL, OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);		
 	OSTaskCreateExt(Task_CdModule, 	(void *)0, &Stk_TaskDlMod[APP_DLMOD_STK_SIZE - 1], APP_TASK_DLMOD_PRIO, APP_TASK_DLMOD_PRIO, Stk_TaskDlMod, APP_DLMOD_STK_SIZE, NULL, OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
  	OSTaskCreateExt(TaskAGunBmsComm, 		(void *)0, &Stk_TaskAGunBmsComm[APP_BMS_STK_SIZE - 1], 	  APP_TASK_AGUNBMS_PRIO, APP_TASK_AGUNBMS_PRIO,  Stk_TaskAGunBmsComm, APP_BMS_STK_SIZE, NULL, OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);	
#ifndef A_B_GUN_TOGETER
  	OSTaskCreateExt(TaskBGunBmsComm, 		(void *)0, &Stk_TaskBGunBmsComm[APP_BMS_STK_SIZE - 1], 	  APP_TASK_BGUNBMS_PRIO, APP_TASK_BGUNBMS_PRIO,  Stk_TaskBGunBmsComm, APP_BMS_STK_SIZE, NULL, OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);	
#endif
//	OSTaskCreateExt(Task_Screen_Main,(void *)0, &Stk_TaskScreenMain[APP_SCREEN_MAIIN_SIZE - 1], APP_TASK_SCREEN_PRIO, APP_TASK_SCREEN_PRIO, Stk_TaskScreenMain, APP_SCREEN_MAIIN_SIZE, NULL, OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
//	OSTaskCreateExt(Task_BackComm, 	(void *)0, &Stk_TaskBackComm[APP_BACK_STK_SIZE - 1], 	APP_TASK_BACK_PRIO, APP_TASK_BACK_PRIO, Stk_TaskBackComm, APP_BACK_STK_SIZE, NULL, OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

	err = OSTaskCreate(StkCheckTask,                    NULL,                    (OS_STK *)&StkCheckTaskStk[STK_CHECK_STK_SIZE - 1],                    STK_CHECK_TASK_PRIO); 
#endif

	Task_Screen_init();

#else	

#if (PRODUCTS_LEVEL != FACTORY_UPGRADE_VERSION)	   /*���������汾ģʶ*/
	
  	OSTaskCreate(TaskDcMeter,   	(void *)0, &Stk_TaskDcMet[APP_DCMET_STK_SIZE - 1], 				OS_USER_PRIO_GET(APP_TASK_DCMETER_PRIO));	

 	OSTaskCreate(Task_CdModule, 	(void *)0, &Stk_TaskDlMod[APP_DLMOD_STK_SIZE - 1], 				OS_USER_PRIO_GET(APP_TASK_DLMOD_PRIO));
	
  	OSTaskCreate(TaskAGunBmsComm, 		(void *)0, &Stk_TaskAGunBmsComm[APP_BMS_STK_SIZE - 1], 	  OS_USER_PRIO_GET(APP_TASK_AGUNBMS_PRIO));	

#endif


#if (PRODUCTS_LEVEL == FACTORY_UPGRADE_VERSION)	   /*���������汾ģʶ*/
	//��USB BOOT
	if(application_status != JUDGE_TO_USB_PROGRAM)
#endif		
	{
//	OSTaskCreate(Task_Screen_Main,(void *)0, &Stk_TaskScreenMain[APP_SCREEN_MAIIN_SIZE - 1],OS_USER_PRIO_GET(APP_TASK_SCREEN_PRIO));
	OSTaskCreate(Task_BackComm, 	(void *)0, &Stk_TaskBackComm[APP_BACK_STK_SIZE - 1], 			OS_USER_PRIO_GET(APP_TASK_BACK_PRIO));

 	Task_Screen_init();
	
	}


#ifdef USB_HOST_SUPPORT	
	// USB BOOT
	if(application_status == JUDGE_TO_USB_PROGRAM)
	{
	//	extern void TaskUSBHBootLoader(void *p_arg);
		OSTaskCreate(TaskUSB,         (void *)0, &Stk_TaskUSBH[APP_USBH_STK_SIZE - 1],          OS_USER_PRIO_GET(APP_TASK_USBH_PRIO));
	}
#endif

#endif

	while(1) 
	{
		
		TaskRunTimePrint("AppTaskStart begin", OSPrioCur);

    	if(GetSystemTick() - watiticks > timecnt*TIM_10MS ){
			watiticks = GetSystemTick();
			LED_RUN();
			//Time_GetCalendarTime();
	//		if(cnt++ > 100){
	//			timecnt = 30;
	//			cnt=0;
	//		}
			
			if( tmcnt++ > 10 ) { /*3s����һ��*/
				tmcnt = 0;
				Bill_GetCurrentFeeReate(AGUN_NO);
				Bill_GetCurrentFeeReate(BGUN_NO);
			}
			
		}
		
		Wdg_feeddog();

		TaskRunTimePrint("AppTaskStart end", OSPrioCur);

    	if (OSRunning == OS_TRUE) {
			Delay10Ms(10); 
		}	
	
	  	Delay10Ms(APP_TASK_START_DELAY);  //patli 20200115		
	
	}
}

#if (PRODUCTS_LEVEL == FACTORY_UPGRADE_VERSION)
/* Exported variables --------------------------------------------------------*/



/* Exported types ------------------------------------------------------------*/
typedef void (*pFunction)(void);

/* Private variables ---------------------------------------------------------*/
pFunction Jump_To_Application = 0;
uint32 JumpAddress = 0;  

/* Private variables ---------------------------------------------------------*/
#define STM32F407ZG_UPGRADE_FLASH_SIZE   (512 * 1024)

#define FACTORY_APPLICATION_ADDRESS   ADDR_FLASH_SECTOR_5
#define USER_APPLICATION_ADDRESS   ADDR_FLASH_SECTOR_8

/* Private variables ---------------------------------------------------------*/
static uint32 ApplicationAddress        = FACTORY_APPLICATION_ADDRESS;

/* Private variables ---------------------------------------------------------*/
static const char strProgramDoneFlag[] = "The Aunice program has already done.";
static const uint32 set_program_mark_position = (USER_FLASH_END_ADDRESS + 1) - 128;

/* Publice variables-------------------------------------------------------*/
uint32 set_program_crc_position  = (USER_FLASH_END_ADDRESS + 1) - 4;
uint32 set_upgrade_flag_position  	= FACTORY_SETTING_BASE_ADDRESS;    //0x08010000     UPGRADE FLAG
uint32 set_upgrade_len_position  	= FACTORY_SETTING_BASE_ADDRESS+4;    //0x08010000  UPGRADE DATA LEN


#define USER_PROGRAM_ON        	0x01     //�����������������������û�������� 
#define USER_PROGRAM_OFF        0x00 	 //�û������������

// ==============================================================================
// void DeviceInit(void)
// �������豸��ʼ������ 
// ==============================================================================
static void BOOTDeviceInit(void)
{
	sysNVICGroupInit();
	
	RTC_BKPDomain();
	
	InPut_OutPut_Init();
	
	sysSerialHwInit();          // ����Ӳ����ʼ��
	
	sysRtcHwInit();             // RTC��ʼ��
	
}


/**
  * @brief  IAP_To_Application.
  * @param  None
  * @retval None
  */
void IAP_To_Application(void)
{
	/* Test if user code is programmed starting from address "ApplicationAddress" */
//patli 20200210	if (((*(__IO uint32*)ApplicationAddress) & 0x2FFE0000 ) == 0x20000000)
	{ 
		/* Jump to user application */
		JumpAddress = *(__IO uint32*) (ApplicationAddress + 4);
		Jump_To_Application = (pFunction) JumpAddress;
		/* Initialize user application's Stack Pointer */
		__set_MSP(*(__IO uint32*) ApplicationAddress);
		Jump_To_Application();
	}
}


/**
  * @brief  Judge_To_Application.
  * @param  None
  * @retval None
  */
uint8 Judge_To_Application(void)
{
	char strMarkBuffer[124] = { 0 };
	uint32 programRegionCRC = 0;
	uint32 programMarkCRC = 0;	
	uint32 upgrade_flag= 0;
	uint8 isUpgradeProgramExist = 0;
	uint8 isUSBHDeviceConnected = 0;
	
	uint32 deviceReg = 0;
	uint32 upgradeReg = 0;
	uint32 ftp_upgrade = 0;
	uint32 upgrade_len = 0;
	
	deviceReg  = ReadFromBackupReg(0);
//	upgradeReg = ReadFromBackupReg(2);
//	ftp_upgrade = ReadFromBackupReg(3);

	
//	printf("Judge_To_Application begin\n\r");	


	if (REGDEFAULTVALUE == deviceReg)
	{
		//��һ���ϵ磬����BOOT���U���������
		return JUDGE_TO_USB_PROGRAM;
	}
	
	WriteToBackupReg(0, 0);


	//read upgrade flag
	memcpy(&upgrade_flag, (uint32_t *)set_upgrade_flag_position, sizeof(uint32_t)); 

	//read upgrade len
	memcpy(&upgrade_len, (uint32_t *)set_upgrade_len_position, sizeof(uint32_t)); 
	
//	printf("Judge_To_Application upgrade_flag=0x%x, upgrade_len=0x%x\n\r", upgrade_flag, upgrade_len);

	//0.�û����򷢳���Ҫ����ָ��,Ҫ�����г����������������û�����
	if(upgrade_flag == USER_PROGRAM_ON || upgrade_len > STM32F407ZG_UPGRADE_FLASH_SIZE)	
	{
	
//		printf("Judge_To_Application return FACTORY_PROGRAM\n\r");
		return JUDGE_TO_FACTORY_PROGRAM;
	}

#if 0 //patli 20200209
	memcpy(strMarkBuffer, (uint8_t *)set_program_mark_position, strlen(strProgramDoneFlag));
#endif

	memcpy(&programMarkCRC, (uint8 *)set_program_crc_position, sizeof(uint32));	
	programRegionCRC = Calc_CRC32_Crypto((uint8 *)USER_APPLICATION_ADDRESS, upgrade_len);

#if 0	
	if ((0 == strcmp(strMarkBuffer, strProgramDoneFlag)) && (programMarkCRC == programRegionCRC))
#else
	if (programMarkCRC == programRegionCRC)	
#endif
	{		
		//1.������������ȷ���û����������û�����
//		printf("Judge_To_Application return JUDGE_TO_USER_PROGRAM\n\r");
		return JUDGE_TO_USER_PROGRAM;
	}

	//2.ȱʡ���г�����������
	return JUDGE_TO_FACTORY_PROGRAM;
	
}

/**
  * @brief  Where_To_Application.
  * @param  None
  * @retval None
  */
uint8 Where_To_Application(void)
{
	uint8 status = 0;
	
	status = Judge_To_Application();
	
//	printf("Where_To_Application status=%d\n\r", status);	
		
	if(status == JUDGE_TO_USER_PROGRAM)
	{
			ApplicationAddress = USER_APPLICATION_ADDRESS;
			
			IAP_To_Application();
	}	

	return status;
}

#endif


// =========================================================================================================
// main(void)
// ����: ������� 
// ��������
// ���أ���
// =========================================================================================================
int main (void) 
{
	
//	uint8 status = 0;
	/* System Clocks Configuration */
	RCC_Configuration();	
	
#if (PRODUCTS_LEVEL == FACTORY_UPGRADE_VERSION)
	BOOTDeviceInit();
	application_status = Where_To_Application();	
#endif


	OSInit();  
	  
	OSTaskCreate(AppTaskStart, (void *)0,	(OS_STK *)&AppTaskStartStk[APP_START_STK_SIZE - 1], APP_TASK_START_PRIO);	// Create the start task

	OSStart();                                                  

}

/**
  * @brief  Inserts a delay time.
  * @param  nCount: number of 1ms periods to wait for.
  * @retval None
  */
void Main_Delay(int32_t nCount)
{
	int32_t time = 0;
	
	while (nCount--)
	{
		time = 0xA380;
		
		while (time--)
		{
			
		}
	}
}

/**
  * @}
  */

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
