/**
  ******************************************************************************
  * @file    TaskCardReader.c
  * @author  zqj
  * @version v1.0
  * @date    2018-04-09
  * @brief   
  ******************************************************************************
*/

#include "common.h"
#include "uart.h"
#include "TaskCardReader.h"
#include "Card.h"
#include "main.h"
#include "TaskMainCtrl.h"
#include "message.h"
#include "ChgData.h"
#include "TaskBackComm.h"
#if  1   //def CARD_ON

//typedef struct
//{
//	uint8   cardtype; 			/*��翨����*/
//	uint8   cardNo[20];     /* ��翨��*/
//	uint8   userPin[3];			/* �û�PIN�� */
//	uint8   lockFlag;				/* ������־ */
//	int32   money;					/* ����� */
//	uint8   key[16];				/* key��  */
//	uint32  backMoney;			/* ���ݿ���� */
//	
//}CARD_INFO;

CARD_OPER_DATA gCardOperData;
const uint8 conManagerCard[] = {0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x88};

//CARD_INFO gCardData;
CARD_INFO *TskCard_GetCardInfPtr(void)
{
	return &gCardOperData.info;
}

//CARD_PARAM_ST gCardParam;

//CARD_PARAM_ST *TskCard_GetCardParamPtr(void)
//{
//	return &gCardParam;
//}
//static CARD_SINGLE_SECT_STRUCT SectTab[10] = {
//{SECT_NO_0,BLOCK_NO_1,STORE_LOG},					/* ����0����1���洢����¼		����Ĭ�ϣ�892551063333 */
//{SECT_NO_0,BLOCK_NO_2,STORE_PIN},					/* ����0����2���洢�û����� 	����Ĭ�ϣ�892551063333 */

//{SECT_NO_1,BLOCK_NO_0,STORE_IcNo},				/* ����1����0���洢���� 			����Ĭ�ϣ�892551063333 */
//{SECT_NO_1,BLOCK_NO_1,STORE_LockFg},			/* ����1����1���洢������־ 	����Ĭ�ϣ�892551063333 */
//{SECT_NO_1,BLOCK_NO_2,STORE_Money},				/* ����1����2���洢���   		����Ĭ�ϣ�892551063333 */

//{SECT_NO_2,BLOCK_NO_0,STORE_KEY},					/* ����2����0���洢KEY��Կ    ����Ĭ�ϣ�601552985678 */
//{SECT_NO_2,BLOCK_NO_1,STORE_BakMoney},		/* ����2����1���洢������� 	����Ĭ�ϣ�601552985678 */
//};

/* ����ṹ */
//typedef struct
//{
//	uint8 Pin[6];/* ESAM��RF������ */
//	uint8 SectNo;/* ����RF�� */
//	uint8 BlockNo;/* ����RF��*/
//}CARD_PWD_STRUCT;

//static uint32 gReaderUartHandler;
#define CARD_BAUD		        115200
#define OPER_CARD_WAITTIME  200     /*2��*/
#define UALOCK_TIMES        100

/* ��ʼ������������*/
void TskReader_InitCom(void)
{
	int32 tmp;
	UartHandler[CARD_COM] = Uart_Open(CARD_COM);
	//gReaderUartHandler = UartHandler[CARD_COM];
	
 	tmp  = CARD_BAUD;
	Uart_IOCtrl(UartHandler[CARD_COM], SIO_BAUD_SET, &tmp);	

	tmp  = USART_Parity_No;
	Uart_IOCtrl(UartHandler[CARD_COM], SIO_PARITY_BIT_SET, &tmp);

	tmp  = USART_StopBits_1;
	Uart_IOCtrl(UartHandler[CARD_COM], SIO_STOP_BIT_SET, &tmp);	
	
	tmp = USART_WordLength_8b;
	Uart_IOCtrl(UartHandler[CARD_COM], SIO_DATA_BIT_SET, &tmp);	
}

typedef struct
{
	uint8 sect0[6];
	uint8 sect1[6];
	uint8 sect2[6];
	uint8 usrpwd[6];
}CARD_PWD_ST;

const CARD_PWD_ST conCardPwd = \
{
	{0x89,0x25,0x51,0x06,0x33,0x33},
  {0x89,0x25,0x51,0x06,0x33,0x33},
	{0x60,0x15,0x52,0x98,0x56,0x78},
  {0x11,0x11,0x11,0x11,0x11,0x11},
};

const uint8 conTaskPrio[] = {APP_TASK_AGUNMAINCTRL_PRIO,APP_TASK_BGUNMAINCTRL_PRIO};

void TskReader_InitCardData(void)
{
	PARAM_DEV_TYPE *param = ChgData_GetDevParamPtr();
	//PARAM_OPER_TYPE *paramOper = ChgData_GetRunParamPtr();
	
	memset(&gCardOperData,0,sizeof(CARD_OPER_DATA));
	memcpy(gCardOperData.para.sect0pwd,conCardPwd.sect0,sizeof(gCardOperData.para.sect0pwd));
  memcpy(gCardOperData.para.sect1pwd,conCardPwd.sect1,sizeof(gCardOperData.para.sect1pwd));
  memcpy(gCardOperData.para.sect2pwd,conCardPwd.sect2,sizeof(gCardOperData.para.sect2pwd));
	//memcpy(gCardOperData.para.keyCode,conKey,sizeof(param->card.safe.keyCode));  /*������޸�*/
	memcpy(gCardOperData.para.keyCode,param->card.safe.keyCode,sizeof(param->card.safe.keyCode)); 
  memcpy(gCardOperData.para.defautusrpwd,conCardPwd.usrpwd,sizeof(gCardOperData.para.defautusrpwd));
}




uint8 TskCReader_CpyLockCardNo(uint8 * CardNo)
{
	memcpy(gCardOperData.lkcardNo,CardNo,sizeof(gCardOperData.lkcardNo));
	return sizeof(gCardOperData.lkcardNo);
}

/*��ȡ����Ϣ*/
uint8 TskCReader_GetCardInfo(void)
{
	 uint8 data[16];
	 CARD_PWD_STRUCT sectorpara;
	 PARAM_COMM_TYPE *BackCOMM = ChgData_GetCommParaPtr();    //patli 20190923 add
	
	 //�쿨
		if( RP_SUCC != RF_CheckChgCard(CARD_COM) ) {
			  return ERR_NOCARD;
		}
	 
	 /*��ȡ�û�����*/
	 sectorpara.SectNo = 0;
	 sectorpara.BlockNo = 2;
	 memcpy(sectorpara.Pin,gCardOperData.para.sect0pwd,6);	

	 if( RP_SUCC == RF_ReadSectorData(CARD_COM,&sectorpara,data) ){
			 memcpy(gCardOperData.info.userPin,data,sizeof(gCardOperData.info.userPin));
	 }else{
			 if( RP_SUCC == RF_ReadSectorData(CARD_COM,&sectorpara,data) ){
				 memcpy(gCardOperData.info.userPin,data,sizeof(gCardOperData.info.userPin));
			 }
			 else{
				return ERR_READ;
			 }
	 }
	 /*��ȡ���š�������־�����*/
	 /*��ȡ����*/
	 sectorpara.SectNo = 1;
	 sectorpara.BlockNo = 0;
	 memcpy(sectorpara.Pin,gCardOperData.para.sect0pwd,6);	
	 if( RP_SUCC == RF_ReadSectorData(CARD_COM,&sectorpara,data) ) {
		 memcpy(gCardOperData.info.cardNo,data,16);
		 if( 0 == memcmp(conManagerCard,gCardOperData.info.cardNo,8)) {
			    gCardOperData.info.cardtype = E_MANG_CARD;
					return ERR_MANGERCARD;
			}
	 } else {
		 if( RP_SUCC == RF_ReadSectorData(CARD_COM,&sectorpara,data) ) {
				 memcpy(gCardOperData.info.cardNo,data,16);
				 if( 0 == memcmp(conManagerCard,gCardOperData.info.cardNo,8)) {
							gCardOperData.info.cardtype = E_MANG_CARD;
							return ERR_MANGERCARD;
					}
			 }
		  else{
			  return ERR_READ;
			}
	 }	 

	 
	 /*��ȡKey�ܳ�*/
		sectorpara.SectNo = 2;
	  sectorpara.BlockNo = 0;
	  memcpy(sectorpara.Pin,gCardOperData.para.sect2pwd,6);	
		if( RP_SUCC == RF_ReadSectorData(CARD_COM,&sectorpara,data)){
			  if( memcmp(data,gCardOperData.para.keyCode,sizeof(data)) == 0 ) {
					gCardOperData.info.cardtype = E_USER_CARD; 
				}else {
					/*�Ƿ���*/
					 return ERR_KEY;
				}
		}else{ 
			  if( RP_SUCC == RF_ReadSectorData(CARD_COM,&sectorpara,data)){
						if( memcmp(data,gCardOperData.para.keyCode,sizeof(data)) == 0 ) {
							gCardOperData.info.cardtype = E_USER_CARD; 
						}else {
							/*�Ƿ���*/
							 return ERR_KEY;
						}
				}
				else{
	        return ERR_READ;
				}
		}
		
	 /***���������жϣ���һ���ַ���8�ּ��ǹ��� patli 20190926*****/
	 if((gCardOperData.info.cardNo[0] & 0xf0) == 0x80) {
		gCardOperData.info.cardtype = E_SUPER_CARD;
	 }else if((gCardOperData.info.cardNo[0] & 0xf0) == 0x70){
	 	gCardOperData.info.cardtype = E_OFFLINE_USER_CARD;
	 }
	 /********************************************************/	
		
	 PARAM_DEV_TYPE *ptrDevPara = ChgData_GetDevParamPtr();
	 if(  ptrDevPara->onlinecard != E_ONLINE_CARD ) {
				/*��ȡ��־*/
			 sectorpara.SectNo = 1;
			 sectorpara.BlockNo = 1;
			 memcpy(sectorpara.Pin,gCardOperData.para.sect0pwd,6);	
			 if( RP_SUCC == RF_ReadSectorData(CARD_COM,&sectorpara,data) ){
				 gCardOperData.info.lockFlag = data[0] & 0x01;
			 }
			 else {
					 if( RP_SUCC == RF_ReadSectorData(CARD_COM,&sectorpara,data) ){
						 gCardOperData.info.lockFlag = data[0] & 0x01;
					 }
					 else{
						 return ERR_READ;
					 }
			 }
			 
				/*��ȡ���*/
			 sectorpara.SectNo = 1;
			 sectorpara.BlockNo = 2;
			 memcpy(sectorpara.Pin,gCardOperData.para.sect0pwd,6);	
			 if( RP_SUCC == RF_ReadSectorData(CARD_COM,&sectorpara,data) ) {
					memcpy(&gCardOperData.info.money,data,sizeof(gCardOperData.info.money));
			 }
			 else{
					if( RP_SUCC == RF_ReadSectorData(CARD_COM,&sectorpara,data) ) {
						memcpy(&gCardOperData.info.money,data,sizeof(gCardOperData.info.money));
					}
					else{
						return ERR_READ;
					}
			 }
   }
	 return RP_SUCC;
}

/*��������Ϣ��������ȡ������Ϣ������ʱ�価����1S��*/
uint8 TskReader_LockCard(uint8 *CardNo,int32 bakmoney)
{
	 uint8 data[16] = {0};
	 CARD_PWD_STRUCT sectorpara;
   
	 /*д���������*/
	 sectorpara.SectNo = 2;
	 sectorpara.BlockNo = 1;
	 memcpy(sectorpara.Pin,gCardOperData.para.sect2pwd,6);
	 
	 memcpy(data,&bakmoney,sizeof(bakmoney));
	 if (RP_SUCC != RF_WriteSectorData(CARD_COM,&sectorpara,data)) {
		  return ERR_READ;
	 }
	 /*д������־*/
	 sectorpara.SectNo = 1;
	 sectorpara.BlockNo = 1;
	 memcpy(sectorpara.Pin,gCardOperData.para.sect1pwd,6);
	 memset(data,0,sizeof(data));
	 data[0] = 0x01;
	 
	 if (RP_SUCC != RF_WriteSectorData(CARD_COM,&sectorpara,data)) {
		  return ERR_READ;
	 }
	 return RP_SUCC;
}

/* �۷ѽ���*/
uint8 TskReader_Account(uint8 *CardNo,int32 decmoney)
{
	 uint8 data[16] = {0};
	 CARD_PWD_STRUCT sectorpara;
	
	 if( RP_SUCC != TskCReader_GetCardInfo() ) {
		  return ERR_READ;
	 }
	 
	 /*��û�б���*/
//	 if( (gCardOperData.info.lockFlag & 0x01) != 0x01) {
//		 return ERR_ERRCARD;
//	 }
	 
	 /*ȷ�Ͽ����Ƿ���Ҫ�۷ѵĿ���*/
	 if( memcmp(CardNo,gCardOperData.info.cardNo,sizeof(gCardOperData.info.cardNo)) != 0 ){
		  return ERR_ERRCARD;
	 }
	 
	 /*�۷�*/
	 sectorpara.SectNo = 1;
	 sectorpara.BlockNo = 2;
	 memcpy(sectorpara.Pin,gCardOperData.para.sect1pwd,6);

	 
	 //decmoney = decmoney > gCardOperData.info.money + 500 ? gCardOperData.info.money + 500 : decmoney;
	  /*�������Ϊ����ʱ ��Ǯ�����Ǹ�����0�������޷�������ȷ*/
	 if(decmoney > 0 && decmoney < 500000){
			if ( RP_SUCC !=  RF_DecMoney(CARD_COM,&sectorpara,decmoney) ) {		 
				return ERR_READ;
		 }
	 }
	 
	 /*��ȡ���š�������־�����*/
	 /*��ȡ����*/
	 sectorpara.SectNo = 1;
	 sectorpara.BlockNo = 0;
	 memcpy(sectorpara.Pin,gCardOperData.para.sect0pwd,6);	
	 if( RP_SUCC == RF_ReadSectorData(CARD_COM,&sectorpara,data) ) {
		;
	 }else {
		  return ERR_READ;
	 }

	 
	 /*���������־*/
	 sectorpara.SectNo = 1;
	 sectorpara.BlockNo = 1;
	 memcpy(sectorpara.Pin,gCardOperData.para.sect1pwd,6);
	 
	 memset(data,0,sizeof(data));
	 if (RP_SUCC != RF_WriteSectorData(CARD_COM,&sectorpara,data)) {
		  return ERR_READ;
	 }
	 
	 return RP_SUCC;
}

/*�رջ��������쿨 */
void TskReader_SetCardOper(uint8 fg,uint32 delaytime)
{
  if( 0 == fg ) {
      gCardOperData.checkFg = 0;
			gCardOperData.delaytime = delaytime+GetSystemTick()+TIM_1S;//���ֹͣ1s
	}else {
			gCardOperData.checkFg = 1;
			gCardOperData.delaytime = delaytime+GetSystemTick()+TIM_1S;//���ֹͣ1s
	}
}

void CardReader_MsgProc(MSG_STRUCT *msg)
{
	uint8 tmp,data[6];
	stoc_u tmps2c = { 0 };
	uint16 checkdelaytime = 0;
	PARAM_DEV_TYPE *ptrDevPara = ChgData_GetDevParamPtr();
	  
	switch(msg->MsgType)
	{
		case MSG_SELECT_GUNCARD:
		{
			gCardOperData.u8Active = msg->MsgData[0];
			gCardOperData.delaytime = 0;
			gCardOperData.checkFg = 1;
			break;
		}
		case MSG_CHECK_CARD:
		{
			gCardOperData.lockFg = 0;
			gCardOperData.unLockFg = 0;
						
			if( 0 == gCardOperData.checkFg ) {
				gCardOperData.delaytime = 0;
				gCardOperData.checkFg = 1;
			}
						
			if (0 == msg->MsgLenth){
				gCardOperData.delaytime = GetSystemTick() + 10;
			}
			else{
				if (1 == msg->MsgData[0]) {
					tmps2c.c[0] = msg->MsgData[1];
					tmps2c.c[1] = msg->MsgData[2];
					checkdelaytime = tmps2c.s;
					gCardOperData.delaytime = GetSystemTick() + checkdelaytime;
				}
			}
			break;
		}
		case MSG_LOCKED_CARD:
		{
			gCardOperData.lockFg = 1;
			gCardOperData.unLockFg = 0;
			gCardOperData.u8Active = msg->MsgData[0];
			if(  ptrDevPara->onlinecard == E_ONLINE_CARD ) {
				tmp = RP_SUCC; 										 
			}
			else{
				tmp = TskReader_LockCard(gCardOperData.info.cardNo,gCardOperData.info.money);
			}
			if( tmp== RP_SUCC ) {
				data[0] = 1;
				data[1] = RES_SUCC; /*�����ɹ�*/
				gCardOperData.lockFg = 0;
				gCardOperData.unLockFg = 0; 
				SendMsgWithNByte(MSG_LOCK_RESULT ,2,&data[0],conTaskPrio[gCardOperData.u8Active]);	
				Delay10Ms(5);
				TskReader_SetCardOper(1,TIM_5S);						
			}else {
				gCardOperData.lockFg = 1;
				gCardOperData.unLockFg = 0;
			}
		}
		break;
		case MSG_UNLOCK_CARD:
		{/*������*/
			u32toc_u money;
			gCardOperData.unLockFg =  1;
			gCardOperData.lockFg = 0;
			gCardOperData.u8Active = msg->MsgData[0];
						
			money.c[0] = msg->MsgData[1];  /*�⿨��Ϣ��ʽ��byte[0] ��������־ byte[1..4] ��� */
			money.c[1] = msg->MsgData[2];
			money.c[2] = msg->MsgData[3];
			money.c[3] = msg->MsgData[4];
			gCardOperData.unlockmoney = money.i;
			if(  ptrDevPara->onlinecard == E_ONLINE_CARD ) {
				tmp = RP_SUCC; 										 
			}
			else{
				tmp = TskReader_Account(gCardOperData.lkcardNo,money.i);
			}
			if( tmp== RP_SUCC ) {
				data[0] = 0;
				data[1] = RES_SUCC; /*�����ɹ�*/
				data[2] = msg->MsgData[0]; /*ǹ��*/				
				gCardOperData.unlockmoney = 0;
				gCardOperData.unLockFg = 0;
				memset(gCardOperData.lkcardNo,0,sizeof(gCardOperData.lkcardNo));
				SendMsgWithNByte(MSG_UNLOCK_RESULT ,3,&data[0],conTaskPrio[gCardOperData.u8Active]);	
				Delay10Ms(5);
				TskReader_SetCardOper(1,TIM_5S);
				break;	
			}else {
				gCardOperData.lockFg = 0;
				gCardOperData.unLockFg = 1; 
								
			}							
		}
		break;
					
		default:
		break;
	} 	

}

void CardReader_MsgRecv(void)
{
	MSG_STRUCT msg= { 0 };
		
	memset(&msg,0,sizeof(MSG_STRUCT));
	if(RcvMsgFromQueue(&msg) == TRUE)
	{
		CardReader_MsgProc(&msg);
   	}
}

uint8 TaskCard_CheckSelect(void)
{
	DEV_LOGIC_TYPE *ptrAgunLogic = ChgData_GetLogicDataPtr(AGUN_NO);
	DEV_LOGIC_TYPE *ptrBgunLogic =ChgData_GetLogicDataPtr(BGUN_NO);
	CALCBILL_DATA_ST *ptrBillA = Bill_GetBillData(AGUN_NO);
	CALCBILL_DATA_ST *ptrBillB = Bill_GetBillData(BGUN_NO);
	
	if( ptrAgunLogic->workstep != STEP_IDEL ) {
		if( memcmp(ptrBillA->cardNo,gCardOperData.info.cardNo,sizeof(gCardOperData.info.cardNo)) == 0 ){
			gCardOperData.u8Active = AGUN_NO;
		}
	}
	if(ptrBgunLogic->workstep != STEP_IDEL) {
		if( memcmp(ptrBillB->cardNo,gCardOperData.info.cardNo,sizeof(gCardOperData.info.cardNo)) == 0 ){
			gCardOperData.u8Active = BGUN_NO;
		}
	}
	return gCardOperData.u8Active % DEF_MAX_GUN_NO;
}

void TskReader_Init()
{
	TskReader_InitCardData();
	TskReader_InitCom();
	
	gCardOperData.delaytime = GetSystemTick();
	
//	Message_QueueCreat(APP_TASK_CARD_PRIO);
	
	gCardOperData.checkFg = 1;//ˢ�����ʹ��
	gCardOperData.delaytime = 0;

}

void TaskCardReader_main_proc()
{
	uint8 u8Tmp;
	uint8 msg[4];
	uint16 errCnt[2];
	
	CHARGE_TYPE  *PtrRunData = ChgData_GetRunDataPtr(gCardOperData.u8Active%DEF_MAX_GUN_NO);
	memset(errCnt,0,2);

#ifndef SCREEN_CARD
	CardReader_MsgRecv();
#endif

	if( 1 == gCardOperData.lockFg ) 
	{
		gCardOperData.unLockFg = 0;
		u8Tmp = TskReader_LockCard(gCardOperData.info.cardNo,gCardOperData.info.money);
		if( u8Tmp== RP_SUCC ) 
		{
			errCnt[0] = 0;
			msg[0] = 1;
			msg[1] = RES_SUCC; /*�����ɹ�*/
			SendMsgWithNByte(MSG_LOCK_RESULT ,2,&msg[0],conTaskPrio[gCardOperData.u8Active]);  
			gCardOperData.lockFg = 0;
			TskReader_SetCardOper(1,TIM_5S);
			Delay10Ms(1);
		}
		else 
		{
		 	if( errCnt[0]++ > UALOCK_TIMES )
			{
				errCnt[0] = 0;
				gCardOperData.lockFg = 0;
				msg[0] = 1;
				msg[1] = RES_FAIL; 
				SendMsgWithNByte(MSG_LOCK_RESULT ,2,&msg[0],conTaskPrio[gCardOperData.u8Active]);   
			}
			Delay10Ms(1);
//			continue;
			return;
		}				
	}
	else if( 1 == gCardOperData.unLockFg ) 
	{
	
		gCardOperData.lockFg = 0;
		u8Tmp = TskReader_Account(gCardOperData.lkcardNo,gCardOperData.unlockmoney);
		if( u8Tmp == RP_SUCC )
		{
			errCnt[1] = 0;
			msg[0] = 0;
			msg[1] = RES_SUCC; /*�����ɹ�*/
			SendMsgWithNByte(MSG_UNLOCK_RESULT ,2,&msg[0],conTaskPrio[gCardOperData.u8Active]);   
		   	gCardOperData.unLockFg = 0;
			TskReader_SetCardOper(1,TIM_5S);
			   
			gCardOperData.unlockmoney = 0;
			memset(gCardOperData.lkcardNo,0,sizeof(gCardOperData.lkcardNo));
			Delay10Ms(1);
		}
		else 
		{
			if( errCnt[1]++ > UALOCK_TIMES ) 
			{
				errCnt[1] = 0;
				gCardOperData.unLockFg = 0;
				gCardOperData.unlockmoney = 0;
				memset(gCardOperData.lkcardNo,0,sizeof(gCardOperData.lkcardNo));
				msg[0] = 1;
				msg[1] = RES_FAIL; 
				SendMsgWithNByte(MSG_UNLOCK_RESULT ,2,&msg[0],conTaskPrio[gCardOperData.u8Active]); 
			}
			Delay10Ms(1);
			//	continue;
			return;
		}
	}
	
	
	if( (gCardOperData.delaytime <  GetSystemTick() ) && ( 1 == gCardOperData.checkFg) )
	{
		//��ȡ����Ϣ
	   	u8Tmp = TskCReader_GetCardInfo();
		if( RP_SUCC == u8Tmp ) 
		{
			u8Tmp = TaskCard_CheckSelect() % DEF_MAX_GUN_NO;
			msg[0] = 1;
			TskReader_SetCardOper(1,5*TIM_1S); 
			SendMsgWithNByte(MSG_CARD_INFO,1,&msg[0],conTaskPrio[gCardOperData.u8Active]);
			Delay10Ms(1);
		}
		else if( ERR_MANGERCARD == u8Tmp )
		{
			msg[0] = 1;
			TskReader_SetCardOper(1,3*TIM_1S); 
			SendMsgWithNByte(MSG_CARD_INFO,1,&msg[0],conTaskPrio[gCardOperData.u8Active]);
			Delay10Ms(1);
		}
		else 
		{
			if( ERR_NOCARD == u8Tmp ) 
			{
				TskReader_SetCardOper(1,0*TIM_1S); 
			}
			else if ( ERR_READ == u8Tmp ) 
			{
				TskReader_SetCardOper(1,0*TIM_1S); 
				/*������Ϣ����*/
				if( STEP_IDEL == PtrRunData->logic->workstep ) 
				{
					msg[1] = CARD_PASSWD_ERR;
					SendMsgWithNByte(MSG_VERIFY_RESULT,2,&msg[0],conTaskPrio[gCardOperData.u8Active]);
				}
						
			}
			else if(ERR_KEY == u8Tmp) 
			{
				/*�Ƿ���*/
				TskReader_SetCardOper(1,2*TIM_1S); 
				if( STEP_IDEL == PtrRunData->logic->workstep ) 
				{
					msg[1] = CARD_ILLEGAL_ERR;
					SendMsgWithNByte(MSG_VERIFY_RESULT,2,&msg[0],conTaskPrio[gCardOperData.u8Active]);
				}
				Delay10Ms(20);
			}
			else
			{
				TskReader_SetCardOper(1,2*TIM_1S);  
			}
		}
	}


}

/*����������*/
void TaskCardReader(void *p_arg)
{
  uint8 u8Tmp;
	uint8 msg[4];
	uint16 errCnt[2];

  CHARGE_TYPE  *PtrRunData = ChgData_GetRunDataPtr(gCardOperData.u8Active%DEF_MAX_GUN_NO);
	memset(errCnt,0,2);
	
	TskReader_InitCardData();
	TskReader_InitCom();
	
	gCardOperData.delaytime = GetSystemTick();
	
	Message_QueueCreat(APP_TASK_CARD_PRIO);
	
	gCardOperData.checkFg = 1;//ˢ�����ʹ��
	gCardOperData.delaytime = 0;
	
	while(1)
	{
		TaskRunTimePrint("TaskCardReader begin", OSPrioCur);

#if 1
	TaskCardReader_main_proc();
#else
		CardReader_MsgDeal();
		
		if( 1 == gCardOperData.lockFg ) {
			 gCardOperData.unLockFg = 0;
			 u8Tmp = TskReader_LockCard(gCardOperData.info.cardNo,gCardOperData.info.money);
			 if( u8Tmp== RP_SUCC ) {
				 errCnt[0] = 0;
				 msg[0] = 1;
				 msg[1] = RES_SUCC; /*�����ɹ�*/
				 SendMsgWithNByte(MSG_LOCK_RESULT ,2,&msg[0],conTaskPrio[gCardOperData.u8Active]);	
				 gCardOperData.lockFg = 0;
				 TskReader_SetCardOper(1,TIM_5S);
				 Delay10Ms(1);
			 }else {
				  if( errCnt[0]++ > UALOCK_TIMES ) {
						errCnt[0] = 0;
						gCardOperData.lockFg = 0;
						msg[0] = 1;
						msg[1] = RES_FAIL; 
						SendMsgWithNByte(MSG_LOCK_RESULT ,2,&msg[0],conTaskPrio[gCardOperData.u8Active]);	
					}
					Delay10Ms(1);
				  continue;
			 }				 
		}else if ( 1 == gCardOperData.unLockFg ) {
			gCardOperData.lockFg = 0;
			u8Tmp = TskReader_Account(gCardOperData.lkcardNo,gCardOperData.unlockmoney);
		  if( u8Tmp == RP_SUCC ) {
				errCnt[1] = 0;
			  msg[0] = 0;
			  msg[1] = RES_SUCC; /*�����ɹ�*/
			  SendMsgWithNByte(MSG_UNLOCK_RESULT ,2,&msg[0],conTaskPrio[gCardOperData.u8Active]);	
		    gCardOperData.unLockFg = 0;
				TskReader_SetCardOper(1,TIM_5S);
				
				gCardOperData.unlockmoney = 0;
				memset(gCardOperData.lkcardNo,0,sizeof(gCardOperData.lkcardNo));
				Delay10Ms(1);
		  }else {
				 if( errCnt[1]++ > UALOCK_TIMES ) {
					  errCnt[1] = 0;
						gCardOperData.unLockFg = 0;
					  gCardOperData.unlockmoney = 0;
					  memset(gCardOperData.lkcardNo,0,sizeof(gCardOperData.lkcardNo));
					  msg[0] = 1;
						msg[1] = RES_FAIL; 
						SendMsgWithNByte(MSG_UNLOCK_RESULT ,2,&msg[0],conTaskPrio[gCardOperData.u8Active]);	
					}
				 Delay10Ms(1);
				 continue;
			}
		}
	
		if( (gCardOperData.delaytime <  GetSystemTick() ) && ( 1 == gCardOperData.checkFg) ){
				//��ȡ����Ϣ
				u8Tmp = TskCReader_GetCardInfo();
				if( RP_SUCC == u8Tmp ) {
					 u8Tmp = TaskCard_CheckSelect() % DEF_MAX_GUN_NO;
					 msg[0] = 1;
					 TskReader_SetCardOper(1,5*TIM_1S); 
					 SendMsgWithNByte(MSG_CARD_INFO,1,&msg[0],conTaskPrio[gCardOperData.u8Active]);
					 Delay10Ms(1);
				}else if( ERR_MANGERCARD == u8Tmp ){
					 msg[0] = 1;
					 TskReader_SetCardOper(1,3*TIM_1S); 
					 SendMsgWithNByte(MSG_CARD_INFO,1,&msg[0],conTaskPrio[gCardOperData.u8Active]);
					 Delay10Ms(1);
				}else {
					if( ERR_NOCARD == u8Tmp ) {
						 TskReader_SetCardOper(1,0*TIM_1S);	
					}else if ( ERR_READ == u8Tmp ) {
						 TskReader_SetCardOper(1,0*TIM_1S);	
						 /*������Ϣ����*/
						 if( STEP_IDEL == PtrRunData->logic->workstep ) {
								msg[1] = CARD_PASSWD_ERR;
								SendMsgWithNByte(MSG_VERIFY_RESULT,2,&msg[0],conTaskPrio[gCardOperData.u8Active]);
						 }
						 
					}else if(ERR_KEY == u8Tmp) {
						/*�Ƿ���*/
						 TskReader_SetCardOper(1,2*TIM_1S);	
						 if( STEP_IDEL == PtrRunData->logic->workstep ) {
								msg[1] = CARD_ILLEGAL_ERR;
								SendMsgWithNByte(MSG_VERIFY_RESULT,2,&msg[0],conTaskPrio[gCardOperData.u8Active]);
						 }
						 Delay10Ms(20);
					}else {
						TskReader_SetCardOper(1,2*TIM_1S);	
					}
			}
	 }
#endif	 
	 Delay10Ms(10);  //patli 20200106 Delay5Ms(5);

	 TaskRunTimePrint("TaskCardReader end", OSPrioCur);
	 
	}
}

#else
CARD_OPER_DATA gCardOperData;

uint8 TskCReader_CpyLockCardNo(uint8 * CardNo)
{
	return 0;
}

CARD_INFO *TskCard_GetCardInfPtr(void)
{
	return NULL;
}

#endif





