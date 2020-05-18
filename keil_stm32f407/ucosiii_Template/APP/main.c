#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "common.h"

#include "gpio.h"
#include "dma.h"
#include "iwdg.h"
#include "timer.h"
#include "24cxx.h"
#include "ds18b20.h"
#include "w25qxx.h"
#include "lwip_comm.h"
#include "lwipopts.h"

#include "malloc.h"

#include "includes.h"
#include "os_app_hooks.h"


#include "MainData.h"
#include "Debug.h"

//ALIENTEK ̽����STM32F407������ UCOSIIIʵ��
//��4-1 UCOSIII UCOSIII��ֲ

//UCOSIII���������ȼ��û�������ʹ�ã�ALIENTEK
//����Щ���ȼ��������UCOSIII��5��ϵͳ�ڲ�����
//���ȼ�0���жϷ������������� OS_IntQTask()
//���ȼ�1��ʱ�ӽ������� OS_TickTask()
//���ȼ�2����ʱ���� OS_TmrTask()
//���ȼ�OS_CFG_PRIO_MAX-2��ͳ������ OS_StatTask()
//���ȼ�OS_CFG_PRIO_MAX-1���������� OS_IdleTask()
//����֧�֣�www.openedv.com
//�Ա����̣�http://eboard.taobao.com  
//������������ӿƼ����޹�˾  
//���ߣ�����ԭ�� @ALIENTEK

//�������ȼ�
#define START_TASK_PRIO		3
//�����ջ��С	
#define START_STK_SIZE 		512
//������ƿ�
OS_TCB StartTaskTCB;
//�����ջ	
CPU_STK START_TASK_STK[START_STK_SIZE];
//������
void start_task(void *p_arg);

//�������ȼ�
#define LED1_TASK_PRIO		4
//�����ջ��С	
#define LED1_STK_SIZE 		128*4
//������ƿ�
OS_TCB Led1TaskTCB;
//�����ջ	
CPU_STK LED1_TASK_STK[LED1_STK_SIZE];
void led1_task(void *p_arg);

////�������ȼ�
//#define LED2_TASK_PRIO		5
////�����ջ��С	
//#define LED2_STK_SIZE 		128
////������ƿ�
//OS_TCB Led2TaskTCB;
////�����ջ	
//CPU_STK LED2_TASK_STK[LED2_STK_SIZE];
////������
//void led2_task(void *p_arg);
//�������ȼ�
#define CARDREADER_TASK_PRIO		5
//�����ջ��С	
#define CARDREADER_STK_SIZE 		128
//������ƿ�
OS_TCB CardReaderTaskTCB;
//�����ջ	
CPU_STK CARDREADER_TASK_STK[CARDREADER_STK_SIZE];
//������
void CardReader_task(void *p_arg);

//�������ȼ�
#define FLOAT_TASK_PRIO		6
//�����ջ��С
#define FLOAT_STK_SIZE		256
//������ƿ�
OS_TCB	FloatTaskTCB;
//�����ջ
CPU_STK	FLOAT_TASK_STK[FLOAT_STK_SIZE];
//������
void float_task(void *p_arg);


//�������ȼ�
#define RealTimeCheck_TASK_PRIO		7
//�����ջ��С
#define RealTimeCheck_STK_SIZE		128
//������ƿ�
OS_TCB	RealTimeCheckTaskTCB;
//�����ջ
CPU_STK	RealTimeCheck_TASK_STK[RealTimeCheck_STK_SIZE];
//������
void RealTimeCheck_task(void *p_arg);


//�������ȼ�
#define TaskStackUsage_TASK_PRIO		30
//�����ջ��С	
#define TaskStackUsage_STK_SIZE 		128
//������ƿ�
OS_TCB TaskStackUsageTaskTCB;
//�����ջ	
CPU_STK TaskStackUsage_TASK_STK[TaskStackUsage_STK_SIZE];
void TaskStackUsage_task(void *p_arg);



//test code������
void led1_task(void *p_arg)
{
	OS_ERR err;
	p_arg = p_arg;
	
	CHARGE_TYPE *PtrRunData = ChgData_GetRunDataPtr();
	uint8_t k2status = PtrRunData->input->statu.bits.key2;
	uint8_t k3status = PtrRunData->input->statu.bits.key3;
	uint8_t *p = NULL;
	uint32_t tmptick = 0;
	u8 datatemp[50] = {0};
	//-----------------test spi func
//	const u8 TEXT_Buffer[]={"Explorer STM32F4 SPI TEST"};
//	u32 FLASH_SIZE=16*1024*1024;	//FLASH ��СΪ16�ֽ�
//	u8 SIZE = sizeof(TEXT_Buffer);
//	
//	W25QXX_Write((u8*)TEXT_Buffer,FLASH_SIZE-100,SIZE);
//	OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ1s
//	W25QXX_Read(datatemp,FLASH_SIZE-100,SIZE);
//	DEBUG_Printf((char *)datatemp);
//-----------------
	
//-----------------test lwip
	while(lwip_comm_init()) 		//lwip��ʼ��
	{
		DEBUG_Printf("Lwip Init failed!\r\n"); 	//lwip��ʼ��ʧ��
		OSTimeDlyHMSM(0,0,2,0,OS_OPT_TIME_HMSM_STRICT,&err);  
	}
#if LWIP_DHCP
	lwip_comm_dhcp_creat(); //����DHCP����
#endif
//-----------------	
	
	while(1)
	{
		if(k2status != PtrRunData->input->statu.bits.key2)
		{
			k2status = PtrRunData->input->statu.bits.key2;
			if(1 == k2status)
			{
//				p=mymalloc(SRAMIN,2048);//����2K�ֽ�  
				LED_Toggle(LED2);
			}
		}
		
		if(k3status != PtrRunData->input->statu.bits.key3)
		{
			k3status = PtrRunData->input->statu.bits.key3;
			if(1 == k3status)
			{
//				myfree(SRAMIN,p);//�ͷ��ڴ�
				LED_Toggle(LED3);
			}
		}

		
		tmptick = GetSystemTick();
		sprintf(DEBUG_Buff,"CPU tick times:%d,ETH LINK:%d\r\n", tmptick,DP83848_GetPHYlinkStatus());
		DEBUG_Printf(DEBUG_Buff);
		
		OSTimeDlyHMSM(0,0,2,20,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ20ms
	}
}

////led1������
//void led2_task(void *p_arg)
//{
//	OS_ERR err;
//	p_arg = p_arg;
//	while(1)
//	{
//		LED_Toggle(LED3);
//		OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ500ms
//	}
//}


//�����������
void float_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	uint8_t i;
	uint16_t adcx = 0;
	static float float_num[3]={0};
	float pro=0;//����
	uint8_t SEND_BUF_SIZE = 20;
  uint16_t *SendBuff = DMA_GetAdcAver();
	
	
	
	while(1)
	{
//		float_num+=0.01f;
//		OS_CRITICAL_ENTER();	//�����ٽ���


//	  if(DMA_GetFlagStatus(DMA2_Stream0,DMA_FLAG_TCIF0)!=RESET)//�ȴ�DMA2_Steam0�������
//		{ 
//		  for(i = 0; i < SEND_BUF_SIZE;i++)
//			{
//				adcx += SendBuff[i];
//			}
//		    adcx = adcx/SEND_BUF_SIZE;
//		    DMA_ClearFlag(DMA2_Stream0,DMA_FLAG_TCIF0);//���DMA2_Steam7������ɱ�־
//		}
//		pro=DMA_GetCurrDataCounter(DMA2_Stream0);//�õ���ǰ��ʣ����ٸ�����
//		pro=1-pro/30;//�õ��ٷֱ�	  
//		pro*=100;      			    //����100��
		
//		adcx=Get_Adc_Average(ADC_Channel_10,20);
		for(i = 0; i < 3; i++)
		{
		  float_num[i]=(float)SendBuff[i]*(3.3/4096);
		}			
		sprintf(DEBUG_Buff,"float_num[0]=%.4f,float_num[1]=%.4f,18b20=%d,temperature=%f\r\n",float_num[0],float_num[1],ChgData_GetRunDataPtr()->dev_status->temperature,DMA_GetTemprate());
    DEBUG_Printf(DEBUG_Buff);
//		OS_CRITICAL_EXIT();		//�˳��ٽ���
			 

//		LED_Toggle(LED3);  //���е�
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ1s

	}
}


//TaskStackUsage������
void TaskStackUsage_task(void *p_arg)
{
	OS_ERR err;
	p_arg = p_arg;
	CPU_STK_SIZE free,used;

	DEBUG_Printf("app start running!\r\n");
	while(1)
	{
		sprintf(DEBUG_Buff,"CPU useage:%d\r\n", OSStatTaskCPUUsage);
		DEBUG_Printf(DEBUG_Buff);
		
		OSTaskStkChk(&TaskStackUsageTaskTCB,&free,&used,&err);
		sprintf(DEBUG_Buff,"TaskStackUsageTask used/free:%d/%d  usage:%%%d\r\n",used,free,(used*100)/(used+free));
		DEBUG_Printf(DEBUG_Buff);
		
		OSTaskStkChk(&Led1TaskTCB,&free,&used,&err);
		sprintf(DEBUG_Buff,"Led1TaskTCB used/free:%d/%d  usage:%%%d\r\n",used,free,(used*100)/(used+free));
		DEBUG_Printf(DEBUG_Buff);
		
		OSTaskStkChk(&CardReaderTaskTCB,&free,&used,&err);
		sprintf(DEBUG_Buff,"Led2TaskTCB used/free:%d/%d  usage:%%%d\r\n",used,free,(used*100)/(used+free));
		DEBUG_Printf(DEBUG_Buff);
		
		OSTaskStkChk(&FloatTaskTCB,&free,&used,&err);
		sprintf(DEBUG_Buff,"FloatTaskTCB used/free:%d/%d  usage:%%%d\r\n",used,free,(used*100)/(used+free));
		DEBUG_Printf(DEBUG_Buff);
		
		OSTaskStkChk(&RealTimeCheckTaskTCB,&free,&used,&err);
		sprintf(DEBUG_Buff,"RealTimeCheckTaskTCB used/free:%d/%d  usage:%%%d\r\n",used,free,(used*100)/(used+free));
		DEBUG_Printf(DEBUG_Buff);
		
		sprintf(DEBUG_Buff,"mymalloc size/useage:%d/%d\r\n", MEM1_MAX_SIZE,my_mem_perused(SRAMIN));
		DEBUG_Printf(DEBUG_Buff);
		
		DEBUG_Printf("\r\n\r\n\r\n");
		
		OSTimeDlyHMSM(0,0,3,0,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ3s
	}
}


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
// ����: 	ʹ�ܸ���ʱ��
// ����ֵ: ��
// ========================================================================================================
static void RCC_Configuration(void)
{																	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE); 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	
	sysSTM32F40xAssertClocks();
}

void Hanrdware_Init(void)
{

	delay_init(168);  	//ʱ�ӳ�ʼ��
	RCC_Configuration();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�жϷ�������
//	uart_init(115200);  //���ڳ�ʼ��
	sys_SerialHwInit();
	
	InPut_OutPut_Init();
	sys_ADC1_Config();
//	IWDG_Init(4,1500); //���Ƶ��Ϊ 64,����ֵΪ 1500,���ʱ��Ϊ 3s
	TimerInit();        //��ʱ����ʼ��
	AT24CXX_Init();     //24C02 ��ʼ��
  DS18B20_Init();	    //DS18B20��ʼ��
	W25QXX_Init();			//W25QXX��ʼ��
	
	my_mem_init(SRAMIN);		//��ʼ���ڲ��ڴ��
	
	DEBUG_Init();
}

void HardWare_Check(void)
{
	uint8_t tmpdata[4] = {0};
	uint32_t reboottimes = 0;
	if(AT24CXX_Check())//��ⲻ��24c02
	{
		DEBUG_Printf("24C02 Check Failed!\r\n");	
//		while(1);
	}
	else
	{
		sprintf(DEBUG_Buff,"24C02 Check Successfull!\r\n");
		DEBUG_Printf(DEBUG_Buff);
//		AT24CXX_Read(AT24Cxx_RebootTimes_ADDR,(uint8_t *)&reboottimes,4);
		reboottimes = AT24CXX_ReadLenByte(AT24Cxx_RebootTimes_ADDR,4);
		reboottimes = Common_Bcd2hex32(reboottimes) + 1;
		sprintf(DEBUG_Buff,"reboot times��%d!\r\n",reboottimes);
		DEBUG_Printf(DEBUG_Buff);
		reboottimes = Common_Hex2bcd32(reboottimes);
//		AT24CXX_Write(0,(uint8_t*)reboottimes,4);    //��BCD���ʽ�洢
		AT24CXX_WriteLenByte(AT24Cxx_RebootTimes_ADDR,reboottimes,4);
	}
	
	if(1 == DS18B20_Check())
	{
		DEBUG_Printf("DS18B20 Check Failed!\r\n");	
	}
	
	if(W25Q64 != W25QXX_ReadID())								//��ⲻ��W25Q128
	{
		DEBUG_Printf("W25Q64 Check Failed!\r\n");
	}
	
	
}

//��ʼ������
void start_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;

	CPU_Init();
	
#if OS_CFG_STAT_TASK_EN > 0u
   OSStatTaskCPUUsageInit(&err);  	//ͳ������                
#endif
	
#ifdef CPU_CFG_INT_DIS_MEAS_EN		//���ʹ���˲����жϹر�ʱ��
    CPU_IntDisMeasMaxCurReset();	
#endif

#if OS_CFG_APP_HOOKS_EN				//ʹ�ù��Ӻ���
	App_OS_SetAllHooks();			
#endif	
	
#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //��ʹ��ʱ��Ƭ��ת��ʱ��
	 //ʹ��ʱ��Ƭ��ת���ȹ���,ʱ��Ƭ����Ϊ1��ϵͳʱ�ӽ��ģ���1*5=5ms
	OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);  
#endif		
  HardWare_Check();
  MainCtrlUnit_Init();
	
	OS_CRITICAL_ENTER();	//�����ٽ���
	
	//�������������ջʹ���������
	OSTaskCreate((OS_TCB 	* )&TaskStackUsageTaskTCB,		
				 (CPU_CHAR	* )"TaskStackUsage task", 		
                 (OS_TASK_PTR )TaskStackUsage_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )TaskStackUsage_TASK_PRIO,     
                 (CPU_STK   * )&TaskStackUsage_TASK_STK[0],	
                 (CPU_STK_SIZE)TaskStackUsage_STK_SIZE/10,	
                 (CPU_STK_SIZE)TaskStackUsage_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);
	
	//����LED1����
	OSTaskCreate((OS_TCB 	* )&Led1TaskTCB,		
				 (CPU_CHAR	* )"led1 task", 		
                 (OS_TASK_PTR )led1_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )LED1_TASK_PRIO,     
                 (CPU_STK   * )&LED1_TASK_STK[0],	
                 (CPU_STK_SIZE)LED1_STK_SIZE/10,	
                 (CPU_STK_SIZE)LED1_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);				
				 
	//����CARDREADER����
	OSTaskCreate((OS_TCB 	* )&CardReaderTaskTCB,		
				 (CPU_CHAR	* )"CardReader task", 		
                 (OS_TASK_PTR )CardReader_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )CARDREADER_TASK_PRIO,     	
                 (CPU_STK   * )&CARDREADER_TASK_STK[0],	
                 (CPU_STK_SIZE)CARDREADER_STK_SIZE/10,	
                 (CPU_STK_SIZE)CARDREADER_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,				
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, 
                 (OS_ERR 	* )&err);
				 
	//���������������
	OSTaskCreate((OS_TCB 	* )&FloatTaskTCB,		
				 (CPU_CHAR	* )"float test task", 		
                 (OS_TASK_PTR )float_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )FLOAT_TASK_PRIO,     	
                 (CPU_STK   * )&FLOAT_TASK_STK[0],	
                 (CPU_STK_SIZE)FLOAT_STK_SIZE/10,	
                 (CPU_STK_SIZE)FLOAT_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,				
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, 
                 (OS_ERR 	* )&err);				
	//����ʵʱ�������
	OSTaskCreate((OS_TCB 	* )&RealTimeCheckTaskTCB,		
				 (CPU_CHAR	* )"RealTimeCheck test task", 		
                 (OS_TASK_PTR )RealTimeCheck_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )RealTimeCheck_TASK_PRIO,     	
                 (CPU_STK   * )&RealTimeCheck_TASK_STK[0],	
                 (CPU_STK_SIZE)RealTimeCheck_STK_SIZE/10,	
                 (CPU_STK_SIZE)RealTimeCheck_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,				
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, 
                 (OS_ERR 	* )&err);											 
	OS_CRITICAL_EXIT();	//�˳��ٽ���
	
								 
	while(1)
	{
		LED_Toggle(LED1);  //���е�
		IWDG_Feed();
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ1s
	}
//	OSTaskDel((OS_TCB*)0,&err);	//ɾ��start_task��������							 
			

}


int main(void)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	
	Hanrdware_Init();
	
	OSInit(&err);		//��ʼ��UCOSIII
	OS_CRITICAL_ENTER();//�����ٽ���
	//������ʼ����
	OSTaskCreate((OS_TCB 	* )&StartTaskTCB,		//������ƿ�
				 (CPU_CHAR	* )"start task", 		//��������
                 (OS_TASK_PTR )start_task, 			//������
                 (void		* )0,					//���ݸ��������Ĳ���
                 (OS_PRIO	  )START_TASK_PRIO,     //�������ȼ�
                 (CPU_STK   * )&START_TASK_STK[0],	//�����ջ����ַ
                 (CPU_STK_SIZE)START_STK_SIZE/10,	//�����ջ�����λ
                 (CPU_STK_SIZE)START_STK_SIZE,		//�����ջ��С
                 (OS_MSG_QTY  )0,					//�����ڲ���Ϣ�����ܹ����յ������Ϣ��Ŀ,Ϊ0ʱ��ֹ������Ϣ
                 (OS_TICK	  )0,					//��ʹ��ʱ��Ƭ��תʱ��ʱ��Ƭ���ȣ�Ϊ0ʱΪĬ�ϳ��ȣ�
                 (void   	* )0,					//�û�����Ĵ洢��
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //����ѡ��
                 (OS_ERR 	* )&err);				//��Ÿú�������ʱ�ķ���ֵ
	OS_CRITICAL_EXIT();	//�˳��ٽ���	 
	OSStart(&err);  //����UCOSIII
	while(1);
}
