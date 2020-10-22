/**
  ******************************************************************************
  * @file    Task_Screen.c
  * @author  zqj
  * @version v1.0
  * @date    2016-03-08
  * @brief   
  ******************************************************************************
	*/
//#include "M_Global.h"
#include "Hmi_Api.h"
#include "Uart.h"
#include "Che_Dian.h"
#include "TaskBackComm.h"

#include "Common.h"
#include "message.h"
#include "app_cfg.h"
#include "ChgData.h"
#include "main.h"
#include "RecordFat.h"
#include "gpio.h"

#include <string.h>
#include <stddef.h>
#include <stdio.h>
#include "rtc.h"

#define Frame_Head1  0x75
#define Frame_Head2  0x72
#define CMD_NULL     0xff
#define CARDNO_NUM   16

#ifdef CHE_DIAN_BACKCOMM

extern uint8 Rd_FindUnAccountRecord(CALCBILL_DATA_ST bill[]);
static const uint8 conCtrlPrio[] = {APP_TASK_AGUNMAINCTRL_PRIO,APP_TASK_BGUNMAINCTRL_PRIO};
static const uint8 conSoure[] = {CLIENTA,CLIENTB};

//BackRun_t  BackRun;
extern BackRun_t  BackRun;

void CheDian_LifeAuto(uint8 gunNo,uint8 dir);
void CheDian_LoginAuto(uint8 gunNo,uint8 dir);
void CheDian_QuitAuto(uint8 gunNo,uint8 dir);
void CheDian_GetDevPara(uint8 gunNo,uint8 dir);
void CheDian_GetChgData(uint8 gunNo,uint8 dir);
void CheDian_SetPara(uint8 gunNo,uint8 dir);
void CheDian_RecordAuto(uint8 gunNo,uint8 dir);
void CheDian_AckOnOffSend(uint8 gunNo,uint8 onoff );
void CheDian_UpDataAuto(uint8 gunNo,uint8 dir);
void CheDian_SetTime(uint8 gunNo,uint8 dir);
void CheDian_SetOnOff(uint8 gunNo,uint8 dir);
void CheDian_CardnoAuto(uint8 gunNo,uint8 dir);
void CheDin_FrameSend(uint8 type,uint8 cmd,uint8 soure,uint16 index);


const CheDian_t conCheDian[] = {                 //������ͨѶ��Ϣ����
	
	{AUTO_LIFE     , CheDian_LifeAuto},     /*����*/		
	{AUTO_LOGIN    , CheDian_LoginAuto},    /*����*/		
	{AUTO_QUIT     , CheDian_QuitAuto},     /*�˳�����*/	
	{GET_DEVPARA   , CheDian_GetDevPara},   /*��ȡ�ն�����*/
	{GET_CHGDATA   , CheDian_GetChgData },  /*��ȡʵʱ����*/	 //GetUpData_CheDian
	{SET_PARA      , CheDian_SetPara},      /*���ò���*/	
	{AUTO_RECORD   , CheDian_RecordAuto},   /*������ʷ��¼*/ 	   
	{AUTO_UPDATA   , CheDian_UpDataAuto},   /*����ǹ״̬����*/
	{SET_TIME      , CheDian_SetTime},      /*��ʱ*/
	{SET_ONOFF     , CheDian_SetOnOff},     /*��ͣ����*/ 
	{AUTO_CARDNO   , CheDian_CardnoAuto},   /*���Ϳ���Ϣ*/
	{CMD_NULL      , NULL},   /*��Ч*/
};

BackRun_t *GetBackRun(void)
{
	return &BackRun;
}

/*���׮��ַ*/
uint32 Get_chargeId(void)
{
	PARAM_DEV_TYPE *BgDevParam =ChgData_GetDevParamPtr();
	
	return Common_Ascii2Uint32(BgDevParam->chargeId,sizeof(BgDevParam->chargeId));	
}


/***********************************************************************
** ��������: CheDian_PackgCheck()
** ����������У���պ�̨֡�ĺϷ���
** ����:     
**����ֵ��   
*************************************************************************/
uint8  CheDian_PackgCheck(uint8 *buf,uint16 len)
{
	uint32 temp, temp1;
	 
 	if( len > (CheDian_MAX-8) ) {
		return 1; //֡��
	}
	 
	if( buf[len - 5] != 0x68) {
		return 2;//������ʶ��
	}
	 
	if( (buf[9]&0xf0) != 0x00 ) {
		return 3;//��Դ  
	}
	 
	temp = Common_LittleToInt(&buf[4]);
	if( temp != Get_chargeId()) {
		return 4;	//׮��ַ
	}
  
  temp = Common_CalclongSum(buf , (len - 4));
	temp1 = Common_LittleToInt(&buf[len - 4]);
	if( temp != temp1) {
		return 5;	//У��
	}
	 
	return 0;
}
 
/****************************
 ��ȡ��̨�·�����
 ****************************/
uint8  CheDian_RcvUploadDataDeal(uint8 *CtrData)
{
	static uint8 sCmd9cnt[DEF_MAX_GUN_NO] = {0};
	uint8 cnt,stat,rtn,gunNo;
	uint16 readlen,num,dlen;	
	uint16 u16TmpLen = 0;
	uint8 *ptr = NULL;
	int32 getlen;
  cnt = 0;
	stat = 0;
	num = 0;
	dlen = 0;
	getlen = 0;
	readlen = 1;   //ÿ�ζ�һ������
	rtn = 0;
	ptr = BackRun.Data;
	BackRun.Len = 0;
	memset(BackRun.Data,0,CheDian_MAX);
	
	do {
		if(1 == cnt)
		{
			rtn = 0;
		}
#ifdef AN_ETH_TASK
		getlen = Back_Read(ptr,readlen);

#else
		getlen = NetHandler.ReadData(ptr,readlen);
#endif
		if( getlen > 0 ) {
			 cnt = 0;

			 switch(stat)
			{
				 case 0:
				 {
					  if( Frame_Head1 == *ptr ) {
							 ptr++ ;
							 stat = 1;
						}
						break;
				 }
				 case 1:
				 {
					   if( Frame_Head2 == *ptr ) {
							 ptr++;
							 stat = 2;
						}else {
							stat = 0;
							ptr = BackRun.Data;//BackRun.Data;
						}
						break;
				 }
				 case 2:
				 {
					   stat = 3;
					   ptr++;
             break;
				 }
				 case 3:
				 {
					   u16TmpLen = (BackRun.Data[3] << 8)+ BackRun.Data[2] + 2;//(BackRun.Data[3]<<8) + BackRun.Data[2] + 2;//2��ͷ�ֽ�
					   if((u16TmpLen > 255) ||(u16TmpLen < 10)){
							 stat = 0;
							 ptr =  BackRun.Data;//BackRun.Data;
						 }
						 else{
					     readlen =  u16TmpLen -4;//255-4  �´ζ�ȡ����
					     dlen = 4;
					     stat = 4;
					     ptr++;
						 }
             break;
				 }
				 case 4:
				 {
					  if( getlen == readlen ) {
							 ptr += getlen;
							 dlen += getlen;
							 if( dlen == u16TmpLen ) {
									rtn = 1;
									break;
							 }
							
						}else if (getlen < readlen  ){
							  readlen = readlen - getlen;
							  dlen += getlen;
							  ptr += getlen;
						}
					 break; 
				 }
				 default:
					 break;
			}
		}
		else {
			if(0 == stat){
				 return FALSE;
			}
			Delay5Ms(2);
			cnt++; 
		}
		
		if( 1 == rtn ) {
			 break;
		}
  }while((cnt < 2 ) && (ptr <  &BackRun.Data[CheDian_MAX]));//&BackRun.Data[120])); /*���մ���120�ֽ��˳�*/
	
	
  	num = (uint8)(ptr - BackRun.Data);  //&BackRun.Data[0]);
	
	if( (0 == rtn ) || num < CheDian_FrameOther ) {
		   return FALSE;
	  }

	/*У�鱨��*/
	if(( CheDian_PackgCheck(BackRun.Data,u16TmpLen)) > 0 ) {
		 memset(BackRun.Data,0,CheDian_MAX);
		 return FALSE;
	}
	ptr = BackRun.Data;
//	memcpy(BackRun.Data,buf,u16TmpLen);
	CtrData[0] = BackRun.Data[8];        //cmd
	CtrData[1] = BackRun.Data[9]&0x0f;   //ǹ��

	DebugRecvCmd(CtrData[0],BackRun.Data, u16TmpLen);

	if(CtrData[1]>0){
		CtrData[1] = (CtrData[1] - 1)% DEF_MAX_GUN_NO;
	}
	
	PARAM_OPER_TYPE *devparaPtr = ChgData_GetRunParamPtr();
	
	if (GUNNUM_2 == devparaPtr->gunnum)
	{
		gunNo = CtrData[1];
		
		if(CtrData[0] == GET_CHGDATA)
		{
			if(sCmd9cnt[gunNo] > 0)        //��һǹ��������2��ʵʱ���ݣ��ķ�����ǹ����
			{
				sCmd9cnt[gunNo] = 0;
				gunNo = (gunNo + 1)% DEF_MAX_GUN_NO;
				sCmd9cnt[gunNo] = 1;
				CtrData[1] = gunNo;
			}
			else
			{
				sCmd9cnt[gunNo]++;
				gunNo = (gunNo + 1)% DEF_MAX_GUN_NO;
				sCmd9cnt[gunNo] = 0;
			}
		}	
	}
	
	BackRun.Len = u16TmpLen;
	return TRUE;
}


/***********************************************
����ͨѶ����֡����
soure     ֡��Դ��ǹ��  ClientFrame, AutoFrame
type      ��������
gunNo       ǹ��
len   �������������ǰ��11������
***********************************************/
void CheDin_FrameSend(uint8 type,uint8 cmd,uint8 soure,uint16 len)
{
	uint32  CalclongSum;
	uint8 u8tmp, index = 0;
//STX		0X75 (K)  0X72(H)
	BackRun.Data[index++]  = Frame_Head1;
	BackRun.Data[index++]  = Frame_Head2;   
//֡��,��ȥ2����ʼ����
	BackRun.Len = CheDian_FrameOther + len - DataStartAddr;
	Common_LittleShortTo(&BackRun.Data[index],(BackRun.Len - 2));  //����֡���Ȳ���
	index += 2;
//�ն˺�	
	Common_LittleIntTo(&BackRun.Data[index],Get_chargeId());
  index += 4;  
//������		
	BackRun.Data[index++]  = cmd;
//֡��Դ��ǹ��	
	PARAM_OPER_TYPE *BgRunParam = ChgData_GetRunParamPtr();
	if(GUNNUM_2 == BgRunParam->gunnum){//˫ǹ
		BackRun.Data[index++]  = soure; 
	}
	else{
		BackRun.Data[index++]  = CLIENTO; 
	}
//��������	
	BackRun.Data[index++] = type;	
//������	
	BackRun.Data[BackRun.Len - 5] = 0x68;      
//У��ͣ�У����ǰ�����������
	CalclongSum = Common_CalclongSum(BackRun.Data , (BackRun.Len - 4));
	Common_LittleIntTo(&BackRun.Data[BackRun.Len - 4],CalclongSum);  
//�������ݺ���

#ifdef AN_ETH_TASK	
	SendMsgToTcpClient(MSG_DATA_TO_TCPCLIENT,BackRun.Len,BackRun.Data);  
#else
  if(NetHandler.WriteData(BackRun.Data, BackRun.Len) != BackRun.Len){
		NetHandler.WriteData(BackRun.Data, BackRun.Len);
  }
#endif  
	u8tmp = BackRun.Len > 100 ? 100 : BackRun.Len;

	DebugSendCmd(cmd, BackRun.Data, BackRun.Len);
	
	Delay10Ms((u8tmp)/50 + 3);		
	
}


/********************************
10ʱ�η���ת48ʱ�η�������
puff�洢��ʽ
10������
10����ʼʱ�䣺Сʱ��ʱ��ֿ�
��ֻ����A�˿ڷ���
*********************************/
uint16 ServerSetFeiLv(uint8 gunNo,uint8 *puff)           //д����
{
	uint8 i, j,unlen, stopNo, Next = 0, priceNo, lastpriceNo = 0, BMsgData[2];
	uint16 index = DataStartAddr, tmp1, tmp2, GetIndex ,GetIndexSum;
  
	PARAM_FEE_TYPE BgParam;
	PARAM_FEE_TYPE *BFeiLvPtr = &BgParam;
	PARAM_FEE_TYPE *SysDevParam = ChgData_GetFeeRate(gunNo);
	
	memcpy(BFeiLvPtr,SysDevParam,sizeof(PARAM_FEE_TYPE));
	
	SetFeiLv_t BackFeiLv[10];
	uint32 sBackSrvrate = 0,sBackBespkrate = 0,sBackParkrate = 0;
	uint8  sFeiLvNum = 0;
	
	memset(BackFeiLv,0,10*sizeof(SetFeiLv_t));
	 
	//���ݵ�Ԫ����
	GetIndexSum = Common_LittleToShort(&puff[index]);
	index +=2;
	
	//��ȡ��̨���õ�ʱ�������
	for(i = 0; i < GetIndexSum; i++)
	{  
		Delay5Ms(1);			
		GetIndex = Common_LittleToShort(&puff[index]);
		index +=3;//�����ֽ�+1
		
		if(GetIndex == 0x0014){  //ͣ���ѵ�Ԫ
				sBackParkrate = Common_LittleToShort(&puff[index]);
				index +=2;
				continue;
		}
		
		if((GetIndex >= 0x0015)&&(GetIndex <= 0x001E)){  //���ʵ�Ԫ
				BackFeiLv[GetIndex - 0x0015].FeiLv = Common_LittleToShort(&puff[index]);			  
				index +=2;
				sFeiLvNum++;
				continue;
		}
	
		if((GetIndex >= 0x001F)&&(GetIndex <= 0x0028)){   //��ʼʱ�䵥Ԫ
				BackFeiLv[GetIndex - 0x001F].startH = puff[index++];
				BackFeiLv[GetIndex - 0x001F].startM = puff[index++];
				
				if(0x001F != GetIndex){						
					BackFeiLv[GetIndex -1- 0x001F].stopH = BackFeiLv[GetIndex - 0x001F].startH;
					BackFeiLv[GetIndex -1- 0x001F].stopM = BackFeiLv[GetIndex - 0x001F].startM;
					tmp1 = BackFeiLv[GetIndex -1- 0x001F].stopH * 60 + BackFeiLv[GetIndex -1- 0x001F].stopM;
					tmp2 = BackFeiLv[GetIndex -1- 0x001F].startH * 60 + BackFeiLv[GetIndex -1- 0x001F].startM;
					if(tmp1 < tmp2) //��ʼʱ�䲻��С�ڽ���ʱ��
					{
						return GetIndex;
					}
				}
				continue;
		}
		
		if(GetIndex == 0x0029){  //����ѵ�Ԫ				  
				sBackSrvrate = Common_LittleToInt(&puff[index]);
				for(j=0;j<10;j++){									
					BFeiLvPtr->SectTimeFee.srvrate[j] = sBackSrvrate  * (BILL_PRICE_BASE/100);
				}
				index +=4;
				continue;
		}	
		if(GetIndex == 0x002d){  //����ѵ�Ԫ	5��С��	
				unlen = Common_LittleToInt(&puff[index]);
				index +=4;
				for(j=0;j<unlen;j++){									
					BackFeiLv[j].FeiLv = Common_LittleToInt(&puff[index])/(1000);
					index +=4;
				}
				for(j=0;j<unlen;j++){									
					sBackSrvrate = Common_LittleToInt(&puff[index]);
					BFeiLvPtr->SectTimeFee.srvrate[j] = sBackSrvrate  / (100000/BILL_PRICE_BASE);
					index +=4;
				}
				BFeiLvPtr->SectTimeFee.serivetype = SRV_ENERGY_TYPE;
				continue;
		}	
		index +=puff[index - 1];//û�õ��ĵ�Ԫ����				
	}
	
	Delay5Ms(1);
	BackFeiLv[sFeiLvNum - 1].stopH = 24;
	BackFeiLv[sFeiLvNum - 1].stopM = 0;
	
	//ת����48ʱ�μƷ� tmp1=10ʱ�ο�ʼʱ�䣺��   tmp2=10ʱ�ν���ʱ�䣺��  Next��48ʱ�κ�
	tmp1 = 0;  //
	for(i = 0; i < sFeiLvNum; i++)
	{		 
		 priceNo = 0xff;
		 //������������ǰ��ʱ���Ƿ���ͬ
		 for(j=0;j < lastpriceNo;j++){    
			 if(BackFeiLv[i].FeiLv == BFeiLvPtr->SectTimeFee.price[j]){
				 priceNo = j;
			 }
		 }
		 Delay5Ms(1);
		 //���һ���·���
		 if(0xff == priceNo){				 
			 priceNo = lastpriceNo;
//				 if(lastpriceNo>7){    modify0701zyf
			 if(lastpriceNo>=10){
				 return lastpriceNo;
			 }
			 BFeiLvPtr->SectTimeFee.price[priceNo] = BackFeiLv[i].FeiLv;
			 lastpriceNo++;				 
		 }
		 
		 if(BackFeiLv[i].stopM < 15){
			 stopNo = 0;
		 }
		 else if(BackFeiLv[i].stopM > 45){
			 stopNo = 2;
		 }
		 else{
			 stopNo = 1;
		 }
		 stopNo += BackFeiLv[i].stopH * 2;
		 BFeiLvPtr->SectTimeFee.cdsectNo[i] = Next;			 //�����̨�����ڴ�������Ӧ��ʼʱ��  //�����̨�����ڴ�������Ӧ��ʼʱ��
		 for(j=Next;j < stopNo;j++){
			 BFeiLvPtr->SectTimeFee.sectNo[Next]  = priceNo;
			 Next++;
		 }			 
		 if(Next>47){
			 break;
		 }
	}
	
	for(i=0;i<10;i++){
		BFeiLvPtr->SectTimeFee.price[i] = BFeiLvPtr->SectTimeFee.price[i] * (BILL_PRICE_BASE/100);
//			BFeiLvPtr->SectTimeFee.srvrate[i] = sBackSrvrate  * (BILL_PRICE_BASE/100);
		BFeiLvPtr->SectTimeFee.bespkrate[i] = sBackBespkrate  * (BILL_PRICE_BASE/100);
		BFeiLvPtr->SectTimeFee.parkrate[i] = sBackParkrate  * (BILL_PRICE_BASE/100);
	}

	if(memcmp(BFeiLvPtr,SysDevParam,sizeof(PARAM_FEE_TYPE)) != 0){
			memcpy(SysDevParam,BFeiLvPtr,sizeof(PARAM_FEE_TYPE));
			memcpy(ChgData_GetFeeRate(BGUN_NO),BFeiLvPtr,sizeof(PARAM_FEE_TYPE));

			BMsgData[0] = SAVE_FEE_TYPE;//
			BMsgData[1] = gunNo;
			SendMsgWithNByte(MSG_PARAM_STORE,2,&BMsgData[0],conCtrlPrio[gunNo]);
	}
	
	return 0;
}

/********************************
48ʱ�η���ת10ʱ�η���
puff�洢��ʽ
10������
10����ʼʱ�䣺Сʱ��ʱ��ֿ�
*********************************/
uint8 ServerReadFeiLv(uint8 gunNo,uint8 *puff)           //������
{
	uint32 temp = 0;
	uint8 i, Next =0, addr, sectNo;
	
	PARAM_FEE_TYPE *BFeiLvPtr = ChgData_GetFeeRate(AGUN_NO);
	
	for(i = 0; i < 48; i++)
	{
		  if(47 != i){				
			  if(BFeiLvPtr->SectTimeFee.sectNo[i] == BFeiLvPtr->SectTimeFee.sectNo[i+1]){					
			    if(46 == i){
					  i = 47;
				  }
				  else {
					  continue;
				  }
		    }
			}
			Next++;			
			if(Next > 10){    //�ϱ�ʱ��ֻ��10��
				return Next;
			}
			
			addr = (Next-1)<<1;
			sectNo = BFeiLvPtr->SectTimeFee.sectNo[i];
			temp = BFeiLvPtr->SectTimeFee.price[sectNo] / (BILL_PRICE_BASE/100);
			Common_LittleShortTo(&puff[addr],temp);//price��32λ��ֵ��ȷ��
			addr += 20;
			puff[addr]   = i / 2;
			puff[addr+1] = (i % 2) * 30;				
	}

	for(i=Next;i<10;i++){  //ʱ��24��00���ʣ����ʶ�	
			addr = (i<<1) + 20;
			puff[addr]   = 24;
			puff[addr+1] = 0;						
	}
	return 1;
}

/*************************
����
**************************/
void CheDian_LifeAuto(uint8 gunNo,uint8 dir)
{	
	uint16 index = DataStartAddr;
	
	if(SendEN == dir){           //����֡����
		BackRun.RcvbeatCnt++;
		BackRun.Data[index++] = CheDian_LifeTime;	//�������
		//�豸�����˶���ʱ��
		Common_LittleIntTo(&BackRun.Data[index],DevRunSec);
		index +=4;
		
		CheDin_FrameSend(AutoFrame, AUTO_LIFE, CLIENTO, index);
		BackRun.LifeTime = GetSystemTick();
	}
	else if(RcvFlag == dir){    //����֡����
    BackRun.RcvbeatCnt = 0;
  }
}

/**************************
��������
***************************/
void CheDian_LoginAuto(uint8 gunNo,uint8 dir)
{
	uint16 index = DataStartAddr;
	uint8 i;
	
	if(SendEN == dir){           //����֡����
		struct tm time_now;
		//����ʱ��
		time_now = Time_GetSystemCalendarTime();		
		BackRun.Data[index++] = Common_Hex2bcd8(time_now.tm_year/100);	//��
		BackRun.Data[index++] = Common_Hex2bcd8(time_now.tm_year%100);
		BackRun.Data[index++] = Common_Hex2bcd8(time_now.tm_mon);
		BackRun.Data[index++] = Common_Hex2bcd8(time_now.tm_mday);
		BackRun.Data[index++] = Common_Hex2bcd8(time_now.tm_hour);
		BackRun.Data[index++] = Common_Hex2bcd8(time_now.tm_min);
		BackRun.Data[index++] = Common_Hex2bcd8(time_now.tm_sec);
		//�豸����
		PARAM_OPER_TYPE *BgRunParam = ChgData_GetRunParamPtr();
		if(GUNNUM_2 == BgRunParam->gunnum){//˫ǹ
			BackRun.Data[index++] = DEV_TYPE_D;
		}
		else{
			BackRun.Data[index++] = DEV_TYPE_S;
		}
		//Ԥ����Ϣ
		BackRun.Data[index++] = 0x00;
		BackRun.Data[index++] = 0x00;
		BackRun.Data[index++] = 0x00;
		BackRun.Data[index++] = 0x00;
		//׮�汾��		
		BackRun.Data[index++] = 0x00;
		BackRun.Data[index++] = 0x00;
		BackRun.Data[index++] = 0x00;
		BackRun.Data[index++] = 0x00;
    //������Ϣ	
		memset(&BackRun.Data[index], 0, 16);
		index += 16;
	
		CheDin_FrameSend(AutoFrame, AUTO_LOGIN, CLIENTO, index);
	}
	else if(RcvFlag == dir){    //����֡����
    BackRun.LifeTime   = 0;
		BackRun.CardnoTime = GetSystemTick();		
		BackRun.ErrTime    = GetSystemTick();
		BackRun.RecordTime = 0;
		
		if(0 == BackRun.Data[DataStartAddr]){
			BackRun.Commforbidden = 0x00;        //����
		}
		for(i=0;i<DEF_MAX_GUN_NO;i++){
			CheDian_UpDataAuto(i,SendEN);
	//		BackRun.UpDataTime[i] = GetSystemTick();
		}
		
  }
}

/******************************************
�����˳�����
****************************************/
void CheDian_QuitAuto(uint8 gunNo,uint8 dir)
{
	uint16 index = DataStartAddr;
	
	if(SendEN == dir){           //����֡����
		struct tm time_now;
		//����ʱ��
		time_now = Time_GetSystemCalendarTime();		
		BackRun.Data[index++] = Common_Hex2bcd8(time_now.tm_year/100);	//��
		BackRun.Data[index++] = Common_Hex2bcd8(time_now.tm_year%100);
		BackRun.Data[index++] = Common_Hex2bcd8(time_now.tm_mon);
		BackRun.Data[index++] = Common_Hex2bcd8(time_now.tm_mday);
		BackRun.Data[index++] = Common_Hex2bcd8(time_now.tm_hour);
		BackRun.Data[index++] = Common_Hex2bcd8(time_now.tm_min);
		BackRun.Data[index++] = Common_Hex2bcd8(time_now.tm_sec);
		
		CheDin_FrameSend(RequestFrame, AUTO_QUIT, CLIENTO, index);
	}
	else if(RcvFlag == dir){    //����֡����
		
    if(0 == BackRun.Data[DataStartAddr]){
			BackRun.Commforbidden = 0xAA;        //�����˳�
		}
		CheDian_UpDataAuto(gunNo,SendEN);
  }
}

/***********************
������ȡ�ն�����
***********************/
void CheDian_GetDevPara(uint8 gunNo,uint8 dir)
{	
	DEV_ACMETER_TYPE *BgAcMete = TskAc_GetMeterDataPtr(0);
	PARAM_DEV_TYPE *BgDevParam = ChgData_GetDevParamPtr();
	PARAM_OPER_TYPE *BgRunParam = ChgData_GetRunParamPtr();
	PARAM_FEE_TYPE *BFeiLvPtr = ChgData_GetFeeRate(0);         //TEST
	
	int16 sendunit = 0;
	uint16 index = DataStartAddr , GetIndex = 0;
	uint8 Getpuff[100],GetFeiLv[40], addr, GetIndexSum = 0, i;
	
	if(gunNo >= DEF_MAX_GUN_NO)return;
	if(RcvFlag == dir){    //����֡����
		    
		GetIndexSum = Common_LittleToShort(&BackRun.Data[index]);//���ݵ�Ԫ����
		if((GetIndexSum > 50)||(0 == GetIndexSum)){
			return;
		}
		index +=2;
		
		ServerReadFeiLv(gunNo,GetFeiLv); //48ʱ��ת��10ʱ��
		
		memcpy(Getpuff,&BackRun.Data[index],(GetIndexSum * 2));
		memset(&BackRun.Data[0],0,CheDian_MAX);
		for(i = 0;i < GetIndexSum;i ++){
			GetIndex = Common_LittleToShort(&Getpuff[i<<1]);
			Common_LittleShortTo(&BackRun.Data[index],GetIndex);
			index +=2;
			switch(GetIndex){
				case 0x0001:   //׮����
					BackRun.Data[index++] = 1;
				  
					if(GUNNUM_2 == BgRunParam->gunnum){//˫ǹ
						BackRun.Data[index++] = DEV_TYPE_D;
					}
					else{
						BackRun.Data[index++] = DEV_TYPE_S;
					}
					break;
				case 0x0002:   //׮���
					BackRun.Data[index++] = 4;
				  Common_LittleIntTo(&BackRun.Data[index],Get_chargeId());  
          index += 4;	
					break;
				case 0x0003:    //IP
					BackRun.Data[index++] = 4;
				  memcpy(&BackRun.Data[index],&BgDevParam->bkcomm.netpara.LocalIpAddr,4);
          index += 4;	
					break;
				case 0x0004:    //gw ����
					BackRun.Data[index++] = 4;
				  memcpy(&BackRun.Data[index],&BgDevParam->bkcomm.netpara.NetGate,4);  
          index += 4;	
					break;
				case 0x0005:    //Mask ��������
					BackRun.Data[index++] = 4;
				  memcpy(&BackRun.Data[index],&BgDevParam->bkcomm.netpara.NetMask,4);
          index += 4;	
					break;
				case 0x0006:    //����˿ں�
					BackRun.Data[index++] = 2;
				  Common_LittleShortTo(&BackRun.Data[index],BgDevParam->bkcomm.netpara.RemotePort);
          index += 2;	
					break;
				case 0x0007:    //׮������LW
					BackRun.Data[index++] = 8;
				  Common_LittleShortTo(&BackRun.Data[index],Get_chargeId());  
          index += 8;	
					break;
				case 0x0008:    //���Ʒ�ʽ��1����Զ�̣�0�ر�
					BackRun.Data[index++] = 1;
				  BackRun.Data[index++] = 1; 
					break;
				case 0x0009:    //���㷽ʽ��1Զ�̣�0����
					BackRun.Data[index++] = 1;
				  BackRun.Data[index++] = 1; 
					break;
				case 0x000A:    //������ʽ��0�ֶΣ�1ͳһ���
					BackRun.Data[index++] = 1;
				  BackRun.Data[index++] = 0; 
					break;
				case 0x000B:    //׮����ߵ�ѹ
					BackRun.Data[index++] = 4;
				  Common_LittleShortTo(&BackRun.Data[index],BgRunParam->maxvolt);
          index += 4;
					break;
				case 0x000C:    //׮����ߵ���
					BackRun.Data[index++] = 4;
				  Common_LittleShortTo(&BackRun.Data[index], BgRunParam->maxcurr);
          index += 4;
					break;
				case 0x000D:    //׮����͵�ѹ
					BackRun.Data[index++] = 4;
				  Common_LittleShortTo(&BackRun.Data[index],BgRunParam->minvolt);
          index += 4;
					break;
//				case 0x000E:    //׮����ѹLW
//					BackRun.Data[index++] = 4;
//				  Common_LittleIntTo(&BackRun.Data[index],add);
//          index += 4;
//					break;
//				case 0x000F:    //׮������LW
//					BackRun.Data[index++] = 4;
//				  Common_LittleIntTo(&BackRun.Data[index],add);
//          index += 4;
//					break;
				case 0x0010:    //�����ѹ A  
					BackRun.Data[index++] = 4;
				  Common_LittleShortTo(&BackRun.Data[index],BgAcMete->Uab);
          index += 4;
					break;
				case 0x0011:    //�����ѹ B 
					BackRun.Data[index++] = 4;
				  Common_LittleShortTo(&BackRun.Data[index],BgAcMete->Ubc);
          index += 4;
					break;
				case 0x0012:    //�����ѹ C 
					BackRun.Data[index++] = 4;
				  Common_LittleShortTo(&BackRun.Data[index],BgAcMete->Uca);
          index += 4;
					break;
				case 0x0013:    //�Ƿ��ͣ������ѣ�1���գ�0��
					BackRun.Data[index++] = 1;
				  BackRun.Data[index++] = 0;
					break;
				case 0x0014:    //ͣ������0.01
					BackRun.Data[index++] = 2;
				  BackRun.Data[index++] = 0;
		      BackRun.Data[index++] = 0;
				  Delay5Ms(1);
					break;
				case 0x0015:    //�׶ε��0.01
			  case 0x0016:
        case 0x0017:
        case 0x0018:
        case 0x0019:
        case 0x001A:
        case 0x001B:
        case 0x001C:
        case 0x001D:
        case 0x001E:	
          addr = (GetIndex - 0x0015)<<1;					
					BackRun.Data[index++] = 2;
				  memcpy(&BackRun.Data[index],&GetFeiLv[addr],2);
          index += 2;
					break;
				case 0x001F:    //�׶�ʱ��
			  case 0x0020:
        case 0x0021:
        case 0x0022:
        case 0x0023:
        case 0x0024:
        case 0x0025:
        case 0x0026:
        case 0x0027:
        case 0x0028:		
          addr = (GetIndex - 0x0015)<<1;			
					BackRun.Data[index++] = 2;
				  memcpy(&BackRun.Data[index],&GetFeiLv[addr],2);
          index += 2;
					break;
				case 0x0029:			//�������		
					BackRun.Data[index++] = 4;
				  Common_LittleShortTo(&BackRun.Data[index],BFeiLvPtr->SectTimeFee.srvrate[0]);
          index += 4;
					break;
				case 0x002A:			//Ӳ���汾
					BackRun.Data[index++] = 20;
				  memset(&BackRun.Data[index] ,0,20);
		      index += 20;
					break;
				case 0x002B:			//����汾
					BackRun.Data[index++] = 20;
				  memset(&BackRun.Data[index] ,0,20);
		      index += 20;
					break;
//				case 0x002C:			//��������	
//					BackRun.Data[index++] = 25;
//				  memset(&BackRun.Data[index] ,0,25);				  
//  			  memcpy(&BackRun.Data[index] ,BgDevParam->chargeId,8);
//		      index += 25;
//					break;
				default:
					sendunit --;
				  index = index -2;
					break;
			}
			sendunit++;
			if(index > CheDian_MAX)
			{
				return;
			}
		}	
    Common_LittleShortTo(&BackRun.Data[DataStartAddr],sendunit);				
		CheDin_FrameSend(ResponseFrame, GET_DEVPARA, CLIENTO, index);
  }
}

/****************************************
��ȡ�澯���ݣ�����׮Э����6.3�澯��
*******************************************/
void GetRunAlarm(uint8 gunNo,uint8 *buff)
{
//	uint8 i;
	DEV_ACMETER_TYPE *BgAcMete = TskAc_GetMeterDataPtr(gunNo);
	CHARGE_TYPE  *ptrBgRunData = ChgData_GetRunDataPtr(gunNo);
	
	memset(buff,0,8);
	if(1 == ptrBgRunData->meter->statu.bits.commerr){  //?????
		buff[0] += 0x10;
	}

	if(1 == Check_GetErrCode(gunNo,ECODE18_CC1LNK)){
		buff[0] += 0x20;
	}

	if(BgAcMete->statu.bits.UabOver){  //��ѹ A
		buff[1] += 0x01;
	}

	else if(BgAcMete->statu.bits.UabUnder){  //Ƿѹ A
		buff[1] += 0x02;
	}

	if(BgAcMete->statu.bits.UbcOver){  //��ѹ B
		buff[1] += 0x04;
	}
	else if(BgAcMete->statu.bits.UbcUnder){  //Ƿѹ B
		buff[1] += 0x08;
	}

	if(BgAcMete->statu.bits.UcaOver){  //��ѹ C
		buff[1] += 0x10;
	}
	else if(BgAcMete->statu.bits.UcaUnder){  //Ƿѹ C
		buff[1] +=0x20;
	}
	
	if(1 == ptrBgRunData->input->statu.bits.stop){  //��ͣ
		buff[2] += 0x01;
	}
	if(1 == ptrBgRunData->input->statu.bits.spd){   //������
		buff[2] += 0x02;
	}
	
//	if(BgRunData->iso->statu.word){
//		buff[2] += 0x20;
//	}
		
//	if(BgRunData->bms->car.bsm.battalm.bits.linkalm){
//		buff[2] += 0x08;
//	}
	if(1 == Check_GetErrCode(gunNo,ECODE31_BMSCOMM)){
		buff[2] += 0x08;
	}
}
/****************************
�жϳ�翨����  LW
���������̨�����������Ҫת��
****************************/
uint8 GetCardType2(uint8 *cardno,uint8 chgtype)
{
	uint8 rtn = 0;
	if(START_BY_CARD == chgtype){ //???
		rtn = 0x61;
	}
	else if(START_BY_BKGROUND == chgtype){ //???
		rtn = 0x71;
	}
	else if(START_BY_ONLINECARD == chgtype){ //???
		rtn = 0x70;
	}
	else if(START_BY_VIN == chgtype){ //app????		
			rtn = 0x78;      //app????		
	}
	return rtn;
}

/********************
0 ����
1 �����У��ǿ��У���̨�ɳ��
2 �����
3 ������
********************/
uint8 GetWorkType(uint8 gunNo)
{
	CHARGE_TYPE  *BgRunData = ChgData_GetRunDataPtr(gunNo);
  uint8 rtn = 0;

  switch(BgRunData->logic->workstep){
		case  STEP_IDEL:      /*���н׶�*/
			rtn = (CC1_4V == BgRunData->gun->statu.bits.cc1stu) ? 1 : 0;
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
      rtn = 2;
     break;		
		case  STEP_CHGEND:
		case  STEP_WAITOVER:	
			//�Ѿ��ϴ���¼
			rtn = (0 == g_BackCtrl.Runflag[gunNo]) ? 3 : 2;	
			break;
    case  STEP_WAIT_SUB:	
      rtn = 3;
      break;		
		default:
			break;;
	}
  return rtn;	
}

/************************************************
��ȡʵʱ����
���������������״̬�ı䣬�����ϴ�һ��ʵʱ����
������ʱ�򿴺�̨�Ƿ񲻶��·�ʵʱ��������
*************************************************/
uint8 GetUpData_ItemData(uint8 gunNo, uint16 item,uint8 *buf)
{
	   uint8 index ;
	   uint8 *ptrData = buf;

		 CHARGE_TYPE  *PtrBgRunData = ChgData_GetRunDataPtr(gunNo);
		 REAL_BILL_DATA_ST *PtrBgFeeData = GetFeeData(gunNo);
	
	   index = 2;
	   Common_LittleShortTo(&ptrData[0],item);
		 switch(item){
//				case 0x0603:   //�������ض������  
//					ptrData[index++] = 2;
//  			  Common_LittleShortTo(&ptrData[index],PtrBgRunData->bms->car.brm.battecapty);  
//          index += 2;	
//					break;
//				case 0x0604:   //����ܵ�ѹ  
//					ptrData[index++] = 2;
//				  Common_LittleShortTo(&ptrData[index],PtrBgRunData->bms->car.brm.battvolte);  
//          index += 2;	
//					break;
				case 0x0B01:    //�豸״̬  
					ptrData[index++] = 1;
				  ptrData[index++] = GetWorkType(gunNo);
					break;
				case 0x0B02:    //��翨��
					ptrData[index++] = 20;				  
				  UpCardNoASCII(&ptrData[index],PtrBgRunData->bill->cardNo);
		      index += 20;	
					break;
				case 0x0B03:    //����VIN    
					ptrData[index++] = 17;
				  memset(&ptrData[index], 0, 17); 
				  memcpy(&ptrData[index],PtrBgRunData->bms->car.brm.vin,17);
          index += 17;	
					break;
				case 0x0B04:    //����ѹ
					ptrData[index++] = 4;
          Common_LittleIntTo(&ptrData[index],PtrBgRunData->meter->volt); 				
          index += 4;	
					break;
				case 0x0B05:    //������
					ptrData[index++] = 4;
				  Common_LittleIntTo(&ptrData[index],PtrBgRunData->meter->current*10);  
          index += 4;	
					break;
				case 0x0B06:    //���ʱ��
					ptrData[index++] = 4;
				  Common_LittleIntTo(&ptrData[index],PtrBgRunData->logic->time);  
          index += 4;	
					break;
				case 0x0B07:    //�����
					ptrData[index++] = 4;
				  Common_LittleIntTo(&ptrData[index],PtrBgRunData->logic->money);  
          index += 4;	
					break;
				case 0x0B08:    // ������
					ptrData[index++] = 4;
				  Common_LittleIntTo(&ptrData[index],PtrBgRunData->logic->energy);  
          index += 4;	
					break;
				case 0x0B09:    //ʣ��ʱ��  
					ptrData[index++] = 4;
				  Common_LittleShortTo(&ptrData[index],PtrBgFeeData->real.lasttimes);
          index += 4;
					break;
				case 0x0B0A:    //��ǰSOC  
					ptrData[index++] = 1;
				  ptrData[index++] = PtrBgFeeData->bill.soc % 101;
					break;
				case 0x0B0B:    //�澯��Ϣ  
					ptrData[index++] = 8;
				  memset(&ptrData[index],0,8);
				  GetRunAlarm(gunNo,&ptrData[index]);
          index += 8;
					break;
				case 0x0B0C:    //��翨���
					ptrData[index++] = 4;
				  Common_LittleIntTo(&ptrData[index],PtrBgRunData->bill->beforemoney);
          index += 4;
					break;
				case 0x0B0D:    //��翨����
					ptrData[index++] = 1;
				  ptrData[index++] = GetCardType2(PtrBgRunData->bill->cardNo,PtrBgRunData->bill->startmod);  //GetCardType(PtrBgRunData->bill->cardNo,PtrBgRunData->bill->cardtype);
					break;
				case 0x0B0E:    //��緽ʽ  1:������� 2��ԤԼ���
					ptrData[index++] = 1;
				  ptrData[index++] = 1;
					break;
				case 0x0B0F:    //���ģʽ  
					ptrData[index++] = 1;
				  ptrData[index++] = 0;;
					break;
//				case 0x0B10:    //��ѹ���� 
//					ptrData[index++] = 4;
//				  Common_LittleShortTo(&ptrData[index],PtrBgRunData->bms->car.bcl.needvolt);
//          index += 4;
//					break;
//				case 0x0B11:    //�������� 
//					ptrData[index++] = 4;
//				  Common_LittleShortTo(&ptrData[index],PtrBgRunData->bms->car.bcl.needcurr);
//          index += 4;
//					break;
//				case 0x0B12:    //��λ��״̬ 
//					ptrData[index++] = 1;
//				  ptrData[index++] = 0x18; //δ֪
//					break;
				case 0x0B13:    //��ǰ���ܱ����					
					ptrData[index++] = 4;
				  Common_LittleIntTo(&ptrData[index],PtrBgRunData->meter->energy);
          index += 4;
					break;
//				case 0x0B14:    //����ѹ
//					ptrData[index++] = 4;
//				  Common_LittleIntTo(&ptrData[index],PtrBgRunData->meter->volt); 	
//          index += 4;
//					break;
//				case 0x0B15:			//������	
//					ptrData[index++] = 4;
//				  Common_LittleIntTo(&ptrData[index],PtrBgRunData->meter->current*10); 	
//          index += 4;
//					break;
				default:
					index -= 2;
					break;
			}
			return index;
}

/*******************************
���ʵʱ�����ϴ�
********************************/
#define MAX_UNIT_BUFF  256
void CheDian_GetChgData(uint8 gunNo,uint8 dir)
{
	uint16 index;
  uint16	Item,rtn;
	uint8 GetIndexSum = 0;
	uint8 Getpuff[MAX_UNIT_BUFF];
 
	uint8 i;
	int16 sendunit = 0;

  index = DataStartAddr;

	if(gunNo >= DEF_MAX_GUN_NO)return;
	if ( RcvFlag == dir ){    //����֡����
	
    //���ݵ�Ԫ����
		GetIndexSum = Common_LittleToShort(&BackRun.Data[index]);
		if((GetIndexSum > 128)||(0 == GetIndexSum)){
			return;
		}
		
		index +=2;
		
		//������Ҫ��Ԫ����
		memcpy(Getpuff,&BackRun.Data[index],(GetIndexSum * 2));
		memset(&BackRun.Data[0],0,CheDian_MAX);		
		
		//��䵥Ԫ����
		sendunit = 0;
		for(i = 0 ; i < GetIndexSum ; i++){
			Item = Common_LittleToShort(&Getpuff[i<<1]);
			rtn =  GetUpData_ItemData(gunNo,Item,&BackRun.Data[index]);
			if( rtn > 0 ) {
				index += rtn;
				sendunit++;
			}
			if(index > CheDian_MAX)
			{
				return;
			}
		}//end for
		BackRun.UpDataTime[gunNo] = GetSystemTick();
    Common_LittleShortTo(&BackRun.Data[DataStartAddr],sendunit);		
		CheDin_FrameSend(ResponseFrame, GET_CHGDATA, conSoure[gunNo], index);
   }
}

/*************************************
����д����
************************************/
void CheDian_SetPara(uint8 gunNo,uint8 dir)
{
	uint16 index = DataStartAddr, temp;
	
//	if(gunNo >= DEF_MAX_GUN_NO)return;
	if(RcvFlag == dir){    //����֡����
	  if(0 != ServerSetFeiLv(AGUN_NO,BackRun.Data))
		{
			return;
		}
		//���ݵ�Ԫ����
		BackRun.Data[index++] = 0x14;
		BackRun.Data[index++] = 0;
		
	//���ݵ�Ԫ��ʶ
 	  for(uint8 i = 0; i < 0x14; i++)
		{
			temp = i + 0x15;
			memcpy(&BackRun.Data[index],&temp,2);
			index += 2;
		}		
		CheDin_FrameSend(ResponseFrame, SET_PARA, conSoure[gunNo], index);	
  }
}


/***************************************** 
��ȡ����¼ 
SerialNo = 0
�������¼�ϴ�
��������ˮ�ŵ���SerialNo�ϴ���

�ҵ��ļ�¼��ֵ��BackGet
û�з���1
�з���0
******************************************/
uint8  CheDian_ChgRecord(CALCBILL_DATA_ST *BackGet)
{
	return Rd_FindUnAccountRecord(BackGet);
}	

/************************ 
��Ӧ��վ�Գ���¼��ȷ�� 
�Ա���ˮ��
**************************/
uint32 AckHostSureChgRd(uint8 *buf)
{
	uint32 TmpSerialNum,  reson;
	uint8 GetSerialNum[32];
	
	TmpSerialNum = Common_LittleToInt(&buf[0]);  //��ˮ��
	reson = Common_LittleToInt(&buf[8]);         //���
	if( reson > 1 ){   //0������1��ˮ���ظ� 2����
		return 0xffff;
	}
	memset(GetSerialNum,0,32);
	memcpy(GetSerialNum,&buf[0],4);
	if(1 == Clear_RecordIsReport(GetSerialNum)){
			return TmpSerialNum;
	}
	return 0xffff;
}


/*���ʱ�ε���*/
void  Fill_SectEnergy(uint8 *buff,CALCBILL_DATA_ST *ptrBackRecord)
{
	uint8 TimeNum = 10;
	uint8 i, StaNum , endNum;
	uint32 u32Tmp;
	PARAM_FEE_TYPE *SysDevParam = ChgData_GetFeeRate(AGUN_NO);
	
	StaNum = 0;
	for(i = 0; i < TimeNum; i++){
		
		if(i < (TimeNum - 1)){		
			endNum = (0 == SysDevParam->SectTimeFee.cdsectNo[i+1]) ? MAX_CHG_SECT : (SysDevParam->SectTimeFee.cdsectNo[i+1]%MAX_CHG_SECT);
		}
		else{
			endNum = MAX_CHG_SECT;
		}
		
		u32Tmp = 0;
		for(;StaNum < endNum; StaNum++){
			u32Tmp += ptrBackRecord->sectenergy[StaNum];
		}
		Common_LittleIntTo(&buff[i*8],u32Tmp);
		if(endNum >= MAX_CHG_SECT){
			break;
		}
	}
}

/**������¼**/
uint16 Fill_OneRecordItem(uint16 Item,uint8 *data,CALCBILL_DATA_ST *ptrBackRecord)
{
	uint8 index ;
	uint8 *ptrItemData = data;
	
	index = 0;
	Common_LittleShortTo(&ptrItemData[index],Item);
	index +=2;
	switch(Item){
				case 0x0101:   //��緽ʽ  1���������   2��ԤԼ���
					ptrItemData[index++] = 1;
				  ptrItemData[index++] = 1;
					break;
				case 0x0102:   //���ģʽ
					ptrItemData[index++] = 1;
				  ptrItemData[index++] = 0;
					break;
				case 0x0103:    //��翨����   
					ptrItemData[index++] = 1;
				  ptrItemData[index++] = GetCardType2(ptrBackRecord->cardNo,ptrBackRecord->startmod);
					break;
				case 0x0104:    //��翨��
					ptrItemData[index++] = 20;
				  UpCardNoASCII(&ptrItemData[index],ptrBackRecord->cardNo);
          index += 20;	
					break;
				case 0x0105:    //����VIN
					ptrItemData[index++] = 17;
				  memcpy(&ptrItemData[index],&ptrBackRecord->vincode,17); 
          index += 17;	
					break;
				case 0x0106:    //���ǰ���
					ptrItemData[index++] = 4;			
				  Common_LittleIntTo(&ptrItemData[index],ptrBackRecord->beforemoney);
          index += 4;	
					break;
//				case 0x0107:    //����ѹ
//					ptrItemData[index++] = 4;
//				  Common_LittleIntTo(&ptrItemData[index],ptrBackRecord->beforemoney); //lw
//          index += 4;	
//					break;
//				case 0x0108:    //������
//					ptrItemData[index++] = 4;
//				  Common_LittleIntTo(&ptrItemData[index],ptrBackRecord->beforemoney); //lw
//          index += 4;	 
//					break;
				case 0x0109:    //���ʱ��
					ptrItemData[index++] = 4;
				  Common_LittleIntTo(&ptrItemData[index],ptrBackRecord->chgsecs);
          index += 4;	
					break;
				case 0x010A:    //�����
					ptrItemData[index++] = 4;
				  Common_LittleIntTo(&ptrItemData[index],ptrBackRecord->billmoney);
          index += 4; 
					break;
				case 0x010B:    //������
					ptrItemData[index++] = 4;
				  Common_LittleIntTo(&ptrItemData[index],ptrBackRecord->energy);
          index += 4;
					break;
				case 0x010C:    //���ǰ����
					ptrItemData[index++] = 4;
				  Common_LittleIntTo(&ptrItemData[index],ptrBackRecord->startenergy);
          index += 4;
					break;
				case 0x010D:    //�������
					ptrItemData[index++] = 4;
				  Common_LittleIntTo(&ptrItemData[index],ptrBackRecord->stopenergy);
          index += 4;
					break;
//				case 0x010E:    //ʣ��ʱ��  lw
//					ptrItemData[index++] = 4;
//				  Common_LittleIntTo(&ptrItemData[index],ptrBackRecord->storeNo);
//          index += 4;
//					break;
				case 0x010F:    //��ǰSOC
					ptrItemData[index++] = 1;
				  ptrItemData[index++] = ptrBackRecord->soc;
					break;
				case 0x0110:    //�Ƿ��ϴ���վ
					ptrItemData[index++] = 1;
				  ptrItemData[index++] = ptrBackRecord->IsReport;
					break;
				case 0x0111:    //�Ƿ񸶷�  
					ptrItemData[index++] = 1;
				  ptrItemData[index++] = 0;//BackRecord.IsPay;
					break;
				case 0x0112:    //��ֹԭ��
					ptrItemData[index++] = 1;
				  ptrItemData[index++] = ptrBackRecord->endreason;
					break;
				case 0x0113:    //��翪ʼʱ�� 
					ptrItemData[index++] = 7;
				  ptrItemData[index++] = ptrBackRecord->starttm[0];
				  ptrItemData[index++] = 0x20;
				  memcpy(&ptrItemData[index],&(ptrBackRecord->starttm[1]),5); 
          index += 5;
					break;
				case 0x0114:    //������ʱ�� 
					ptrItemData[index++] = 7;
				  ptrItemData[index++] = ptrBackRecord->stoptm[0];
				  ptrItemData[index++] = 0x20;
				  memcpy(&ptrItemData[index],&ptrBackRecord->stoptm[1],5); 
          index += 5;
					break;
				case 0x0115:    //��¼��ˮ��	
					ptrItemData[index++] = 4;	
				  memcpy(&ptrItemData[index],ptrBackRecord->serialNo,4);
          index += 4;
					break;
				case 0x0116:    //��¼�洢��			
					ptrItemData[index++] = 4;
				  memcpy(&ptrItemData[index],ptrBackRecord->serialNo,4);
	//			  Common_LittleIntTo(&ptrData[index],BackRecord.storeNo);			  
          index += 4;
					break;
				case 0x0117:			//10��ʱ�γ���������   lw
					ptrItemData[index++] = 80;
				  memset(&ptrItemData[index],0,80);
				  Fill_SectEnergy(&ptrItemData[index],ptrBackRecord);
	//			  Common_LittleIntTo(&ptrItemData[index],ptrBackRecord->energy);
          index += 80;
					break;
				default:
					index -= 2;
					break;
			}
			return index;
}



uint16 FillGetRecord(CALCBILL_DATA_ST *ptrBackRecord)
{
	uint16 i,rtn;
	uint16 sendunit = 0;
	uint16 startitem = 0x0101; 
	uint16 enditem = 0x0118; 
	uint16 index = DataStartAddr;
	//���ݵ�Ԫ����		
	index +=2;
	memset(&BackRun.Data[0],0,CheDian_MAX);
	for( i = startitem; i < enditem; i++ ) {
		rtn = Fill_OneRecordItem(i,&BackRun.Data[index],ptrBackRecord);
		if( rtn > 0 ) {
			sendunit++;
			index += rtn;
			if(index > CheDian_MAX){
				return 0;
			}
		}
	}
	Common_LittleShortTo(&BackRun.Data[DataStartAddr],sendunit);
	
	return index;
}


/*************************
�ϴ����߳���¼
**************************/
void CheDian_RecordAuto(uint8 gunNo,uint8 dir)
{	
	uint16 index = DataStartAddr;
	CALCBILL_DATA_ST  BackGetRecor;
	if(gunNo >= DEF_MAX_GUN_NO)return;
	if(SendEN == dir){           //����֡����
		if( 1 == BackRun.RecordMsg ) {  /*���˵��ϴ�*/
				BackRun.RecordMsg = 0;
				memcpy(&BackGetRecor, Bill_GetBillData(gunNo%DEF_MAX_GUN_NO),sizeof(CALCBILL_DATA_ST));
			
				index = FillGetRecord(&BackGetRecor);
        gunNo = BackGetRecor.gunno;				
				CheDin_FrameSend( AutoFrame, AUTO_RECORD, conSoure[gunNo], index);	
			
	  }else if(1 == CheDian_ChgRecord(&BackGetRecor)) {			
			index = FillGetRecord(&BackGetRecor);	
      gunNo = BackGetRecor.gunno;			
			CheDin_FrameSend(AutoFrame, AUTO_RECORD, conSoure[gunNo], index);	
    }	
	}
	else if(RcvFlag == dir){    //����֡����  		
		AckHostSureChgRd(&BackRun.Data[index+2]);			
  }	
}




/***********************
������ʱ
***********************/
void CheDian_SetTime(uint8 gunNo,uint8 dir)
{
	uint16 index = DataStartAddr;
	
	if(RcvFlag == dir){    //����֡����
		//����ʱ��
		updataTime(&BackRun.Data[DataStartAddr], 0);
    //ȷ�ϱ�ʶ��		
		BackRun.Data[index++] = 0x00;
		CheDin_FrameSend(ResponseFrame, SET_TIME, CLIENTO, index);		
  }
}

/***********************************
�����������͵Ĺر�������Ϣ
0.�����ɹ�   1.����ʧ��
2.ֹͣ�ɹ�   3.ֹͣʧ��
***********************************/
extern uint8 IsJudgeDevErrDtatus(void);
uint8 Deal_SetOnOff(uint8 gunNo)
{
	START_PARAM  *PtrStartPara = ChgData_GetStartParaPtr(gunNo);
	CHARGE_TYPE  *BgRunData = ChgData_GetRunDataPtr(gunNo);
	
	uint8 CtrlType,CtrData[3] ; 
	uint16 index = DataStartAddr;
	uint32 Value32;
	
  CtrlType = BackRun.Data[index++];
	if(CtrlType == 0x01)
	{
		/* ������� */
		if ((PtrStartPara->vailfg != 0) || (CC1_4V != BgRunData->gun->statu.bits.cc1stu))
		{
			/* ��ռ�ã�����ʹ�� */
			/* ǹû��� */
			return ON_FAIL;
		}
		else
		{				
			    PtrStartPara->startby = START_BY_BKGROUND; /*ɨ���̨*/	
			    //�� ��
			    Get_BcdCardNo(PtrStartPara->account,&BackRun.Data[index]);
			    //memcpy(PtrStartPara->account,&BackRun.Data[index],20);
			    index += 20;
					//�����
					PtrStartPara->money = Common_LittleToInt(&BackRun.Data[index]);
					index += 4;
					//��̨������ˮ��
			    memset(PtrStartPara->serilNo,0,32);
			    memcpy(PtrStartPara->serilNo,&BackRun.Data[index],4);
					index += 4;
					//���ģʽ				
          CtrlType	= (Common_LittleToInt(&BackRun.Data[index]) % 4);//CTR_AUTO
					index += 4;
					//���ģʽֵ�����-��  ʱ��-��  ����-0.01KW*H	
			    Value32 = Common_LittleToInt(&BackRun.Data[index]);
			    index += 4;
			    switch(CtrlType){
						case 0:
							PtrStartPara->chgmod = CHG_AUTO_TYPE;
							break;
						case 1:
							PtrStartPara->chgmod = CHG_MONEY_TYPE;
							PtrStartPara->setmoney = Value32;
							break;
						case 2:
							PtrStartPara->chgmod = CHG_TIME_TYPE;
							PtrStartPara->settime = Value32;
							break;
						case 3:
							PtrStartPara->chgmod = CHG_ENERGY_TYPE;
							PtrStartPara->setenergy = Value32;
							break;
						default:
							PtrStartPara->chgmod = CHG_AUTO_TYPE;
							break;
					} 

					if(1 == GetWorkType(gunNo))
					{
						CtrData[0] = 1;//����
			      CtrData[1] = gunNo;//ǹ��
						PtrStartPara->vailfg = 1;
						PtrStartPara->gunNo = gunNo;
						PtrStartPara->startby = START_BY_BKGROUND;
						SendMsgWithNByte(MSG_START_STOP,2,&CtrData[0],conCtrlPrio[gunNo]);
						g_BackCtrl.Runflag[gunNo] = 1;
						Delay5Ms(100);
						return ON_SUCC;
					}
					else{
						CtrData[0] = gunNo;//ǹ��
			      CtrData[1] = CARD_NOTIDLE_ERR;
						SendMsgWithNByte(MSG_VERIFY_RESULT,2,&CtrData[0],APP_TASK_SCREEN_PRIO);
					}
//					PtrStartPara->vailfg = 0;
					return ON_FAIL;			  
		}
	}
	else if(0x02 == CtrlType)
	{/* ������� */
  	CtrData[0] = 0;//ֹͣ
		CtrData[1] = gunNo;//ǹ��
		CtrData[2] = 1;//0=����  1=��̨
		SendMsgWithNByte(MSG_START_STOP,3,&CtrData[0],conCtrlPrio[gunNo]);	 
		Delay5Ms(2);
		return OFF_SUCC;
	}
	else{
		CtrData[0] = gunNo;//ǹ��
	  CtrData[1] = CARD_ILLEGAL_ERR;
		SendMsgWithNByte(MSG_VERIFY_RESULT,2,&CtrData[0],APP_TASK_SCREEN_PRIO);
	}
	return  OTHER_ERR;
}

/*��������ֹͣ����*/
void CheDian_SetOnOff(uint8 gunNo,uint8 dir)
{
	uint8 temp;
	
	if(gunNo >= DEF_MAX_GUN_NO)return ;
	
	if(RcvFlag == dir){    //����֡����				
	  /*��ͣ�������*/
		temp = Deal_SetOnOff(gunNo);
		if( OTHER_ERR != temp ) {
			CheDian_AckOnOffSend(gunNo,temp);
//			CheDian_UpDataAuto(gunNo,SendEN);
		}		
  }
}

/*******************************************************************
�����������ϴ�����Ϣ����Ӧ����Ϣ
CtrData[0]����
CtrData[1]��״̬ 
0:������1������У�2�����㣻 3����ʧ��4����Ч��;5:����
********************************************************************/
void Deal_AckCardno(uint8 gunNo)
{
		START_PARAM  *PtrStartPara = ChgData_GetStartParaPtr(gunNo);
	  CHARGE_TYPE  *BgRunData = ChgData_GetRunDataPtr(gunNo);

		uint8 Cardstate,CtrData[2],GetCardNo[20]; 
		uint16 index = DataStartAddr;
    	
	    PtrStartPara->startby = START_BY_ONLINECARD; /*ˢ��*/	
			//�� ��
	    memset(GetCardNo,0,sizeof(GetCardNo));
			Get_BcdCardNo(GetCardNo,&BackRun.Data[index]);
	    
			index += 20;
			
			//�����
			PtrStartPara->money = Common_LittleToInt(&BackRun.Data[index]);
			index += 4;
			//��״̬
			Cardstate = BackRun.Data[index];//BackRun.Data[index+24]
			index += 1;
			//��̨������ˮ��
			memset(PtrStartPara->serilNo,0,32);
			memcpy(PtrStartPara->serilNo,&BackRun.Data[index],4);
			index += 4;
		 if((0 == Cardstate)&&(memcmp(&GetCardNo,PtrStartPara->account,20)==0)\
			 &&(1 == GetWorkType(gunNo))){  //��״̬  ���Գ��
					CtrData[0] = 1;		
				  CtrData[1] = gunNo;//ǹ��
          g_BackCtrl.Runflag[gunNo] = 1;			
          PtrStartPara->vailfg = 1;		
          PtrStartPara->gunNo = gunNo;			 
          SendMsgWithNByte(MSG_START_STOP,2,&CtrData[0],conCtrlPrio[gunNo]);	
		}
		else if(3 < Cardstate){
			CtrData[0] = gunNo;//ǹ��
	    CtrData[1] = CARD_ILLEGAL_ERR;
		  SendMsgWithNByte(MSG_VERIFY_RESULT,2,&CtrData[0],APP_TASK_SCREEN_PRIO);
		}
		else if(1 != GetWorkType(gunNo)){
			CtrData[0] = gunNo;//ǹ��
			CtrData[1] = CARD_NOTIDLE_ERR;
		  SendMsgWithNByte(MSG_VERIFY_RESULT,2,&CtrData[0],APP_TASK_SCREEN_PRIO);
		}
}
/*******************************************************************
�����������ϴ�����Ϣ����Ӧ����Ϣ
CtrData[0]����
CtrData[1]��״̬ 
0:������1������У�2�����㣻 3����ʧ��4����Ч��;5:����
********************************************************************/
void Deal_AckVIN(uint8 gunNo)
{
		START_PARAM  *PtrStartPara = ChgData_GetStartParaPtr(gunNo);
	  CHARGE_TYPE  *BgRunData = ChgData_GetRunDataPtr(gunNo);

		uint8 Cardstate,CtrData[2],GetCardNo[20]; 
		uint16 index = DataStartAddr;
    	
	    PtrStartPara->startby = START_BY_VIN; /*ˢ��*/	
			//�� ��
	    memset(GetCardNo,0,sizeof(GetCardNo));
			memcpy(GetCardNo,&BackRun.Data[index],17);
	    
			index += 20;
			
			//�����
			PtrStartPara->money = Common_LittleToInt(&BackRun.Data[index]);
			index += 4;
			//��״̬   	CARD_PASSWD_ERR = 1,  //��������,�����ԣ�	CARD_ILLEGAL_ERR =2,  //����ʶ�� 	CARD_NOMONEY_ERR = 3, //���� 	CARD_NOTIDLE_ERR = 4, //���Ƿǿ��� 	CARD_VIN_NO_MATCH = 5,  //VIN�벻ƥ��  patli 20190926
			Cardstate = BackRun.Data[index];//BackRun.Data[index+24] 
			index += 1;
			//��̨������ˮ��
			memset(PtrStartPara->serilNo,0,32);
			memcpy(PtrStartPara->serilNo,&BackRun.Data[index],4);
			index += 4;

	 if((0 == Cardstate)&&(memcmp(&GetCardNo,PtrStartPara->vincode,17)==0)\
			 &&(STEP_IDEL == BgRunData->logic->workstep)){  //��״̬  ���Գ��
	//	 if((0 == Cardstate)&&(memcmp(&GetCardNo,PtrStartPara->vincode,17)==0)){  //��״̬  ���Գ��
			
				  CtrData[0] = 1;		
				  CtrData[1] = gunNo;//ǹ��
          g_BackCtrl.Runflag[gunNo] = 1;			
          PtrStartPara->vailfg = 1;		
          PtrStartPara->gunNo = gunNo;	
          PtrStartPara->vinback =2;				 
          SendMsgWithNByte(MSG_START_STOP,2,&CtrData[0],conCtrlPrio[gunNo]);	
		}
		else if(3 < Cardstate){
			CtrData[0] = gunNo;//ǹ��
	    CtrData[1] = CARD_ILLEGAL_ERR;
		  SendMsgWithNByte(MSG_VERIFY_RESULT,2,&CtrData[0],APP_TASK_SCREEN_PRIO);
		}
		else if(1 != GetWorkType(gunNo)){
			CtrData[0] = gunNo;//ǹ��
			CtrData[1] = CARD_NOTIDLE_ERR;
		  SendMsgWithNByte(MSG_VERIFY_RESULT,2,&CtrData[0],APP_TASK_SCREEN_PRIO);
		}
}
/*****************************************
�ϴ�ˢ����Ϣ
���������ܲ��û�����ط����ϴ�����
*****************************************/
void CheDian_CardnoAuto(uint8 gunNo,uint8 dir)
{
	uint16 index = DataStartAddr;
	
	if(gunNo >= DEF_MAX_GUN_NO)return ;
	
	START_PARAM  *PtrStartPara = ChgData_GetStartParaPtr(gunNo);
	CARD_INFO *PtrCard = TskCard_GetCardInfPtr();	
	CHARGE_TYPE  *BgRunData = ChgData_GetRunDataPtr(gunNo);
		
	if(SendEN == dir){           //����֡����
	
    //����			
		memset(&BackRun.Data[index], 0, 20); 
		if(START_BY_VIN != PtrStartPara->startby){
			UpCardNoASCII(&BackRun.Data[index],PtrCard->cardNo);
			memcpy(PtrStartPara->account, PtrCard->cardNo, 20); 
		}
		else{
			memcpy(PtrStartPara->vincode, BgRunData->bms->car.brm.vin, sizeof(PtrStartPara->vincode));
			Dealadd_VIN(PtrStartPara->vincode,17);
			memcpy(&BackRun.Data[index], PtrStartPara->vincode, sizeof(PtrStartPara->vincode)); 
		}
		
		index += 20;		
		CheDin_FrameSend(AutoFrame, AUTO_CARDNO, conSoure[gunNo], index);			
	}
	else if(RcvFlag == dir){    //����֡����
		if(START_BY_VIN != PtrStartPara->startby){
      Deal_AckCardno(gunNo);	
		}
		else{
			Deal_AckVIN(gunNo);	
		}
  }
}
/*************************
�������� ֹͣ��Ӧ����
��̨�Գ��״̬�жϣ�ֱ���ϴ���״̬
**************************/
void CheDian_AckOnOffSend(uint8 gunNo,uint8 onoff )
{	 
  uint16 index = DataStartAddr;
	
	BackRun.Data[index++] = onoff;
	CheDin_FrameSend(AutoFrame, SET_ONOFF, conSoure[gunNo], index);	
}
/*******************************
����ǹ״̬
Aǹ��0
Bǹ��1
��������0x04  ��0x09�����ظ�
*******************************/
void CheDian_UpDataAuto(uint8 gunNo,uint8 dir)
{
	uint16 rtn,Item;
	uint16 index = DataStartAddr;
	uint16 sendunit = 0;
	
	if(gunNo >= DEF_MAX_GUN_NO)return ;
	
	if(SendEN == dir){           //����֡����
	    
		memset(&BackRun.Data[0],0,CheDian_MAX);
		index +=2;
		Item = 0x0B0B;
		rtn =  GetUpData_ItemData(gunNo,Item,&BackRun.Data[index]);
		if( rtn > 0 ) {
			index += rtn;
			sendunit++;
		}
		
		Item = 0x0B01;
		rtn =  GetUpData_ItemData(gunNo,Item,&BackRun.Data[index]);
		if( rtn > 0 ) {
			index += rtn;
			sendunit++;
		}
		BackRun.UpDataTime[gunNo] = GetSystemTick();		
		Common_LittleShortTo(&BackRun.Data[DataStartAddr],sendunit);		
		CheDin_FrameSend(AutoFrame, GET_CHGDATA, conSoure[gunNo], index);	
	}
}
/***********************************************************************
***��������: CheDianfun
** ��������: ͨ��������Һ���ִ��
***    ����: 
**   ����ֵ:   
*************************************************************************/
uint8  CheDian_fun_Deal(uint8 cmd,uint8 gunNo,uint8 dir)
{
	 uint16 i;
	 const CheDian_t *CheDian = &conCheDian[0];
	 
	 for( i = 0 ; CheDian->cmd != CMD_NULL ; i++ ) {
		 if( cmd == CheDian->cmd ) {
			 if( NULL != CheDian->func ) {
			   CheDian->func(gunNo,dir);
				 break;
			 }
			 return  FALSE;
		 }
		 CheDian++;
	 }
	 return TRUE;
}

/************************************ 
��ȡ��Ҫ�����ϴ�����
CtrData[0] ��������
CtrData[1] ǹ��
************************************/
uint8 CheDian_GetAuto(uint8 *CtrData)
{		
	PARAM_OPER_TYPE *devparaPtr = ChgData_GetRunParamPtr();
	
	 static uint8 sGunlink[DEF_MAX_GUN_NO];  // 0: not  1:link
	 static uint32 sWaitTicks[DEF_MAX_GUN_NO] = { 0 };
	 static uint32 sGuntype[DEF_MAX_GUN_NO] = { 0 };
   uint8 i;
	
   CtrData[1]  = 0;
	 if( 0xAA == BackRun.Commforbidden ){  //�����˳�����ִ��	��������������	 
		 if( GetSystemTick() < sWaitTicks[0] + TIM_NS(10) ) {
			  sWaitTicks[0] = GetSystemTick();
			  CtrData[0] = AUTO_LOGIN;
				return TRUE;	
		 }
		 return FALSE;
	 }
	 
//	 for(i=0;i<DEF_MAX_GUN_NO;i++){
//		if(sGuntype[i] != GetWorkType(i)){		
//			CtrData[0] = AUTO_UPDATA;	
//      CtrData[1] = i;
//			sGuntype[i] = GetWorkType(i);
//	
//			return TRUE;
//		}
//  }
  for(i=0;i<devparaPtr->gunnum;i++){
		CHARGE_TYPE  *ptrBgRunData = ChgData_GetRunDataPtr(i);
	  if(( sGunlink[i] != ptrBgRunData->gun->statu.bits.cc1stu )||(sGuntype[i] != GetWorkType(i))){
		    sGunlink[i] = ptrBgRunData->gun->statu.bits.cc1stu;
			  sGuntype[i] = GetWorkType(i);
			  CtrData[0] = AUTO_UPDATA;	
        CtrData[1] = i;				
		    return TRUE;
	  }
  }	
	for(i=0;i<devparaPtr->gunnum;i++){  //DEF_MAX_GUN_NO
		if(GetSystemTick() > BackRun.UpDataTime[i] + TIM_NS(15)){		
			CtrData[0] = AUTO_UPDATA;	   //����30��û��APP����ֽ�����ʾ����̨�����09�����ж�����
      CtrData[1] = i;	
			
			return TRUE;
		}
  }
	
//	if( GetSystemTick() > sWaitTicks[1] + TIM_NS(50)) {
//		 sWaitTicks[1] = GetSystemTick();
//		 CtrData[0] = AUTO_UPDATA;
//		 CtrData[1] = GetSystemTick()%2;
//     return TRUE;		
//	}
	 


	/* ������ʱ�ϴ���¼ */
	if((TskMain_GetWorkStep(AGUN_NO) != STEP_CHGEND ) &&(TskMain_GetWorkStep(BGUN_NO) != STEP_CHGEND )\
		&&(GetSystemTick()  > BackRun.RecordTime + TIM_NS(60) ) ) \
	{		
			 BackRun.RecordTime = GetSystemTick();
			 CtrData[0] = AUTO_RECORD;	
       return TRUE;		
	}
		
	/* ��ʱ�ϴ������� */
	if(GetSystemTick() >  BackRun.LifeTime + TIM_NS(10) )
	{
		  BackRun.LifeTime = GetSystemTick();
			CtrData[0] = AUTO_LIFE;	
		  return TRUE;
	}
	return FALSE;
}

/************************************ 
��������Э�����������ʼ��
************************************/
void CheDian_Init(void)
{
	memset(&BackRun,0,sizeof(BackRun));
//	BackRun.RecordAutoEN = 1;
}

/************************************ 
��ȡ���������͹�������Ϣ������
************************************/
/* �����̨��Ϣ */
void CheDian_PreBackMsg(MSG_STRUCT *msg)
{
	uint8 u8GunNo;
	uint8 CtrData[4];
	
	switch(msg->MsgType)
	{
		case MSG_ONLINE_CARD:/* �����ȡIC���ʺ���Ϣ */
		{	
			u8GunNo = msg->MsgData[0] % DEF_MAX_GUN_NO;
//      if(1 != GetWorkType(msg->MsgData[0])){
//		    CtrData[0] = msg->MsgData[0];//ǹ��
//			  CtrData[1] = CARD_NOTIDLE_ERR;
//		    SendMsgWithNByte(MSG_VERIFY_RESULT,2,&CtrData[0],APP_TASK_SCREEN_PRIO);
//	    }
//      else{		
			  CheDian_CardnoAuto(u8GunNo,SendEN);
//			}
			break;
		}
		case MSG_VIN_CARD:/* �����ȡIC���ʺ���Ϣ */
		{	
			u8GunNo = msg->MsgData[0] % DEF_MAX_GUN_NO;	
			CheDian_CardnoAuto(u8GunNo,SendEN);
			break;
		}
		case MSG_UP_RECORD:/* �����ϴ���ǰ����¼ */
		{	
      u8GunNo = msg->MsgData[0] % DEF_MAX_GUN_NO;			
      BackRun.RecordMsg = 1;
			BackRun.RecordTime = 0;			
			BackRun.RecordTime =  GetSystemTick();
			CheDian_RecordAuto(u8GunNo,SendEN);
			Delay10Ms(5);
			g_BackCtrl.Runflag[u8GunNo] = 0;
//			CheDian_UpDataAuto(u8GunNo,SendEN);			
			break;
		}
		case MSG_START_STOP_RESULT:
		{
			u8GunNo = msg->MsgData[2] % DEF_MAX_GUN_NO;	
		  CheDian_UpDataAuto(u8GunNo,SendEN);
			Delay10Ms(5);
			break;
		}
#ifdef AN_ETH_TASK
		case MSG_TCPCLIENT_DATA_TO_BACK:
		{
			BackRun.ptr_msg = msg->PtrMsg;
			BackRun.msg_len = msg->MsgLenth;
			BackRun.msg_readed_num = 0;
			//��ȡ�����·����ݲ����д���		  
			if(CheDian_RcvUploadDataDeal(CtrData) == TRUE) {
					  CheDian_fun_Deal(CtrData[0],CtrData[1],RcvFlag);								  //������Ϣ
					  g_BackCtrl.errCount = 0;
					  g_BackCtrl.CommStatus = CONNECT_OK;			  /*0 ͨѶ����	1������*/
					  TskBack_CommStatusSet(CONNECT_OK);
					  g_BackCtrl.CommTimers = GetSystemTick();	  
			Monitor_NetConnect(0);	  
			Delay10Ms(1);			  
			}
			
			break;
		}
#endif		
		default:
			break;
	}
}

uint8 CheDian_check_START_STOP(void)
{
	uint8 i;
  static uint8 sGuntype[DEF_MAX_GUN_NO];
	
	for(i=0;i<DEF_MAX_GUN_NO;i++){
		if(sGuntype[i] != GetWorkType(i)){		
			CheDian_UpDataAuto(i,SendEN);
			sGuntype[i] = GetWorkType(i);
		}
  }
	return 0;
}

extern void SoftReset(void);
uint8 Monitor_NetConnect(uint8 linkfg)
{
	  PARAM_DEV_TYPE *ptrDevPara = ChgData_GetDevParamPtr();
	  struct tm  tmlocal = Time_GetSystemCalendarTime();
	  static uint8 fg = 0;
	  static uint32 sResetMcuTicks = 0;
	  PARAM_COMM_TYPE *ptrBackCOMM = ChgData_GetCommParaPtr();
	
	  if( 0 == linkfg ) {
			fg = 0;
			sResetMcuTicks = GetSystemTick();
			return 0;
		}
	
	  if( ptrDevPara->onlinecard != E_ONLINE_CARD ) {
			return 0;
		}
	  
//		if( ptrBackCOMM->conntype != CONN_ETH ) {
//			return 0;
//		}
		
		if( TskMain_GetWorkStep(AGUN_NO) != STEP_IDEL  \
			&& TskMain_GetWorkStep(BGUN_NO) != STEP_IDEL ) {
			sResetMcuTicks = GetSystemTick();
			return 0;
		}
		
		if( 0 == fg ) {
			fg = 1;
			sResetMcuTicks = GetSystemTick();
		}
		
		if( ( 1 == fg ) && ( GetSystemTick() > sResetMcuTicks + TIM_NMIN(15) ) ) {
			if( tmlocal.tm_hour == 18 ) {
				 SoftReset();
			}else {
				 fg = 0;
			}
			
		}
		
		return 0;
}


/************************************ 
����������
************************************/
void CheDian_Run(void)
{
	uint8 CtrData[4];
	PARAM_COMM_TYPE *ptrBackCOMM = ChgData_GetCommParaPtr();
	static uint32 CheDian_delay = 0,CheDian_ticks = 0;
	static uint8 sDelayT = 3;
//	uint32 getruntime;
	/* ��ȡ����ʱ��*/
	  if(GetSystemTick() > CheDian_ticks + TIM_NS(5)){
			CheDian_delay++;
		}
	  CheDian_ticks = GetSystemTick();
	  
		if(g_BackCtrl.CommStatus  == CONNECT_OFF)
		{				
				if(GetSystemTick() > CheDian_delay + TIM_NS(sDelayT))//5
				{
						CheDian_fun_Deal(AUTO_LOGIN,0,SendEN);							//������Ϣ,���͵���
					  CheDian_delay = GetSystemTick();
						sDelayT = ( g_BackCtrl.errCount++ < 20 ) ? 3 : 8;
				}
				
		}
		else if( CheDian_GetAuto(CtrData) > 0 ) {      	// ��ȡ�����ϴ�����			  
				CheDian_fun_Deal(CtrData[0],CtrData[1],SendEN);  				    	//�����Ϸ�����
			  Delay10Ms(5);   //�ӳ�ʱ�䣬�����������ݰ����̫�̣�ͨ��4Gģ���Ϸ����һ�����ݰ�

		}						

#ifndef AN_ETH_TASK
      //��ȡ�����·����ݲ����д���			
			if(CheDian_RcvUploadDataDeal(CtrData) == TRUE) {
 					CheDian_fun_Deal(CtrData[0],CtrData[1],RcvFlag);  								//������Ϣ
					g_BackCtrl.errCount = 0;
					g_BackCtrl.CommStatus = CONNECT_OK;             /*0 ͨѶ����  1������*/
				  TskBack_CommStatusSet(CONNECT_OK);
					g_BackCtrl.CommTimers = GetSystemTick();	
          Monitor_NetConnect(0);	
          Delay10Ms(1);				
			}
			else {
#endif				
				/* ��̨ͨ����������*/
				if(GetSystemTick() > g_BackCtrl.CommTimers + TIM_NMIN(1))
				{
					 
					 g_BackCtrl.errCount = 0;
					 g_BackCtrl.CommStatus = CONNECT_OFF;
					 g_BackCtrl.CommTimers = GetSystemTick();
					 
					 /*ϵͳ��λ,���������ͨѶδ����10���Ӻ�ִ��*/
//					 Monitor_NetConnect(1);
				}
//				if(GetSystemTick() - g_BackCtrl.CommTimers > TIM_NS(15)){
				if( (BackRun.RcvbeatCnt > 1 ) && (BackRun.RcvbeatCnt % 3 == 0) \
					  &&(GetSystemTick() > g_BackCtrl.CommTimers + TIM_NS(8))) {
							
					  g_BackCtrl.CommTimers = GetSystemTick();
					  CheDian_fun_Deal(AUTO_LOGIN,0,SendEN);
//patli 20200128					  TskBack_CommStatusSet(CONNECT_OFF);
					  Delay10Ms(10);		
				}
				
				if( (g_BackCtrl.CommStatus == CONNECT_OK) && (BackRun.RcvbeatCnt > 10) ) {                       //10������û��Ӧ
					 BackRun.RcvbeatCnt = 0;
					 g_BackCtrl.CommStatus = CONNECT_OFF;
					 TskBack_CommStatusSet(CONNECT_OFF);
					 CheDian_delay = 0;								//�������͵���
					 g_BackCtrl.CommTimers = GetSystemTick() - TIM_NMIN(1);
					 Delay10Ms(10);
				}
#ifndef AN_ETH_TASK				
				
		 }	
#endif

}
#else
void CheDian_Init(void)
{

}

void CheDian_PreBackMsg(MSG_STRUCT *msg)
{

}

void CheDian_Run(void)
{

}

#endif




