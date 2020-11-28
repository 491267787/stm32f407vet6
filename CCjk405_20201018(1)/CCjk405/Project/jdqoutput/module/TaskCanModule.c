
#include "main.h"
#include "can.h"
#include "Common.h"
#include "message.h"
#include "List.h"
#include "ChgData.h"
#include "TaskCanModule.h"

typedef enum
{
	NO_ERR   = 0x0,
	ADDR_ERR = 0x1,
	CMD_ERR  = 0x2,
	CHK_ERR  = 0x3,
}ERRTYPE;


extern uint8 gSetGrpFg,m_gModnum;
extern uint8 m_gModAddr,m_gModCmdListIndex;
extern COMM_MODLIST gModCommList;


extern int32 CanHander[];/* CAN��� */
/*��ʼ��CAN*/
void TskMod_InitCAN(void)
{
	CanHander[CDMOD_CAN] = Can_Open(CAN_2);
}

  
CMD_LIST gCmdList[30];
#define LIST_CMD_NUM (sizeof(gCmdList)/sizeof(CMD_LIST))

/***********�����б��ʼ��*****************************************/

/*����Ϊ���ģʽ*/
uint32 CdModel_SendData(uint8 objAdr,uint8 cmd,uint16 valtype,uint32 *u32data)
{
	 CAN_MSG Txmsg;
	 CDFRAME_HEAD  *frameId;
	 CDFRAME_DATA *frmdata;
	
	 uint8 cnt = 0,rtn;
	 while(cnt < 10){
     rtn = Can_Read(CanHander[CDMOD_CAN],&Txmsg,0) ;   //�����ܴ��ڵĲ�������
     if(0 == rtn){
         break;
     }
     cnt++;
   }
	 memset(&Txmsg,0,sizeof(Txmsg));
	 frameId = (CDFRAME_HEAD *)&Txmsg;
	 frameId->sbit.protno = CDPROTO_NO;   //0x60
	 frameId->sbit.ptp = 1;
	 frameId->sbit.dst = objAdr;
	 frameId->sbit.src = CD_HOSTID;       //0x01
	 frameId->sbit.cnt = 0;
	 frameId->sbit.res1 = 1;
	 frameId->sbit.res2 = 1;
	
	 /*������*/
	 frmdata = (CDFRAME_DATA*)&(Txmsg.Data);
	 frmdata->db.ntype.bits.groupAddress=0;
	 frmdata->db.ntype.bits.msgType = cmd ;                //������Ϣ����
	 frmdata->db.errType = NO_ERR;      
	 frmdata->db.vtype.us = Common_Change2Byte(&valtype);  //��ͬ�Ĺ������� 
	
	 frmdata->db.val.u32d = Common_Change4Byte(u32data);   //����
	 
	 Txmsg.FrmDef.FF = CAN_FRAME_EXTERNED;  							 // 1
   Txmsg.FrmDef.RTR = CAN_DATA_TYPE_FRAME;  					   // 0
	 Txmsg.DLC = 8;
	 Can_Write(CanHander[CDMOD_CAN],&Txmsg); 							 //дcan������	 
 
	 DebugInfoWithPbuf(MOD_MOD,(uint8*)"CdModel_SendData", Txmsg.Data, sizeof(Txmsg.Data),0);
	 
	 Delay10Ms(1);
	 
	 return 0;
}



/*У���ַ�Ƿ�Ϊ�Լ����յĵ�ַ*/
uint32  CdModel_CheckCanId(uint32 extId)
{
	  uint32 rtn = 1;
	  CDFRAME_HEAD *frmhead = (CDFRAME_HEAD *)&extId;
	  if( frmhead->sbit.dst != CD_HOSTID ) {
			 return 0;
	  }else if(frmhead->sbit.protno != CDPROTO_NO ) {
			 return 0;
		}else if(frmhead->sbit.cnt != 0 ) { /*ֻ����ģ�������֡*/
			 return 0;
		}else {
			 ;
		}
	  return rtn;
}


/*����Can����*/
uint16 CdModel_ParseFrame(uint8 srcAdr,uint8 *data)
{
	 
	uint16 rtnCmd = ERR_CMD;
	double fdata = 0.0;
	CDMOD_DATA *ptrModData = NULL;
	
	DEV_ACMETER_TYPE *pACMet = TskAc_GetMeterDataPtr(0);
	DEV_METER_TYPE *pDCMet = TskDc_GetMeterDataPtr(0);
	PARAM_OPER_TYPE *devparaPtr = ChgData_GetRunParamPtr();
	
	srcAdr %= CD_MAX_NUM + 1; 
	CDFRAME_DATA *da =  (CDFRAME_DATA *)data;

	 DebugInfoWithPbuf(MOD_MOD,(uint8*)"CdModel_ParseFrame", data, sizeof(da->dat),0);

	if( (da->db.ntype.bits.msgType == ACK_SETTINGDATA) \
	|| (da->db.ntype.bits.msgType == ACK_REQ_DATA ) ) 
	{
		ptrModData = CdModData_GetDataPtr(srcAdr-1);
		ptrModData->grpNo = da->db.ntype.bits.groupAddress;
		ptrModData->commErr = 0;
		ptrModData->lastticks = GetSystemTick();
		uint16 cmd = Common_Change2Byte((uint16 *)&da->db.vtype.us);
		
	 	DebugInfoWithPx(MOD_MOD,(uint8*)"CdModel_ParseFrame cmd", cmd);
	 
		switch(cmd) 
		{
		 case OUT_VOLTE:  //0x0001 �����ѹ
			 Common_Change4Byte((uint32 *)&(da->db.val.u32d));/*1024��*/
			 fdata = (da->db.val.u32d/1024.0)*10;
			 ptrModData->outVolte = fdata;
//			 pDCMet->volt = ptrModData->outVolte;
			 break;
		 case OUT_CURR:  //0x0002 �������
			 Common_Change4Byte((uint32 *)&(da->db.val.u32d));
			 fdata = (da->db.val.u32d/1024.00) * 10 + 0.5;
			 ptrModData->outCurr = fdata;
//		   pDCMet->current = ptrModData->outCurr +  CdModData_GetDataPtr(srcAdr%2)->outCurr;  //�泵Ĭ��Aǹ,�̶�����ģ��
		   pDCMet->commOvertimecnt = 0;
			 break;
		 case INPUT_AC_AVOLT:  //0x000C �����ѹAC_A
			 Common_Change4Byte((uint32 *)&(da->db.val.u32d));
			 fdata = (da->db.val.u32d/1024.0) * 10;
		   ptrModData->inVolteACA = fdata;
		   pACMet->Uab = ptrModData->inVolteACA * 1.73;
			 break;
		 case INPUT_AC_BVOLT:  //0x000D �����ѹAC_B
			 Common_Change4Byte((uint32 *)&(da->db.val.u32d));
			 fdata = (da->db.val.u32d/1024.0) * 10;
		   ptrModData->inVolteACB = fdata;
		   pACMet->Ubc = ptrModData->inVolteACB * 1.73;
			 break;
		 case INPUT_AC_CVOLT:  //0x000E �����ѹAC_C
			 Common_Change4Byte((uint32 *)&(da->db.val.u32d));
			 fdata = (da->db.val.u32d/1024.0) * 10;
		   ptrModData->inVolteACC = fdata;
		   pACMet->Uca = ptrModData->inVolteACC * 1.73;
			 break;
		 case INPUT_TEMPER:  //0x000B ����ڻ����¶�
			 Common_Change4Byte((uint32 *)&(da->db.val.u32d));
			 fdata = (da->db.val.u32d/1024.0) * 10;
			 ptrModData->temper = (uint32)fdata;
			 break;
		 case SETTING_LIMIT_POINT:  //0x0022 ģ��������
			 Common_Change4Byte((uint32 *)&(da->db.val.u32d));  
			 fdata = (da->db.val.u32d/1024.0 )* 10;
			 ptrModData->limtPoint = (uint32)fdata;
			 break;
		case GROUP_NUMBER:  //0x00BD���ö�ȡģ����� 01000000
			 ptrModData->grpNo = (da->db.val.u32d)>>24;
			 break;
		case ALARM_WORDS:  //0x0040 ��ȡ��ǰ�澯״̬
			  Common_Change4Byte((uint32 *)&(da->db.val.u32d));
			  ptrModData->alarmInfo.u32d = da->db.val.u32d;
			  ptrModData->onoff = (~ptrModData->alarmInfo.bits.ModStu)&0x01;
			 break;
		case SETTING_MOD_VOLTE:  //0x0021 ����ģ���ѹ
			 Common_Change4Byte((uint32 *)&(da->db.val.u32d));
			 fdata = (da->db.val.u32d/1024.0)*10 ;
			 ptrModData->outVolte = (uint32)fdata;
			 break;
		case START_STOP_OPER:  //0x0030 ����ģ�鿪�ػ�
			 Common_Change4Byte((uint32 *)&(da->db.val.u32d));
			 fdata = da->db.val.u32d ;
			 if(fdata){
				 ptrModData->alarmInfo.bits.ModWork = 1;
				 ptrModData->onoff = 1;
			 }
			 else{
				 ptrModData->alarmInfo.bits.ModWork = 0;
				 ptrModData->onoff = 0;
			 }
			 break;		
		 default:			 
			 rtnCmd = ERR_CMD;
			 break; 
		}
		rtnCmd = cmd;
	}
	return rtnCmd;
}



/*
 ����ģ������ objAdr = 0, Ϊ�㲥��ַ
 groupAddress = 0 ,Ϊȫ�ֹ㲥
 groupAddress ��= 0 ���㲥������
*/
uint32 CdModel_SetData(uint8 objAdr,uint8 groupAddress,uint8 ptp,uint16 cmd,uint16 valtype,DATA4_TYPE *data)
{
	 CAN_MSG Txmsg;
	 CDFRAME_HEAD  *frameId;
	 CDFRAME_DATA *frmdata;
	 DATA4_TYPE b4temp;
	
   //�����ܴ��ڵĲ�������
	 Can_ClearReadBuff(CanHander[CDMOD_CAN]);
	
	 memcpy(&b4temp,data,sizeof(DATA4_TYPE));
	 memset(&Txmsg,0,sizeof(Txmsg));
	 
	 ptp = ( 0x00 == objAdr ) ? 0 : 1;
	
	 frameId = (CDFRAME_HEAD *)&Txmsg;
	 frameId->sbit.protno = CDPROTO_NO;   //0x60
	 frameId->sbit.ptp = ptp;
	 frameId->sbit.dst = objAdr;
	 frameId->sbit.src = CD_HOSTID;     //0xF0
	 frameId->sbit.cnt = 0;
	 frameId->sbit.res1 = 1;
	 frameId->sbit.res2 = 1;
	 
	 if(valtype == 0x0021 || valtype == 0x0022) {
		 valtype= valtype;
	 }
	
	 /*������*/
	 frmdata = (CDFRAME_DATA*)Txmsg.Data;
	 frmdata->db.ntype.bits.groupAddress = groupAddress;
	 frmdata->db.ntype.bits.msgType = cmd ;  //������Ϣ����
	 frmdata->db.errType = NO_ERR;      //0x00
	 frmdata->db.vtype.us = Common_Change2Byte(&valtype);    //��ͬ�Ĺ������� 
	  
	 Common_Change4Byte((uint32 *)&b4temp);    //����
	 memcpy(frmdata->db.val.uc,&b4temp,sizeof(DATA4_TYPE));
	 
	 
	 Txmsg.FrmDef.FF = CAN_FRAME_EXTERNED;  // 1
	 Txmsg.FrmDef.RTR = CAN_DATA_TYPE_FRAME;  // 0
	 Txmsg.DLC = 8;
	 
	 Can_Write(CanHander[CDMOD_CAN],&Txmsg); //дcan������

	 DebugInfoWithPbuf(MOD_MOD,(uint8*)"CdModel_SetData", Txmsg.Data, sizeof(Txmsg.Data),2);

#ifdef AUNICE_DEBUG  //patli 20190930
	 Delay10Ms(5);

#else
	 Delay10Ms(2);
#endif
	 
	 return 0;
}


void CanModule_SetGroup(uint8 adr,uint8 grpNo)
{
	uint32 data = grpNo;
	CdModel_SetData(adr,grpNo,1,SETTING_DATA,GROUP_NUMBER,(DATA4_TYPE *)&data);

}


/********************************************************************
���ճ��ģ�������

*********************************************************************/
uint8 TskMod_ReadData(uint8 localAdr,uint8 dstAdr,uint16 *nCmd)
{
	uint16 cmd;
	uint8 rtn;
	uint16 cnt,tmp;
	uint8 addr = 0;
	CAN_MSG Canmsg;
	
	CDFRAME_HEAD *frmhead = (CDFRAME_HEAD *)&(Canmsg.Id.ExtId);
	memset(&Canmsg,0,sizeof(CAN_MSG));
	 
	tmp = 0;
  rtn = 0;
	cnt = 0;
	cmd = ERR_CMD;
	do{ 
		if(Can_Read(CanHander[CDMOD_CAN],&Canmsg,0) > 0) {
			if(CdModel_CheckCanId(Canmsg.Id.ExtId) > 0) {
				if( (localAdr == frmhead->sbit.dst) && (dstAdr == frmhead->sbit.src)) {
					  addr = frmhead->sbit.src;
					  cmd = CdModel_ParseFrame(addr,(uint8 *)Canmsg.Data);
					  if( cmd > ERR_CMD ) {
						  tmp++;
					}
				}
			}
		}else {
			if( tmp > 0 ) {
				*nCmd = cmd;
				break;
			}
			Delay10Ms(2); 
		}
    cnt++;			
	}while(cnt < 5);
	
	rtn = tmp > 0 ? 1 : 0;
	return rtn;
}





/********************************************************************
gCmdList���е������    
*********************************************************************/
uint8 TskMod_ExecuCmd(void)
{
	
	 uint8 rtn = 0;
   uint16 rCmd;

	 CMD_LIST * tempCmd;
	 struct list *node = CdMod_GetHead();
	 while(node)
	 {
		 tempCmd = (CMD_LIST *)node->cmdst;

		 CdModel_SetData(tempCmd->dstAddr,tempCmd->groupAddress,0,tempCmd->cmd,tempCmd->vtype,(void *)&(tempCmd->value));
		 Delay10Ms(2);
     	if((tempCmd->dstAddr != 0 )&& TskMod_ReadData(CD_HOSTID,tempCmd->dstAddr,&rCmd))  {
			 if ( rCmd == tempCmd->vtype ) {
				 tempCmd->succ = 1;
			 }
		 }else {
				 tempCmd->exec++;
		 }
		 Delay10Ms(2);
		 if((tempCmd->exec > CMD_SEND_TIMES) || (1 ==  tempCmd->succ)) {
			  CdMod_deleteCmd(node->id);
			  tempCmd->succ = 0;
			  tempCmd->exec = 0;
		 }
		 node = node->next;
		 rtn = tempCmd->vtype;
	 }
	 return rtn;
}


const uint16 conGatherCdMod[] = {ALARM_WORDS,OUT_VOLTE,OUT_CURR,MOD_LIMIT_POIN,INPUT_TEMPER,INPUT_AC_AVOLT,INPUT_AC_BVOLT,INPUT_AC_CVOLT};
#define CAN_CMDTYPE_LIST_NUM  (sizeof(conGatherCdMod)/sizeof(uint16)) 


uint8 TskMod_GatherCdModData(uint8 objAdr,uint8 cmdtypeindex)
{
	uint8 rtn;
	uint16 cnt,tmp;
	uint8 addr = 0;
	CAN_MSG Canmsg;
	uint32 temp = 0;
	
	//�����ܴ��ڵĲ�������
	Can_ClearReadBuff(CanHander[CDMOD_CAN]);
	
	CdModel_SendData(objAdr,REQ_BYTEDATA,conGatherCdMod[cmdtypeindex],&temp);
	Delay10Ms(2); 
	
  CDFRAME_HEAD *frmhead = (CDFRAME_HEAD *)&(Canmsg.Id.ExtId);
	memset(&Canmsg,0,sizeof(CAN_MSG));
	 
	tmp = 0;
  rtn = 0;
	cnt = 0;
	do{ 
		if(Can_Read(CanHander[CDMOD_CAN],&Canmsg,0) > 0) {
			if(CdModel_CheckCanId(Canmsg.Id.ExtId) > 0) {
				if( objAdr == frmhead->sbit.src ) {
			      addr = frmhead->sbit.src;
					  if( CdModel_ParseFrame(addr,(uint8 *)Canmsg.Data)) {
						  tmp++;
					   }
				 }
			 }
		}else {
			if( tmp > 0 ) {
				break;
			}
			Delay10Ms(1); 
		}
		cnt++;			
	}while(cnt < 5);
	
	rtn = tmp > 0 ? 1 : 0;
	
	return rtn;
}

static const uint8 conModAdr[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};
#ifdef CAN_CDMOD
void Task_CdModule(void *p_arg)
{
    uint8 rtn = 0, MODGatherCnt = 0;
	  MSG_STRUCT msg;
    
		Delay10Ms(300);  //20201115addzyf
	  TskMod_InitCAN();

	  m_gModAddr = 0;
	  m_gModCmdListIndex = 0;
	  Message_QueueCreat(APP_TASK_DLMOD_PRIO);

	  PARAM_DEV_TYPE *ptrDevParam = ChgData_GetDevParamPtr();
	  PARAM_OPER_TYPE *ptrRunParam = ChgData_GetRunParamPtr();
	
	  gSetGrpFg = 0;
	  m_gModnum = ptrRunParam->modnum;
	
	  CdModData_Init(AGUN_NO);
//	  CdModData_Init(BGUN_NO);
    
		while(1)
		{			 		
				TaskRunTimePrint("Task_CdModule begin", OSPrioCur);
				
				if (RcvMsgFromQueue(&msg) == TRUE)
				{
					CdModData_MsgDeal(&msg);
				}
				
			  /*��ִ�п�������*/
				rtn = TskMod_ExecuCmd();

			  
			  if( rtn == ALARM_WORDS ) {
					m_gModCmdListIndex = 0;
				}else if( rtn == SETTING_MOD_VOLTE ) {
					m_gModCmdListIndex = 1;
				}				
				
				
				//�Զ�ȡģ��ĵ�ѹ��������Ϊ����ֵ�������߶�ȡ��������Ϣ������
				if(MODGatherCnt%2)
				{
					TskMod_GatherCdModData(conModAdr[m_gModAddr],OUT_VOLTE);
				}
				else
				{
					TskMod_GatherCdModData(conModAdr[m_gModAddr],OUT_CURR);
				}
							
				/*��ѯ�ɼ�����*/
			  MODGatherCnt++;
				if (0 == MODGatherCnt % 4)
				{	
					MODGatherCnt = 0;
					
					TskMod_GatherCdModData(conModAdr[m_gModAddr],m_gModCmdListIndex);
					
					m_gModAddr++;
				}
				
			  if( m_gModAddr == m_gModnum ) {
					
					gSetGrpFg = 0;
					m_gModAddr = 0;
					m_gModCmdListIndex++;
					m_gModCmdListIndex %= CAN_CMDTYPE_LIST_NUM;
					
				}
				

				CdModData_CheckCommErr(AGUN_NO);

				TaskRunTimePrint("Task_CdModule end", OSPrioCur);

				Delay10Ms(1);//APP_TASK_DLMOD_DELAY);  //patli 20200106 Delay5Ms(2);		����̫���̫����̫��������Ϣ���ղ���ȫ


    }
}

#endif



