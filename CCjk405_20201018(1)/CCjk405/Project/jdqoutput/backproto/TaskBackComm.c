/**
  ******************************************************************************
  * @file    TaskBackComm.c
  * @author  zqj
  * @version v1.0
  * @date    2017-03-09
  * @brief   
  ******************************************************************************
	*/
//#include "M_Global.h"
#include "Message.h"
#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ChgData.h"
#include "Che_Dian.h"
#include "TaskBackComm.h"
#include "Back_Inter.h"	
#include "main.h"
#include "wdg.h"
#include "rtc.h"

#define MINI_FRAME_LENGTH 12
#define CARDNO_NUM   16


BackRun_t  BackRun;
#ifdef AUNICE_DEBUG   //patli 20190930
void DebugSendCmd(uint8 cmd, uint8 *data, uint16 len)
{
	uint16 index = DataStartAddr;

	len %= 256;
	
	switch(cmd){
		case AUTO_LIFE:
			DebugInfoWithPbuf(BAC_MOD,(uint8*)"->CHG����", data, len,0);
				break;
		case AUTO_LOGIN:
   			DebugInfoWithPbuf(BAC_MOD,(uint8*)"->��¼", data, len,0);
				break;			
		case GET_DEVPARA:
			DebugInfoWithPbuf(BAC_MOD,(uint8*)"->��ȡ�ն�����", data, len,0);
				break;
		case SET_PARA:
			DebugInfoWithPbuf(BAC_MOD,(uint8*)"->���ò���", data, len,0);
				break;
		case AUTO_RECORD:
			len = len >= 255 ? 255:len;
			DebugInfoWithPbuf(BAC_MOD,(uint8*)"->�����ϴ�", data, len,0);
				break;
		case AUTO_ERR:
			DebugInfoWithPbuf(BAC_MOD,(uint8*)"->��ȡ�澯����", data, len,0);
				break;
		case AUTO_UPDATA:
			DebugInfoWithPbuf(BAC_MOD,(uint8*)"->�ϱ�ʵʱ����", data, len,0);
			break;
		case AUTO_CARDNO:
			DebugInfoWithPbuf(BAC_MOD,(uint8*)"->���Ϳ���Ϣ", data, len,0);
			break;			
//		case GET_RECORD:
//			DebugInfoWithBack((uint8*)"send GET_RECORD");
//			break;
		case GET_CHGDATA:			
//			DebugInfoByBack((uint8*)"->�ϴ�ʵʱ����");
			DebugInfoWithPbuf(BAC_MOD,(uint8*)"->�ϴ�ʵʱ����", data, len,0);	
			break;
		case SET_TIME:
			DebugInfoWithPbuf(BAC_MOD,(uint8*)"->��ʱ", data, len,0);	
			break;			
		case SET_ONOFF:
			DebugInfoWithPbuf(BAC_MOD,(uint8*)"->��̨��ͣ", data, len,0); 
			break;	
#ifdef MYIR_TCU_SUPPORT	
		case CHG_TO_TCU_START_CHARGE_RES:
			DebugInfoWithPbuf(BAC_MOD,(uint8*)"->CHG�������RES", &data[index], len-index-5,0); 
			break;	
		case CHG_TO_TCU_STOP_CHARGE_RES:
			DebugInfoWithPbuf(BAC_MOD,(uint8*)"->CHG���ֹͣRES", &data[index], len-index-5,0); 
			break;	
		case CHG_TO_TCU_START_CHARGED_REQ:
			DebugInfoWithPbuf(BAC_MOD,(uint8*)"->CHG����������REQ", &data[index], len-index-5,0); 
			break;	
		case CHG_TO_TCU_STOP_CHARGED_REQ:
			DebugInfoWithPbuf(BAC_MOD,(uint8*)"->CHG���ֹͣ���REQ", &data[index], len-index-5,0); 
			break;	
		case CHG_TO_TCU_RM_METER_20:
			DebugInfoWithPbuf(BAC_MOD,(uint8*)"->CHGң������20", &data[index], len-index-5,0); 
			break;	
		case CHG_TO_TCU_RM_SIGNAL_22:
			DebugInfoWithPbuf(BAC_MOD,(uint8*)"->CHGң������22", &data[index], len-index-5,0); 
			break;	
		case CHG_TO_TCU_RM_SIGNAL_23:
			DebugInfoWithPbuf(BAC_MOD,(uint8*)"->CHGң������23", &data[index], len-index-5,0); 
			break;	
		case CHG_TO_TCU_LOCK_RES:
			DebugInfoWithPbuf(BAC_MOD,(uint8*)"->CHG����������RES", &data[index], len-index-5,0); 
			break;	
		case CHG_TO_TCU_POWER_ADJUST_RES:
			DebugInfoWithPbuf(BAC_MOD,(uint8*)"->CHG���ʵ���RES", &data[index], len-index-5,0); 
			break;	
		case CHG_TO_TCU_PARAM_QUERY_RES:
			DebugInfoWithPbuf(BAC_MOD,(uint8*)"->CHG������ѯRES", &data[index], len-index-5,0); 
			break;	
		case CHG_TO_TCU_HB:
			DebugInfoWithPbuf(BAC_MOD,(uint8*)"->CHG����", &data[index], len-index-5,0); 
			break;	
#endif	

		default:
			DebugInfoWithPbuf(BAC_MOD,(uint8*)"->��Ч����", data, len,0);
			break;			
		}
}

#ifdef ETH_TEST_DEBUG
extern uint16 max_recv_hb_num;
#endif

void DebugRecvCmd(uint8 cmd, uint8 *data, uint16 len)
{

	uint16 index = DataStartAddr;

	len %= 256;

	switch(cmd){
		case AUTO_LIFE:
			DebugInfoWithPbuf(BAC_MOD,(uint8*)"<-����", data, len,0);
				break;
		case AUTO_LOGIN:
   			DebugInfoWithPbuf(BAC_MOD,(uint8*)"<-��¼", data, len,0);
				break;			
		case GET_DEVPARA:
			DebugInfoWithPbuf(BAC_MOD,(uint8*)"<-��ȡ�ն�����", data, len,0);
				break;
		case SET_PARA:
			DebugInfoWithPbuf(BAC_MOD,(uint8*)"<-���ò���", data, len,0);
				break;
		case AUTO_RECORD:
			DebugInfoWithPbuf(BAC_MOD,(uint8*)"<-�����ϴ�", data, len,0);
				break;
		case AUTO_ERR:
			DebugInfoWithPbuf(BAC_MOD,(uint8*)"<-��ȡ�澯����", data, len,0);
				break;
		case AUTO_UPDATA:
			DebugInfoWithPbuf(BAC_MOD,(uint8*)"<-�ϱ�ʵʱ����", data, len,0);
			break;
		case AUTO_CARDNO:
			DebugInfoWithPbuf(BAC_MOD,(uint8*)"<-���Ϳ���Ϣ", data, len,0);
			break;			
//		case GET_RECORD:
//			DebugInfoWithBack((uint8*)"recv GET_RECORD");
//			break;
		case SET_TIME:
			DebugInfoWithPbuf(BAC_MOD,(uint8*)"<-��ʱ", data, len,0);	
			break;			
		case SET_ONOFF:
			DebugInfoWithPbuf(BAC_MOD,(uint8*)"<-��̨��ͣ", data, len,0); 
			break;	
		case GET_CHGDATA:
//			DebugInfoByBack((uint8*)"<-�ϴ�ʵʱ����");
			DebugInfoWithPbuf(BAC_MOD,(uint8*)"<-�ϴ�ʵʱ����", data, len,0);	
			break;
#ifdef MYIR_TCU_SUPPORT
			
		case TCU_TO_CHG_START_CHARGE_REQ:
			DebugInfoWithPbuf(BAC_MOD,(uint8*)"<-TCU�������REQ", &data[index], len-index-5,0); 
			break;	
		case TCU_TO_CHG_STOP_CHARGE_REQ:
			DebugInfoWithPbuf(BAC_MOD,(uint8*)"<-TCU���ֹͣREQ", &data[index], len-index-5,0); 
			break;	
		case TCU_TO_CHG_START_CHARGED_RES:
			DebugInfoWithPbuf(BAC_MOD,(uint8*)"<-����������RES", &data[index], len-index-5,0); 
			break;	
		case TCU_TO_CHG_STOP_CHARGED_RES:
			DebugInfoWithPbuf(BAC_MOD,(uint8*)"<-���ֹͣ���RES", &data[index], len-index-5,0); 
			break;	
		case TCU_TO_CHG_RM_METER_21:
			DebugInfoWithPbuf(BAC_MOD,(uint8*)"<-TCUң������21", &data[index], len-index-5,0); 
			break;	
		case TCU_TO_CHG_LOCK_REQ:
			DebugInfoWithPbuf(BAC_MOD,(uint8*)"<-TCU����������REA", &data[index], len-index-5,0); 
			break;	
		case TCU_TO_CHG_POWER_ADJUST_REQ:
			DebugInfoWithPbuf(BAC_MOD,(uint8*)"<-TCU���ʵ���REQ", &data[index], len-index-5,0); 
			break;	
		case TCU_TO_CHG_PARAM_QUERY_REQ:
			DebugInfoWithPbuf(BAC_MOD,(uint8*)"<-TCU������ѯREQ", &data[index], len-index-5,0); 
			break;	
		case TCU_TO_CHG_HB:
			DebugInfoWithPbuf(BAC_MOD,(uint8*)"<-TCU����", &data[index], len-index-5,0); 
			break;	
		case TCU_TO_CHG_ERR:
			DebugInfoWithPbuf(BAC_MOD,(uint8*)"<-TCU ERR", &data[index], len-index-5,0); 
			break;	
#endif			
		default:
			DebugInfoWithPbuf(BAC_MOD,(uint8*)"<-��Ч����", &data[index], len-index-5,0);
			break;			
		}
}

#else
void DebugSendCmd(uint8 cmd, uint8 *data, uint16 len)
{

}

void DebugRecvCmd(uint8 cmd, uint8 *data, uint16 len)
{

}
#endif
/*********************************************MISC*********************/
void Get_BcdCardNo(uint8 *CardNo,uint8 *buff)
{
	uint8 i,j;
	uint8 tmp8;
	j = 0;
	memset(CardNo,0xAA,8);  /*����16λ����ȫ����A*/
	
	for( i = 0 ; i < CARDNO_NUM ; i++ ){
		if( buff[i] >= '0' && buff[i] <= '9' ) {
			 if( i % 2 == 1 ) {
				 CardNo[j] &= 0xF0;
				 CardNo[j] += buff[i]-0x30;
				 j++;
			 }
			 else {
				  tmp8 = buff[i]-0x30;
					CardNo[j] = ((tmp8 << 4) | 0x0A );
			 }
		}
		else {			  
         break;
		}			
	}
}

/*********������ˮ��**********************
buff: ���---��ˮ���ַ���
len: ����---��ˮ�ų��ȣ�Ĭ����32�ֽ�
*****************************************/
/* ʵ��itoa������Դ�� */ 
char *myitoa(int num,char *str,int radix) 
{  
	/* ������ */ 
	char index[]="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"; 
	unsigned unum; /* �м���� */ 
	int i=0,j,k; 
	/* ȷ��unum��ֵ */ 
	if(radix==10&&num<0) /* ʮ���Ƹ��� */ 
	{ 
		unum=(unsigned)-num; 
		str[i++]='-'; 
	} 
	else unum=(unsigned)num; /* ������� */ 
	/* ���� */ 
	do  
	{ 
		str[i++]=index[unum%(unsigned)radix]; 
		unum/=radix; 
	}while(unum); 
	str[i]='\0'; 
	/* ת�� */ 
	if(str[0]=='-') k=1; /* ʮ���Ƹ��� */ 
	else k=0; 
	/* ��ԭ���ġ�/2����Ϊ��/2.0������֤��num��16~255֮�䣬radix����16ʱ��Ҳ�ܵõ���ȷ��� */ 
	char temp; 
	for(j=k;j<=(i-k-1)/2.0;j++) 
	{ 
		temp=str[j]; 
		str[j]=str[i-j-1]; 
		str[i-j-1]=temp; 
	} 
	return str; 
}

void CreateSerilNo(uint8 *buff,uint8 len)
{

	uint32 ticks; 

	memset(buff,0,len);
	ticks = Time_GetUnixTime();
	
	myitoa(ticks, buff, 10);	

}

/**
��ȡ18�ֽڵ�ASCII�ϴ�����
��Ա90 00 00 00 00 1
�·���39 30 30 30 30 30 30 30 30 30 31 00 00 00 00 00 00 00 00 00
��������ʾ��90000000001
**/
static const char conCdNumber[] = {'0','1','2','3','4','5','6','7','8','9'};

uint8 UpCardNoASCII(uint8 *buff,uint8 *CardNo)
{
	uint8 i,j;
	j = 0;
	memset(buff,0,CARDNO_NUM);
	for( i = 0 ; i < 8; i++ ) {
		if( (CardNo[i] & 0xF0) != 0xA0 ) {
			buff[j++] = conCdNumber[ ((CardNo[i] >> 4) & 0x0F) % 10];
		}else {
			break;
		}
		
		if( (CardNo[i] & 0x0F) != 0x0A ) {
			buff[j++] = conCdNumber[ (CardNo[i] & 0x0F) % 10];
		}else {
			break;
		}
	}
	return 16;
}

///*********************
//����ʱ��:2019��
//type = 0,19��ǰ��20�ں� ����
//type = 1,20��ǰ��19�ں� ��֮��
//*********************/
///*********************
//����ʱ��:2019��
//type = 0,19��ǰ��20�ں� ����
//type = 1,20��ǰ��19�ں� ��֮��
//*********************/
uint8 updataTime(uint8 *puff ,uint8 tpye)
{
	struct tm time_now = { 0 };
	struct tm	tmlocal  = { 0 };
	
	CHARGE_TYPE  *BgRunDataA = ChgData_GetRunDataPtr(AGUN_NO);
	CHARGE_TYPE  *BgRunDataB = ChgData_GetRunDataPtr(BGUN_NO);
	
	if (BgRunDataA->logic->workstep != STEP_IDEL || BgRunDataB->logic->workstep != STEP_IDEL)//��������ã�������Ʒѽ�������
	{
			return FALSE;
	}
	
	tmlocal = Time_GetSystemCalendarTime();
	
	if(0 == tpye){
	  time_now.tm_year = Common_Bcd2hex8( puff[0]) + Common_Bcd2hex8( puff[1]) * 100;
	}
	else{
		time_now.tm_year = Common_Bcd2hex8( puff[1]) + Common_Bcd2hex8( puff[0]) * 100;
  }
	if(time_now.tm_year < 2000){
		return FALSE;
	}
	time_now.tm_mon  = Common_Bcd2hex8( puff[2]);
	if((time_now.tm_mon > 12)||((0 == time_now.tm_mon))){
		return FALSE;
	}
	time_now.tm_mday = Common_Bcd2hex8( puff[3]);
	if((time_now.tm_mday > 31)||((0 == time_now.tm_mday))){
		return FALSE;
	}
	time_now.tm_hour = Common_Bcd2hex8( puff[4]);
	if(time_now.tm_hour > 24){
		return FALSE;
	}
	time_now.tm_min  = Common_Bcd2hex8( puff[5]);
	if(time_now.tm_min >= 60){
		return FALSE;
	}
	time_now.tm_sec  = Common_Bcd2hex8( puff[6]);
	if(time_now.tm_sec >= 60){
		return FALSE;
	}
	
	if ( tmlocal.tm_year != time_now.tm_year \
		|| tmlocal.tm_mon  != time_now.tm_mon   \
	  || tmlocal.tm_mday != time_now.tm_mday  \
	  || tmlocal.tm_hour != time_now.tm_hour  \
	  || tmlocal.tm_min  != time_now.tm_min ) 
	{			
		  time_now.tm_wday = 0;     //�������оƬRTC���ܳ���
			
			RTC_SetDateTime(time_now);
		  Delay10Ms(5);
		     
		  Hmi_SetTime(&time_now);
		  
			return TRUE;
	}
	
	return FALSE;
}

////��ȡ����ʱ��STEP 
uint8 GetConnectType()
{	
	return ChgData_GetCommParaPtr()->conntype;
}


////��ȡ����ʱ��STEP 
uint8 GetWorkStep(uint8 gunNo)
{
	
	CHARGE_TYPE  *BgRunData = ChgData_GetRunDataPtr(gunNo);
	return BgRunData->logic->workstep;
}

////����״̬ 00H������ ; 01H������ ; 02H�������� ; 03H�������ͣ
uint8 GetWorkStatus(uint8 gunNo)
{
	CHARGE_TYPE  *BgRunData = ChgData_GetRunDataPtr(gunNo);
  uint8 rtn = 0;

  switch(BgRunData->logic->workstep){
		case  STEP_IDEL:      /*���н׶�*/
			rtn = 0;   //rtn = (CC1_4V == BgRunData->gun->statu.bits.cc1stu) ? 1 : 0;
			break;
	  case  STEP_START:
		case  STEP_LINK:
		case  STEP_BPOWER:
		case  STEP_SKHANDS:	
    case  STEP_ISOCHECK_SUB:			
	  case  STEP_ISOWAIT_SUB:
		case  STEP_ISOFINISH_SUB:		
		case  STEP_PARAMSET:
		case  STEP_OLD_GBT_SUB:			
		case  STEP_BRM_WAIT_SUB:			
	  case  STEP_BRM_SUB:
		case  STEP_BCPWAIT_SUB:
		case  STEP_BCP_SUB:			
	  case  STEP_BROWAIT_SUB:
		case  STEP_BRO_SUB:			
	  case  STEP_ADJUST_SUB:			
		case  STEP_CHARGEING:		
      rtn = 1;
     break;		
		case  STEP_CHGEND:
		case  STEP_WAITOVER:	
			//�Ѿ��ϴ���¼
			rtn = 2;	
			break;
    case  STEP_WAIT_SUB:	
      rtn = 2;
      break;		
		default:
			break;;
	}
  return rtn;	
}
	
/****************************************************************************/
BACK_CTRL_STRUCT g_BackCtrl;
NET_DATA_INTER NetHandler;/* �ײ������� */
Back_Agreement BackAgree;

void TskBack_Init(void)
{
	uint8 temp;
	temp = g_BackCtrl.AgreeType;
	memset(&g_BackCtrl,0,sizeof(g_BackCtrl));
	g_BackCtrl.CommStatus = CONNECT_OFF;
	TskBack_CommStatusSet(CONNECT_OFF);
	g_BackCtrl.AgreeType = temp;
}
NET_DATA_INTER * TskBack_getNetHandle()
{
	return &NetHandler;
}

int32 Back_Read(uint8 *buf,uint16 len)
{
	if(len <=BackRun.msg_len - BackRun.msg_readed_num)
	{
		memcpy(buf, &BackRun.ptr_msg[BackRun.msg_readed_num],len);
		BackRun.msg_readed_num += len;
	}else{

		memcpy(buf, &BackRun.ptr_msg[BackRun.msg_readed_num],BackRun.msg_len - BackRun.msg_readed_num);
		BackRun.msg_readed_num = BackRun.msg_len;
		return BackRun.msg_len - BackRun.msg_readed_num;
 	
		}
 
	return len;
}

/*ͨѶ 0:����   1���Ͽ�*/	
uint8 TskBack_CommStatus(void)
{
	return g_BackCtrl.u8ShowCommStatus;
}
uint8 TskBack_CommStatusSet(uint8 staus)
{
	g_BackCtrl.u8ShowCommStatus = staus;
	return staus;
}

/* ����Э�� */
void Init_AgreementFunc(Back_Agreement *pNet,uint8 Type)
{
	OS_CPU_SR  cpu_sr;
	
	OS_ENTER_CRITICAL();  
	
	switch(Type)
	{		
#ifdef CHE_DIAN_BACKCOMM
		case CONN_CheDian:
		{
			pNet->InitAgree = CheDian_Init;
			pNet->DealBackMsg = CheDian_PreBackMsg;
			pNet->RunAgree = CheDian_Run;
			break;
		}
#endif		
		
#ifdef AUNICE_BACK_COMM		
		case CONN_Aunice:
		{
			pNet->InitAgree = Proto_InitData;
			pNet->DealBackMsg = Proto_PreBackMsg;
			pNet->RunAgree = Proto_Run;
			break;
		}
#endif		
#ifdef CHARGE_BIRD_BACK_COMM		
		case CONN_CHGBIRD:
		{
			pNet->InitAgree = ChargeBird_Init;
			pNet->DealBackMsg = ChargeBird_PreBackMsg;
			pNet->RunAgree = ChargeBird_Run;
			break;
		}
#endif
		
#ifdef ZPLD_BACK_COMM		
		case CONN_CHGZPLD:
		{
			pNet->InitAgree = ChargeZPLD_Init;
			pNet->DealBackMsg = ChargeZPLD_PreBackMsg;
			pNet->RunAgree = ChargeZPLD_Run;
			break;
		}
#endif
#ifdef 	CP_BACK_COMM	
		case CONN_CHCP:
		{
			pNet->InitAgree = ChargeCP_Init;
			pNet->DealBackMsg = ChargeCP_PreBackMsg;
			pNet->RunAgree = ChargeCP_Run;
			break;
		}
#endif		
		default:
		{
			pNet->InitAgree = CheDian_Init;
			pNet->DealBackMsg = CheDian_PreBackMsg;
			pNet->RunAgree = CheDian_Run;
			break;
		}
		
	}
	
	OS_EXIT_CRITICAL();
}

/* ��ʼ����̨ͨ�ſ������� */
void InitTCPData(void)
{	
	PARAM_COMM_TYPE *BackCOMM = ChgData_GetCommParaPtr();
	//�ӿڳ�ʼ��
	NetHandler.Init(&BackCOMM->netpara);
	NetHandler.Reset();
}

/* ��ʼ����̨ͨ�ſ������� */
static void InitBackData(void)
{	
	PARAM_COMM_TYPE *BackCOMM = ChgData_GetCommParaPtr();


#ifndef AN_ETH_TASK
	//�ӿڳ�ʼ��
	Init_NetInterFunc(&NetHandler,BackCOMM->conntype);
	NetHandler.Init(&BackCOMM->netpara);
	NetHandler.Reset();
#endif	
	
	//Э��ѡ��
	g_BackCtrl.AgreeType = BackCOMM->agreetype;//CONN_CheDian;//CONN_Aunice;//CONN_CHGBIRD
	Init_AgreementFunc(&BackAgree,g_BackCtrl.AgreeType);
	BackAgree.InitAgree();	
}

/***********************************************************************
**����:					��̨ͨ������
**��ڲ���:			p_arg      
**����ֵ: 			��
*************************************************************************/
void Task_BackComm(void *p_arg)
{
	MSG_STRUCT Backmsg;
	
    TskBack_Init();
	Message_QueueCreat(APP_TASK_BACK_PRIO); 
	
	Delay10Ms(50);		
	
	InitBackData();
		
	while(1)
	{
		TaskRunTimePrint("Task_BackComm begin", OSPrioCur);

#if 1		
		if (RcvMsgFromQueue(&Backmsg) == TRUE)
		{
				BackAgree.DealBackMsg(&Backmsg);		
		}
		
		BackAgree.RunAgree();
#endif

		TaskRunTimePrint("Task_BackComm end", OSPrioCur);

		Delay10Ms(APP_TASK_BACK_DELAY);	//patli 20200106 Delay10Ms(2);		
		
  }	
	
}
