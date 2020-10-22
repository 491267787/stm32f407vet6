/**
  ******************************************************************************
  * @file    PowerSplitt.c
  * @author  
  * @version v1.0
  * @date    2018-08-03
  * @brief   
  ******************************************************************************
*/
#include "PowerSplitt.h"
#include "ChgData.h"

#include "Common.h"
#include "TaskRealCheck.h"
#include "Adebug.h"



extern MOD_CTRL_PARA gModGpPara[MAX_MOD_GROUP];
/* 2��ģ������Ӵ���*/
extern uint8 RelayOut_DcConTactKmOut(uint8 onoff);



uint8 PowerSplitt_ParamInit(uint8 grpNo)
{
	  uint8 i,modnum,offset;
	  PARAM_OPER_TYPE *ptrRunParam = ChgData_GetRunParamPtr();
//	  /*****************************************************************/
//    /*���Դ���*/
//	   ptrRunParam->grpmodnum[0] = 2;
//	   ptrRunParam->grpmodnum[1] = 2;
//	   ptrRunParam->modnum = 4;
//	  /*****************************************************************/

	  if( AGUN_NO == grpNo ) {
			offset = 0;
		}else if( BGUN_NO == grpNo ) {
			offset = ptrRunParam->grpmodnum[AGUN_NO];
		}
		modnum =  ptrRunParam->grpmodnum[grpNo];	
	  memset(&gModGpPara[grpNo],0,sizeof(MOD_CTRL_PARA));
		/*������ģ������ = ����ģ��������һ��*/
		gModGpPara[grpNo].u8HwGpModNum = modnum;
		gModGpPara[grpNo].u8Num = gModGpPara[grpNo].u8HwGpModNum;
		gModGpPara[grpNo].u16HwTolCurr = ptrRunParam->singmodcurr * gModGpPara[grpNo].u8HwGpModNum;
		
		gModGpPara[grpNo].u8HwGpAddr[0] = 1 + offset;
		gModGpPara[grpNo].u8HwGpAddr[1] = 2 + offset;
		gModGpPara[grpNo].u8HwGpAddr[2] = 3 + offset;
		gModGpPara[grpNo].u8HwGpAddr[3] = 4 + offset;
		
		gModGpPara[grpNo].u8HwGpAddr[4] = 1 + offset;
		gModGpPara[grpNo].u8HwGpAddr[5] = 2 + offset;
		gModGpPara[grpNo].u8HwGpAddr[6] = 3 + offset;
		gModGpPara[grpNo].u8HwGpAddr[7] = 4 + offset;
		
	  gModGpPara[grpNo].u8GpNo = grpNo+1; /*��ţ� 1��2*/
		
		for( i = 0 ; i < MAX_GPMOD_NUM ; i++ ) {
			gModGpPara[grpNo].u8Addr[i] = 0xff;
		}
		
	  return 0;
}


uint8 PowerSplitt_CalcSplitModNum(uint16 needCurr,MOD_CTRL_PARA *ptrModPara)
{
	 uint16 u16ModB;
	 uint16 u16NeedModNum;
	 PARAM_OPER_TYPE *ptrRunPara = ChgData_GetRunParamPtr();
	 MOD_CTRL_PARA *ptrOtherModGpPara = CdModData_GetModGpPara((ptrModPara->u8GpNo)%MAX_MOD_GROUP);
	
	 u16NeedModNum = 0;
	 if( needCurr > ptrModPara->u16HwTolCurr + 50 ) {
			u16NeedModNum =  (needCurr - ptrModPara->u16HwTolCurr )/ ptrRunPara->singmodcurr;
			u16ModB = needCurr % ptrRunPara->singmodcurr;
			if( u16ModB > 50 ) {
				u16NeedModNum += 1;
			}
			u16NeedModNum = (u16NeedModNum <= ptrOtherModGpPara->u8HwGpModNum ) ? u16NeedModNum : ptrOtherModGpPara->u8HwGpModNum;
	 }
		 
	 return (uint8)u16NeedModNum;
}

uint8 PwoerSplitt_CalcStepCurrent(MOD_CTRL_PARA *ptrModPara)
{
	  uint8 rtn = 0;
	  uint16 u16ModCurr = 200 / ptrModPara->u8Num ;
	  if( GetSystemTick() > ptrModPara->u32StepTicks +TIM_NS(1)) {
				 ptrModPara->u32StepTicks = GetSystemTick();
				 if( ptrModPara->u16StepCurr < ptrModPara->u16Curr)
				 {
						ptrModPara->u16StepCurr += u16ModCurr;
						ptrModPara->u16StepCurr = ptrModPara->u16StepCurr > ptrModPara->u16Curr \
				                          ? ptrModPara->u16Curr : ptrModPara->u16StepCurr;
				 }else{
					  if(ptrModPara->u16StepCurr > ptrModPara->u16Curr + u16ModCurr ) {
							 ptrModPara->u16StepCurr -= u16ModCurr;
						}else {
							ptrModPara->u16StepCurr = ptrModPara->u16Curr;
						}
				}
				rtn = 1;
		 }
	  return rtn;
}





/* �ú���ֻ�����յ�BCL����� */
uint8 PowerSplitt_CalcPower(uint8 grpNo,MOD_CTRL_PARA *ptrModPara)
{
	 static uint16 sU16RmainTicks = 0;
	 uint8 gunNo = grpNo > 0 ? grpNo-1 : 0;
	 PARAM_OPER_TYPE *ptrRunPara = ChgData_GetRunParamPtr();
	 BMSDATA_ST *ptrBms = (BMSDATA_ST *)Bms_GetBmsCarDataPtr(gunNo) ;
	
	 /*ȡ��һ��ĵ����� *********************************************************/
	 DEV_METER_TYPE *ptrOtherMet = TskDc_GetMeterDataPtr((gunNo+1)%MAX_MOD_GROUP);
	 DEV_GUN_TYPE *ptrOtherGun = Check_GetGunDataPtr((gunNo+1)%MAX_MOD_GROUP);
	 DEV_LOGIC_TYPE *ptrOtherLog = ChgData_GetLogicDataPtr((gunNo+1)% MAX_MOD_GROUP);
	 MOD_CTRL_PARA *ptrOtherModGpPara = CdModData_GetModGpPara((gunNo+1)%MAX_MOD_GROUP);
	 /*************************************************************************/

//	 DebugInfoWithPi(MOD_MOD,"PowerSplitt_CalcPower u16Allowed", ptrModPara->u16Allowed);

	 if( 1 == ptrModPara->u16Allowed ) { 
		 return 0;
	 }
	 
	 ptrModPara->u16Curr = (ptrBms->bcl.needcurr) / ptrModPara->u8Num;
	 ptrModPara->u16Curr = (ptrModPara->u16Curr <= ptrRunPara->singmodcurr) ? \
	                            ptrModPara->u16Curr : ptrRunPara->singmodcurr;
	 
	 ptrModPara->u16Volt = ptrBms->bcl.needvolt;

	 if( PwoerSplitt_CalcStepCurrent(ptrModPara) ) { /*2S һ��*/
		 
			ptrModPara->u16Allowed = 1; 
			ptrModPara->u8Step = ESET_INDI_VOLCURR;
	 }
	 
//	 if( 1 == ptrOtherLog->gunInUsed ) {
//			/*����һǹ����ʱ����ǹ��ͣ����*/
//		  return 0 ; 
//	 }

//	 DebugInfoWithPi(MOD_MOD,"PowerSplitt_CalcPower Sysparalarm.powersplitt", ptrRunPara->Sysparalarm.bits.powersplitt);

	 /*����Ϊ�����ʷ���*/
	 if( 0 == ptrRunPara->Sysparalarm.bits.powersplitt ) {
		  /*�Ͽ�K3�Ӵ���*/
	    RelayOut_DcConTactKmOut(PW_CTRL_OFF);
		  ptrModPara->u8SplitNum = 0;
	    ptrModPara->u8SlipCnt = 0;
		  ptrModPara->u8Num = ptrModPara->u8HwGpModNum;
		  return 0;
	 }
	 
//	 DebugInfoWithPi(MOD_MOD,"PowerSplitt_CalcPower u8CommStu", ptrOtherModGpPara->u8CommStu);
	 /*����ģ����ͨѶ������*/
	 if( 0 == ptrOtherModGpPara->u8CommStu ) {
		  return 0 ; 
	 }

//	 DebugInfoWithPi(MOD_MOD,"PowerSplitt_CalcPower ptrOtherLog->workstep", ptrOtherLog->workstep);	 	 
	 /*����һ��ǹ�Ƿ����*/
	 if( (ptrOtherLog->workstep != STEP_IDEL) && (ptrOtherLog->workstep != STEP_WAITOVER))  {
		 ptrModPara->u32WaitTicks = GetSystemTick();
		 ptrModPara->u8SlipCnt = 0;
		 return 0;
	 }
	 
//	 DebugInfoWithPi(MOD_MOD,"PowerSplitt_CalcPower statu.bits.kmnstu", ptrOtherGun->statu.bits.kmnstu);	 	 	 
//	 DebugInfoWithPi(MOD_MOD,"PowerSplitt_CalcPower statu.bits.kmpstu", ptrOtherGun->statu.bits.kmpstu);	 	 	 
	 if( 1 == ptrOtherGun->statu.bits.kmnstu  || 1 == ptrOtherGun->statu.bits.kmpstu ) {
		 ptrModPara->u32WaitTicks = GetSystemTick();
		 ptrModPara->u8SlipCnt = 0;
		 return 0;
	 }

	 
//	 DebugInfoWithPi(MOD_MOD,"PowerSplitt_CalcPower current", ptrOtherMet->current);	 	 	 
	 if( ptrOtherMet->current > 50 ) {
		 ptrModPara->u32WaitTicks = GetSystemTick();
		 ptrModPara->u8SlipCnt = 0;
		 return 0;
	 }
	 
	 if( ptrModPara->u8SplitNum > 0 && ptrBms->bcl.needcurr < ptrModPara->u16HwTolCurr + 10 ) {
		 if(GetSystemTick() > ptrModPara->u32ExitTicks + TIM_NMIN(3)) {
			ptrModPara->u32ExitTicks = GetSystemTick();
			ptrModPara->u32WaitTicks = GetSystemTick();
			/*2��ģ���˳�*/
			sU16RmainTicks = 0;
			ptrModPara->u16Allowed = 1;
			ptrModPara->u8SlipCnt = 0;
		  ptrModPara->u8Step = ESET_INDI_EXIT;
		  
//		  DebugInfo(MOD_MOD,"2��ģ���˳�");			  
		  
		 return 2;
	 }
		 
		 
	 }
	 
//	 if(GetSystemTick() - ptrModPara->u32ExitTicks > TIM_NMIN(3))  {
//			ptrModPara->u32ExitTicks = GetSystemTick();
//			ptrModPara->u32WaitTicks = GetSystemTick();
//			/*2��ģ���˳�*/
//			ptrModPara->u16Allowed = 1;
//			ptrModPara->u8SlipCnt = 0;
//		  ptrModPara->u8Step = ESET_INDI_EXIT;
//		 return 2;
//	 }
	 
	 
	 if( GetSystemTick() < ptrModPara->u32WaitTicks+ TIM_NMIN(1)+sU16RmainTicks \
    && 	ptrBms->bcs.lasttimes > 5 ) { 
		 return 0;
	 }
	 Delay10Ms(100);
	 ptrModPara->u8SlipCnt++;
	 ptrModPara->u32WaitTicks = GetSystemTick();
	 /*��������ģ������*/
	 ptrModPara->u8SplitNum = PowerSplitt_CalcSplitModNum(ptrBms->bcl.needcurr,ptrModPara);
	 ptrModPara->u8Num = (ptrModPara->u8HwGpModNum + ptrModPara->u8SplitNum)% (MAX_GPMOD_NUM+1);
	 if( ptrModPara->u8SplitNum > 0 ) {
		 sU16RmainTicks = TIM_NMIN(1);
	 }
	 else{
		 sU16RmainTicks = 0;
	 }
	 ptrModPara->u16Curr = (ptrBms->bcl.needcurr) / ptrModPara->u8Num;
	 ptrModPara->u16Volt = ptrBms->bcl.needvolt;
	 PwoerSplitt_CalcStepCurrent(ptrModPara) ;  /*2S һ��*/
	 
	 DEV_METER_TYPE *pDcMet = TskDc_GetMeterDataPtr(gunNo);
	  
	 /* ���븺�غ�pDcMet->current��Ϊ0���и��غ�ʼ�������� */
//	 if( pDcMet->current < (ptrBms->bcl.needcurr+50 )  ) {
//		 ptrModPara->u16StepCurr = pDcMet->current / ptrModPara->u8Num;
//	 }else {
//		 ptrModPara->u16StepCurr = ( ptrBms->bcl.needcurr  ) / ptrModPara->u8Num ;
//	 }
	 
	 if (pDcMet->current > ptrBms->bcl.needcurr) 
	 {
		 ptrModPara->u16StepCurr = ( ptrBms->bcl.needcurr  ) / ptrModPara->u8Num ;
	 }
	 
	 ptrModPara->u16StepCurr = (ptrModPara->u16StepCurr <= ptrRunPara->singmodcurr) ? \
	                            ptrModPara->u16StepCurr : ptrRunPara->singmodcurr;
	 ptrModPara->u16Allowed = 1; 
	 ptrModPara->u8Step = ESET_CON_GROUP;
	 
	 return 3;
}

/* �ú���ֻ�����յ�BCL����� */
uint8 PowerSplitt_ModCtrlFunc(uint8 grpNo,MOD_CTRL_PARA *ptrModPara)
{
	 uint8  u8I;
	 uint32 u32TempVolt;
	 uint32 u32TempCurr;
	 uint8 gunNo = grpNo > 0 ? grpNo-1 : 0;
	 BMSDATA_ST *ptrBms = (BMSDATA_ST *)Bms_GetBmsCarDataPtr(gunNo) ;
	 DEV_LOGIC_TYPE *ptrLog = ChgData_GetLogicDataPtr((gunNo)% MAX_MOD_GROUP);
	 MOD_CTRL_PARA *ptrOtherModGpPara = CdModData_GetModGpPara((gunNo+1)%MAX_MOD_GROUP);
	
	 u32TempVolt = ptrModPara->u16Volt;
   u32TempVolt *= COFF_DATA;
	
	 u32TempCurr = ptrModPara->u16StepCurr ;
	 u32TempCurr *= COFF_DATA;
	 
//	 DebugInfoWithPi(MOD_MOD,"PowerSplitt_ModCtrlFunc0 u16Allowed", ptrModPara->u16Allowed);			 
//	 DebugInfoWithPi(MOD_MOD,"PowerSplitt_ModCtrlFunc0 u8Step", ptrModPara->u8Step);			 

	 if( 1 == ptrModPara->u16Allowed ) { 
		 switch(ptrModPara->u8Step ) 
		 { 
			 case ESET_CON_GROUP: /*���÷���*/
				 ptrLog->gunInUsed = 1;
				 for( u8I = 0; u8I < ptrOtherModGpPara->u8HwGpModNum; u8I++ ) 
				 {
					 if( u8I < ptrModPara->u8SplitNum ) {
						 CdModData_SetGroupNo(ptrOtherModGpPara->u8HwGpAddr[u8I],ptrModPara->u8GpNo);  //CdModData_SetGropCmd
					 }else {
						 CdModData_SetSingleCmd(ptrOtherModGpPara->u8HwGpAddr[u8I],MOD_ONOFF,MOD_CMD_OFF);
						 CdModData_SetGroupNo(ptrOtherModGpPara->u8HwGpAddr[u8I],ptrOtherModGpPara->u8GpNo);  //CdModData_SetGropCmd
					 }
				 }
				ptrModPara->u8Step = ESET_CON_VOLCURR; 
			break;
			case ESET_CON_VOLCURR:/*���õ���*/
					CdModData_SetGroupCmd(ptrModPara->u8GpNo,MOD_SETCURR,u32TempCurr);
					CdModData_SetGroupCmd(ptrModPara->u8GpNo,MOD_SETVOLT,u32TempVolt);
			    ptrModPara->u8Step = ESET_CON_ONOFF; 
			 break;
			case ESET_CON_ONOFF:  /*ģ�鿪��*/
				if( ptrModPara->u8SplitNum > 0 ) {
						for( u8I = 0; u8I < ptrModPara->u8SplitNum ; u8I++) {
							CdModData_SetSingleCmd(ptrOtherModGpPara->u8HwGpAddr[u8I],MOD_ONOFF,MOD_CMD_ON);
						}
						ptrModPara->u8Step = ESET_CON_END; 
			  }else {
					ptrModPara->u8Step = ESET_EXIT_RELAY; 
				}
				Delay10Ms(10);
				break;
			case ESET_CON_END:
				/*�պ�K3�Ӵ���*/
				RelayOut_DcConTactKmOut(PW_CTRL_ON);
			  ptrModPara->u8Step = 0;
			  ptrModPara->u16Allowed = 0;
			  ptrLog->gunInUsed = 0;
				break;
			case ESET_INDI_VOLCURR: /*����������ѹ������*/
				CdModData_SetGroupCmd(ptrModPara->u8GpNo,MOD_SETCURR,u32TempCurr);
				CdModData_SetGroupCmd(ptrModPara->u8GpNo,MOD_SETVOLT,u32TempVolt);
			  ptrModPara->u16Allowed = 0;
				break;
			case ESET_INDI_EXIT: /*2��ģ�������˳� ,С�����˳�ģʽ*/
				
				for( u8I = 0; u8I < ptrOtherModGpPara->u8HwGpModNum; u8I++ ) 
				 {
					  CdModData_SetSingleCmd(ptrOtherModGpPara->u8HwGpAddr[u8I],MOD_ONOFF,MOD_CMD_OFF);
						CdModData_SetGroupNo(ptrOtherModGpPara->u8HwGpAddr[u8I],ptrOtherModGpPara->u8GpNo);  //CdModData_SetGropCmd
				 }
				 ptrModPara->u8Step = ESET_EXIT_RELAY; 
				 Delay10Ms(10);
				 break;
			case ESET_EXIT_RELAY:
				/*�Ͽ�K3�Ӵ���*/
			  Delay10Ms(10);
	      RelayOut_DcConTactKmOut(PW_CTRL_OFF);
			
			  ptrOtherModGpPara->u8Num = ptrOtherModGpPara->u8HwGpModNum;
			  ptrModPara->u8Num = ptrModPara->u8HwGpModNum;
			
			  ptrModPara->u16Allowed = 0;
			  ptrModPara->u8Step = ESET_OTHER_DEFAULT; 
				break;
//			case ESET_PREEM_EXIT: /*��ռʽ�˳�*/
//				ptrLog->gunInUsed = 1;
//				for( u8I = 0; u8I < ptrModPara->u8HwGpModNum; u8I++ ) 
//				 {
//					  CdModData_SetSingleCmd(ptrModPara->u8HwGpAddr[u8I],MOD_ONOFF,MOD_CMD_OFF);
//						CdModData_SetGroupNo(ptrModPara->u8HwGpAddr[u8I],ptrModPara->u8GpNo);  //CdModData_SetGropCmd
//				 }
//				 ptrModPara->u8Step = ESET_PREEM_RELAY; 
//				
//				break;
//			case ESET_PREEM_RELAY:
//				/*�Ͽ�K3�Ӵ���*/
//	      RelayOut_DcConTactKmOut(PW_CTRL_OFF);
//			  ptrModPara->u8Num = ptrModPara->u8HwGpModNum;
//			  ptrModPara->u16Allowed = 0;
//			  ptrModPara->u8Step = ESET_OTHER_DEFAULT; 
//			  ptrLog->gunInUsed = 0;
//				break;
			default:
				
				break;
			}
	 }
	 return ptrModPara->u8Step;
}

/*��ű����1�鿪ʼ��1��2��*/
uint8 PowerSplitt_PreemExitGroup(uint8 grpNo,MOD_CTRL_PARA *ptrModPara)
{
	  uint8 u8I;
	  uint8 gunNo = grpNo ;
		DEV_LOGIC_TYPE *ptrLog = ChgData_GetLogicDataPtr((gunNo)% MAX_MOD_GROUP);
	  DEV_LOGIC_TYPE *ptrOtherLog = ChgData_GetLogicDataPtr((gunNo+1)% MAX_MOD_GROUP);
		MOD_CTRL_PARA *ptrOtherModGpPara = CdModData_GetModGpPara((gunNo+1)%MAX_MOD_GROUP);
	  
	  /*ע�⣬��һǹ��������BCL�� ptrLog->gunInUsed = 0 */
		ptrLog->gunInUsed = 1;
		for( u8I = 0; u8I < ptrModPara->u8HwGpModNum; u8I++ ) 
		{
			CdModData_SetSingleCmd(ptrModPara->u8HwGpAddr[u8I],MOD_ONOFF,MOD_CMD_OFF);
			CdModData_SetGroupNo(ptrModPara->u8HwGpAddr[u8I],ptrModPara->u8GpNo);  //CdModData_SetGropCmd
		}
		if( STEP_IDEL == ptrOtherLog->workstep ) {
			for( u8I = 0; u8I < ptrOtherModGpPara->u8HwGpModNum; u8I++ ) 
			{
				CdModData_SetGroupNo(ptrOtherModGpPara->u8HwGpAddr[u8I],ptrOtherModGpPara->u8GpNo); 
			}
		}
		
		ptrModPara->u8Num = ptrModPara->u8HwGpModNum; 
		ptrModPara->u8SlipCnt = 0;
		
		ptrOtherModGpPara->u8Num = ptrOtherModGpPara->u8HwGpModNum; 
		ptrOtherModGpPara->u8SlipCnt = 0;
		 
		return 0;
}





