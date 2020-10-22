
#include "rtc.h"

#if defined (STM32F10X_CL)
#include "stm32f10x_bkp.h"
#include "stm32f10x_rtc.h"
#endif

#if defined (STM32F4XX)
#include "stm32f4xx_rtc.h"
#include "common.h"
#endif

/*******************************************************************************
* ���ļ�ʵ�ֻ���RTC�����ڹ��ܣ��ṩ�����յĶ�д��������ANSI-C��time.h��
* ANSI-C�ı�׼���У��ṩ�����ֱ�ʾʱ�������  �ͣ�
* time_t:   	UNIXʱ�������1970-1-1��ĳʱ�侭����������
* 	typedef unsigned int time_t;
* 
* struct tm:	Calendar��ʽ����������ʽ��
*   tm�ṹ���£�
*   struct tm {
*   	int tm_sec;   // �� seconds after the minute, 0 to 60
*   					 (0 - 60 allows for the occasional leap second)
*   	int tm_min;   // �� minutes after the hour, 0 to 59
*		int tm_hour;  // ʱ hours since midnight, 0 to 23
*		int tm_mday;  // �� day of the month, 1 to 31
*		int tm_mon;   // �� months since January, 0 to 11
*		int tm_year;  // �� years since 1900
*		int tm_wday;  // ���� days since Sunday, 0 to 6
*		int tm_yday;  // ��Ԫ��������� days since January 1, 0 to 365
* 		int tm_isdst; // ����ʱ����Daylight Savings Time flag
* 		...
* 	}
* 	����wday��yday�����Զ����������ֱ�Ӷ�ȡ
* 	mon��ȡֵΪ0-11
*	***ע��***��
*	tm_year:��time.h���ж���Ϊ1900�������ݣ���2008��Ӧ��ʾΪ2008-1900=108
* 	���ֱ�ʾ�������û���˵����ʮ���Ѻã�����ʵ�нϴ���졣
* 	�����ڱ��ļ��У����������ֲ��졣
* 	���ⲿ���ñ��ļ��ĺ���ʱ��tm�ṹ�����͵����ڣ�tm_year��Ϊ2008
* 	ע�⣺��Ҫ����ϵͳ��time.c�еĺ�������Ҫ���н�tm_year-=1900
*******************************************************************************/

#if defined (STM32F4XX)
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define LSE_MAX_TRIALS_NB         6

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
int8_t RTC_Error = 0;

int8_t RTC_Configuration(RTC_InitTypeDef* RTC_InitStruct);
#endif


/*******************************************************************************
* Function Name  : Time_ConvUnixToCalendar(time_t t)
* Description    : RTCʱ�� -> ����ʱ��
* Input 		 : u32 t  ��ǰRTCʱ��
* Output		 : None
* Return		 : struct tm
*******************************************************************************/
struct tm Time_ConvUnixToCalendar(time_t t)
{
	struct tm *t_tm;
	t_tm = localtime(&t);
	t_tm->tm_year += 1900;	//localtimeת�������tm_year�����ֵ����Ҫת�ɾ���ֵ
	return *t_tm;
}

/*******************************************************************************
* Function Name  : Time_ConvCalendarToUnix(struct tm t)
* Description    : ת��calendarʱ�ӵ�RTCʱ�� 
* Input 		 : struct tm t
* Output		 : None
* Return		 : time_t
*******************************************************************************/
time_t Time_ConvCalendarToUnix(struct tm t)
{
	//t.tm_year -= 2000; 
	t.tm_year -= 1900;
    t.tm_mon -= 1;
	return mktime(&t);
}

/*******************************************************************************
* Function Name  : Time_GetUnixTime()
* Description    : ��ȡRTCʱ��
* Input 		 : None
* Output		 : None
* Return		 : time_t t
*******************************************************************************/
time_t Time_GetUnixTime(void)
{
	struct tm t_tm;
#if defined (STM32F10X_CL)
	return (time_t)RTC_GetCounter();
#endif

#if defined (STM32F4XX)
	t_tm = Time_GetCalendarTime();
	return (time_t)Time_ConvCalendarToUnix(t_tm);	
#endif
	
}

static struct tm gSystemTime;
/*******************************************************************************
* Function Name  : Time_GetCalendarTime()
* Description    : ��ȡ����ʱ�䣨struct tm��
* Input 		 : None
* Output		 : None
* Return		 : time_t t
*******************************************************************************/
struct tm Time_GetCalendarTime(void)
{
	
	struct tm t_tm;
#if defined (STM32F10X_CL)
	time_t t_t;
	t_t = (time_t)RTC_GetCounter();
	t_tm = Time_ConvUnixToCalendar(t_t);
	t_tm.tm_mon += 1; 		
#endif

#if defined (STM32F4XX)
	RTC_TimeTypeDef   RTC_TimeStructure;
	RTC_DateTypeDef   RTC_DateStructure;
	 /* Get info from RTC here */
	RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
	t_tm.tm_sec    =  RTC_TimeStructure.RTC_Seconds;
	t_tm.tm_min    =  RTC_TimeStructure.RTC_Minutes;
	t_tm.tm_hour   =  RTC_TimeStructure.RTC_Hours;	

	RTC_GetDate(RTC_Format_BIN, &RTC_DateStructure);
	t_tm.tm_year =  RTC_DateStructure.RTC_Year + 2000;	 //localtimeת�������tm_year�����ֵ����Ҫת�ɾ���ֵ
	t_tm.tm_mon =  RTC_DateStructure.RTC_Month;
	t_tm.tm_mday =  RTC_DateStructure.RTC_Date;
	t_tm.tm_wday =  RTC_DateStructure.RTC_WeekDay;
#endif
  
	if(t_tm.tm_year > 2003 && t_tm.tm_mon < 13  && t_tm.tm_hour < 25 ) {
		gSystemTime = t_tm;	
	}
	return t_tm;
}

struct tm Time_GetSystemCalendarTime(void)
{
	return gSystemTime;
}



 void Time_SetCalendarTime(struct tm t);
/*******************************************************************************
* Function Name  : Time_SetUnixTime()
* Description    : д��RTCʱ��
* Input 		 : time_t t
* Output		 : None
* Return		 : None
*******************************************************************************/
void Time_SetUnixTime(time_t t)
{
#if defined (STM32F10X_CL)
	RTC_WaitForLastTask();
	RTC_SetCounter((u32)t);
	RTC_WaitForLastTask();
#endif

#if defined (STM32F4XX)
	struct tm t_tm;
	t_tm = Time_ConvUnixToCalendar(t);
	Time_SetCalendarTime(t_tm);
#endif
	return;
}

/*******************************************************************************
* Function Name  : Time_SetCalendarTime()
* Description    : ��������Calendar��ʽʱ��д��RTC
* Input 		 : struct tm t
* Output		 : None
* Return		 : None
*******************************************************************************/
void Time_SetCalendarTime(struct tm t)
{
#if defined (STM32F10X_CL)
	t.tm_mon -= 1; 
	Time_SetUnixTime(Time_ConvCalendarToUnix(t));
#endif

#if defined (STM32F4XX)
	RTC_TimeTypeDef   RTC_TimeStructure;
	RTC_DateTypeDef   RTC_DateStructure;

//	RTC_InitTypeDef RTC_InitStruct;
//	RTC_Configuration(&RTC_InitStruct);	                                      // ����RTC
//	RTC_InitStruct.RTC_HourFormat = RTC_HourFormat_24;
//	RTC_Init(&RTC_InitStruct);
	
	RTC_TimeStructure.RTC_Seconds = t.tm_sec;
    RTC_TimeStructure.RTC_Minutes = t.tm_min;
    RTC_TimeStructure.RTC_Hours   = t.tm_hour;
    RTC_SetTime(RTC_Format_BIN, &RTC_TimeStructure);

    RTC_DateStructure.RTC_Year  = t.tm_year-2000;
    RTC_DateStructure.RTC_Month = t.tm_mon;
    RTC_DateStructure.RTC_Date  = t.tm_mday;
	RTC_DateStructure.RTC_WeekDay  = t.tm_wday;
    RTC_SetDate(RTC_Format_BIN, &RTC_DateStructure);
#endif

	return;
}

#if defined (STM32F10X_CL)
// ================================================================================================================
// void RTC_Configuration(void) RTC���� 
// ================================================================================================================

void RTC_Configuration(void)
{
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_AHB1Periph_BKPSRAM, ENABLE);	 // �򿪵�Դ�ͺ󱸽ӿ�ʱ��
  PWR_BackupAccessCmd(ENABLE);	                       // ʹ�ܶԺ󱸼Ĵ�����RTC�ķ��� 
  BKP_DeInit();		                                     // ��λ�󱸼Ĵ���
  RCC_LSEConfig(RCC_LSE_ON);	                         // 32.768K Hz �����ⲿг���� Ϊʵʱʱ���ṩһ���͹����Ҿ�׼��ʱ��Դ RCC_BDCR-LSEON 
  while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)	 // �ȴ� LSE�ȶ� 
  {}

  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);	             // ����LSE��ΪRTCʱ��Դ 
  RCC_RTCCLKCmd(ENABLE);	                             // ʹ�� RTCʱ�� 
  RTC_WaitForSynchro();			                           // ʱ�ӼĴ���ͬ�����  
  RTC_WaitForLastTask();			                         // �ȴ���RTC�Ĵ���д�������  
  RTC_WaitForLastTask();		                           // �ȴ���RTC�Ĵ���д�������
  RTC_SetPrescaler(32767);                             // RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) Ԥ��Ƶ�� ����Ϊ1S������
  RTC_WaitForLastTask();		                           // �ȴ���RTC�Ĵ���д�������
}
#endif



//int8_t Iwdg_LSI_Init()
//{
//	 int32 RTC_Error = 0,RTC_Timeout=0x10000;
//	 RCC_LSICmd(ENABLE);
//  /* Wait till LSI is ready */
//  while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
//  {
//    if (RTC_Timeout-- == 0)
//    {
//      RTC_Error = -1;
//			break;
//    }
//  }
//	return RTC_Error;
//}


#if defined (STM32F4XX)
/**
* @brief  Configures the RTC peripheral.
* @param  None
* @retval None
*/
int8_t RTC_Configuration(RTC_InitTypeDef* RTC_InitStruct)
{
  uint8_t trialno;
  int32 RTC_Error = 0,RTC_Timeout=0x10000;
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

  /* Allow access to BKP Domain */
  PWR_BackupAccessCmd(ENABLE);

#ifdef RTC_CLOCK_SOURCE_LSI
  /* Enable the LSI OSC */
  RCC_LSICmd(ENABLE);
  /* Wait till LSI is ready */
  while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
  {
    if (RTC_Timeout-- == 0)
    {
      RTC_Error = -1;
      break;
    }
  }
	
  if (RTC_Error == -1)
  {
    return -1;
  }

  /* Select the RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
#endif

#ifdef RTC_CLOCK_SOURCE_LSE

  for ( trialno = 0 ; trialno <= LSE_MAX_TRIALS_NB ; trialno++)
  {
    /* Enable the LSE OSC */
    RCC_LSEConfig(RCC_LSE_ON);
    RTC_Timeout = 0x500;         //old is 0x100  20190320 modify by 
    
    if ((trialno == 1) && (trialno == 4))
    {
      //GL_State_Message((uint8_t *)"RTC and backup Starting.. ");
    }

    else if ((trialno == 2)&& (trialno == 5))
    {
      //GL_State_Message((uint8_t *)"RTC and backup Starting...");
    }
    
    else if ((trialno == 3)&& (trialno == 6))
    {
      //GL_State_Message((uint8_t *)"RTC and backup Starting.  ");
    }
    OSTimeDly(2); //20181213
    /* Wait till LSE is ready */
    while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
    {
      if (RTC_Timeout-- == 0)
      {
        RTC_Error = -1;
        RCC_LSEConfig(RCC_LSE_OFF);
        break;
      }
	    OSTimeDly(2); //20190319 û��32K������ʱ������
    }
    
    if (RTC_Error == 0)
    {
      break;
    }
  }

  if (RTC_Error == -1)
  {
    return -1;
  }
  
  /* Select the RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
#endif
  
#ifdef RTC_CLOCK_SOURCE_HSE
  /* Select the RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_HSE_Div31);
#endif
  /* Enable the RTC Clock */
  RCC_RTCCLKCmd(ENABLE);

#ifdef RTC_CLOCK_SOURCE_HSE
  /*Calender Configuration*/
  RTC_InitStruct->RTC_AsynchPrediv =  0x189D;
  RTC_InitStruct->RTC_SynchPrediv = 0x7F;
#endif

#ifdef RTC_CLOCK_SOURCE_LSI
  /*Calender Configuration*/
  RTC_InitStruct->RTC_AsynchPrediv = 0x7F;
  RTC_InitStruct->RTC_SynchPrediv =  0xCB;
#endif

#ifdef RTC_CLOCK_SOURCE_LSE
  /*Calender Configuration*/
  RTC_InitStruct->RTC_AsynchPrediv = 0x7F;
  RTC_InitStruct->RTC_SynchPrediv =  0xFF;
#endif

  /* Wait for RTC APB registers synchronisation */
  if ( RTC_WaitForSynchro() == ERROR)
  {
    return -1;
  }
  return 0;
}

#endif


// ================================================================================
//  void Time_Adjust(void)
//  ʱ��У׼ 
//  ==============================================================================
void Time_Adjust(struct tm CalTime)
{
#if defined (STM32F10X_CL)
	RTC_Configuration();	                // ����RTC 
	PWR_BackupAccessCmd(ENABLE);	        // ʹ�ܶԺ󱸼Ĵ�����RTC�ķ��� 
	RTC_WaitForLastTask();				        // �ȴ��ϴ�д�Ĵ����������
	Time_SetCalendarTime(CalTime);
	RTC_WaitForLastTask();			          // �ȴ�д�Ĵ����������
	PWR_BackupAccessCmd(DISABLE);	        // ���ܶԺ󱸼Ĵ�����RTC�ķ��� 
	PWR_BackupAccessCmd(ENABLE);	        // ʹ�ܶԺ󱸼Ĵ�����RTC�ķ��� 
	BKP_WriteBackupRegister(BKP_DR1, BKP_RTC_CFG_FLAG);	        // д�������־	
	PWR_BackupAccessCmd(DISABLE);	        // ʹ�ܶԺ󱸼Ĵ�����RTC�ķ��� 
#endif

#if defined (STM32F4XX)
	//RTC_InitTypeDef RTC_InitStruct;
	//RTC_Configuration(&RTC_InitStruct);	                                      // ����RTC
	//RTC_InitStruct.RTC_HourFormat = RTC_HourFormat_24;
	//RTC_Init(&RTC_InitStruct);
	Time_SetCalendarTime(CalTime);
#endif	
}

//#include"appstart.h"
void sysRtcHwInit(void)
{	 
	struct tm CalTime;

#if defined (STM32F10X_CL)
	if (BKP_ReadBackupRegister(BKP_DR1) != BKP_RTC_CFG_FLAG)			// ��ȡ���ݼĴ���ƫ����Ϊ4��λ�� - �������ݼĴ�������ȷ����û�б��ù�
  {
    uart_test("BKP init\r\n", strlen("BKP init\r\n"));
		RTC_Configuration();	                                      // ����RTC 

		CalTime.tm_year = 2010;
		CalTime.tm_mon  = 8;
		CalTime.tm_mday = 10;
		CalTime.tm_hour = 11;
		CalTime.tm_min  = 30;
		CalTime.tm_sec  = 0;
    Time_Adjust(CalTime);	
		                                      // ����ʱ���� -  ��ʼ��һ��Ĭ��ʱ�� 
		PWR_BackupAccessCmd(ENABLE);	        // ʹ�ܶԺ󱸼Ĵ�����RTC�ķ��� 
    BKP_WriteBackupRegister(BKP_DR1, BKP_RTC_CFG_FLAG);	        // д�������־
		PWR_BackupAccessCmd(DISABLE);	        // ʹ�ܶԺ󱸼Ĵ�����RTC�ķ��� 
  }
  else		                                                      // ���ݼĴ����Ѿ��������� 
  {
    RTC_WaitForSynchro();			                                  // �ȴ�RTC�Ĵ���ͬ�� 
    RTC_WaitForLastTask();		                                  // �ȴ��Ĵ���д������� 
  }
#endif

#if defined (STM32F4XX)
	RTC_InitTypeDef RTC_InitStruct;
	RTC_Configuration(&RTC_InitStruct);	                                      // ����RTC
	RTC_InitStruct.RTC_HourFormat = RTC_HourFormat_24;
	RTC_Init(&RTC_InitStruct);
	if(RTC_ReadBackupRegister(RTC_BKP_DR1) != BKP_RTC_CFG_FLAG)
	{
		RTC_InitTypeDef RTC_InitStruct;
		RTC_Configuration(&RTC_InitStruct);	                                      // ����RTC
		RTC_InitStruct.RTC_HourFormat = RTC_HourFormat_24;
		RTC_Init(&RTC_InitStruct);

		CalTime.tm_year = 2013;
		CalTime.tm_mon  = 9;
		CalTime.tm_mday = 12;
		CalTime.tm_hour = 20;
		CalTime.tm_min  = 20;
		CalTime.tm_sec  = 0;
		CalTime.tm_wday  = 0;
		Time_Adjust(CalTime);	
		RTC_WriteBackupRegister(RTC_BKP_DR1,BKP_RTC_CFG_FLAG);

	}
#endif
}

/*�ú���Ϊ�˼���207�Ĵ�����*/
ErrorStatus RTC_SetDateTime(struct tm * CalTime)
{
   Time_Adjust(*CalTime);
   return SUCCESS;
}
