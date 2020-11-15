/**
  ******************************************************************************
  * @file    relayOut.c
  * @author  zqj
  * @version v1.0
  * @date    2018-04-09
  * @brief   
  ******************************************************************************
	*/
#include "main.h"
#include "gpio.h"
	
#include "ChgData.h"

#include "RelayOut.h"
	
//extern CHARGE_TYPE *ChgData_GetRunDataPtr(void);

DEV_RELAY_TYPE gRelay;

DEV_RELAY_TYPE * Relay_GetRelayDataPtr(uint8 no)
{
	return &gRelay;
}




/* ���������� */
uint8 RelayOut_ALocked(uint8 fg)
{
	DEV_LOGIC_TYPE *ptrStu =  ChgData_GetLogicDataPtr(AGUN_NO);
	uint8 i;
	 static uint16 sCnt = 0;
	 static uint8 sFirstFg = 0;
	 if( CTRL_ON == fg ) {
		 /*��������*/
		 sFirstFg = 0;
		 WriteJoutputDataBit(JOUT_AELOCKKM,(BitAction)fg); 
		 Delay10Ms(20);
		 ((CHARGE_TYPE *)ChgData_GetRunDataPtr(AGUN_NO))->gun->statu.bits.elockctrl = CTRL_ON;
	 }else {
		 /*����*/
		 i = 10;
		 while( i-- ) {
			 if( 0 == sFirstFg ) {
					sCnt = 0;
					sFirstFg = 1;
					WriteJoutputDataBit(JOUT_AELOCKKM,(BitAction)1); //����һ��
					Delay10Ms(100);
					WriteJoutputDataBit(JOUT_AELOCKKM,(BitAction)fg);
					Delay10Ms(20);
			 }else {
				 WriteJoutputDataBit(JOUT_AELOCKKM,(BitAction)fg);
//				    Delay5Ms(1);		//patli 20200220 60 to 1
					Delay10Ms(10);      //zyf 20201107 5ms to 100ms
					if(ptrStu->runstu.bits.elockstu == CTRL_ON){//CTRL_ON Ϊ��
					  if( sCnt++ > 3 ) {
							sFirstFg = 0;
						}
					}
					else {
							break;
					}
			 }
			 Delay5Ms(1); 
		}
		 ((CHARGE_TYPE *)ChgData_GetRunDataPtr(AGUN_NO))->gun->statu.bits.elockctrl = CTRL_OFF;
	 }
	 return fg;
}

/* ���������� */
uint8 RelayOut_BLocked(uint8 fg)
{
	DEV_LOGIC_TYPE *ptrStu =  ChgData_GetLogicDataPtr(BGUN_NO);
	uint8 i;
	 static uint16 sCnt = 0;
	 static uint8 sFirstFg = 0;
	 if( CTRL_ON == fg ) {
		 /*��������*/
		 sFirstFg = 0;
		 WriteJoutputDataBit(JOUT_BELOCKKM,(BitAction)fg); 
		 Delay10Ms(20);
		 ((CHARGE_TYPE *)ChgData_GetRunDataPtr(BGUN_NO))->gun->statu.bits.elockctrl = CTRL_ON;
	 }else {
		 /*����*/
		 i = 10;
		 while( i-- ) {
			 if( 0 == sFirstFg ) {
					sCnt = 0;
					sFirstFg = 1;
					WriteJoutputDataBit(JOUT_BELOCKKM,(BitAction)1); //����һ��
					Delay10Ms(100);
					WriteJoutputDataBit(JOUT_BELOCKKM,(BitAction)fg);
					Delay10Ms(20);
			 }else {
					WriteJoutputDataBit(JOUT_BELOCKKM,(BitAction)fg);
					Delay5Ms(1);		//patli 20200220 60 to 1
					//					Delay10Ms(1);	   //patli 20200220 60 to 1
				  if(ptrStu->runstu.bits.elockstu == CTRL_ON){//CTRL_ON Ϊ��
					  if( sCnt++ > 3 ) {
							sFirstFg = 0;
						}
					}
					else {
							break;
					}
			 }
			 Delay5Ms(1); 
		 }
		 ((CHARGE_TYPE *)ChgData_GetRunDataPtr(BGUN_NO))->gun->statu.bits.elockctrl = CTRL_OFF;
	 }
	 return fg;
}


/* ���������� */
uint8 RelayOut_Locked(uint8 gunNo,uint8 fg)
{
if(fg == CTRL_ON)
	DebugInfoByCon("������:��");
else
	DebugInfoByCon("������:��");


	if( gunNo == 0 ) {
		 RelayOut_ALocked(fg);
	}else if( gunNo == 1 ) {
		 RelayOut_BLocked(fg);
	}
	return fg;
}

// 	JOUT_ACKM   = 0,
//	JOUT_XFKM   = 1,
//	JOUT_ERRLED = 2,
//	JOUT_CHGLED = 3,
//	JOUT_BMSPWKM= 4,
//	JOUT_AELOCKKM= 5,
//	JOUT_DCKMP  = 6,
//	JOUT_DCKMN  = 7,
/***************** 
BMS�����Դ�л�
fg=0:12V
fg=1:24V 
******************/
/*BMS��Դ��־��ʼ��*/
void RelayOut_Init(void)
{	
	memset(&gRelay,0,sizeof(DEV_RELAY_TYPE));
}
/*BMS��Դ����*/
uint8 RelayOut_PowerSet(uint8 gunNo)
{
	uint8 fg = BMS_12V;
	if( AGUN_NO == gunNo ) {
		if(BMS_24V == gRelay.statu.bits.apow){
			fg = BMS_24V;
		}
		WriteJoutputDataBit(JOUT_ABMSPOWER,(BitAction)fg);
	}else if(BGUN_NO == gunNo) {
		if(BMS_24V == gRelay.statu.bits.bpow){
			fg = BMS_24V;
		}
		WriteJoutputDataBit(JOUT_BBMSPOWER,(BitAction)fg);
	}
//patli 20200310	Delay10Ms(20);
	
	return fg;
}
/*BMS��Դ��־����*/
uint8 RelayOut_Power12_24V(uint8 gunNo,uint8 fg)
{	
	if( AGUN_NO == gunNo ) {
		gRelay.statu.bits.apow = (BitAction)fg;
	}else if(BGUN_NO == gunNo) {
		gRelay.statu.bits.bpow = (BitAction)fg;
	}
	return fg;
}
/*��ȡBMS��Դ��־*/
uint8 RelayOut_GetBmsPower(uint8 gunNo)
{
	uint8 fg = BMS_12V;
	if( AGUN_NO == gunNo ) {
		if(BMS_24V == gRelay.statu.bits.apow){
			fg = BMS_24V;
		}
	}else if(BGUN_NO == gunNo) {
    if(BMS_24V == gRelay.statu.bits.bpow){
			fg = BMS_24V;
		}
	}
	return fg;
}
/* BMS�����Դ */
uint8 RelayOut_AssistPower(uint8 gunNo,uint8 onoff)
{
    if(onoff == ASSIST_POWER_ON)
		DebugInfoByCon("BMS�����Դ:��");
	else
		DebugInfoByCon("BMS�����Դ:��");


//	RelayOut_PowerSet(gunNo);
	if( AGUN_NO == gunNo ) {
		WriteJoutputDataBit(JOUT_ABMSPWKM,(BitAction)onoff);
	}else if(BGUN_NO == gunNo) {
		WriteJoutputDataBit(JOUT_BBMSPWKM,(BitAction)onoff);
	}
//patli 20200226		Delay10Ms(20);
	
	return onoff;
}
/* ���ֱ���Ӵ��� */
uint8 RelayOut_DcKmOut(uint8 gunNo,uint8 onoff)
{
    if(onoff == KM_ON)
		DebugInfoByCon("���ֱ���Ӵ���:��");
	else
		DebugInfoByCon("���ֱ���Ӵ���:��");


	if( AGUN_NO == gunNo ) {
		WriteJoutputDataBit(JOUT_ADCKMP,(BitAction)onoff);
		Delay10Ms(20);
//		WriteJoutputDataBit(JOUT_ADCKMN,(BitAction)onoff);    //�泵ֻ��һ������̵���
//		Delay10Ms(20);
	}else if( BGUN_NO == gunNo ) {
		WriteJoutputDataBit(JOUT_BDCKMP,(BitAction)onoff);
		Delay10Ms(20);
		WriteJoutputDataBit(JOUT_BDCKMN,(BitAction)onoff);
		Delay10Ms(20);
	}
	
	return onoff;
}

/* 2��ģ������Ӵ���*/
uint8 RelayOut_DcConTactKmOut(uint8 onoff)
{

    if(onoff == KM_ON)
		DebugInfoByCon("ģ������Ӵ���:��");
	else
		DebugInfoByCon("ģ������Ӵ���:��");

	WriteJoutputDataBit(JOUT_TACKKM,(BitAction)onoff);
	Delay10Ms(20);
	return onoff;
}


/* ���ȽӴ��� */
uint8 RelayOut_AcKmOut(uint8 onoff)  
{
    if(onoff == JOUT_ON)
		DebugInfoByCon("���ȽӴ���:��");
	else
		DebugInfoByCon("���ȽӴ���:��");


	WriteJoutputDataBit(JOUT_ACKM,(BitAction)onoff);
	Delay10Ms(20);
	
	return onoff;
}

/* й�ŵ��� */
uint8 RelayOut_XfResOnOff(uint8 gunNo,uint8 onoff)
{
    if(onoff == CTRL_ON)
		DebugInfoByCon("й��:��");
	else
		DebugInfoByCon("й��:��");


	if(AGUN_NO == gunNo ) {
		WriteJoutputDataBit(JOUT_AXFKM,(BitAction)onoff);
	}else if(BGUN_NO == gunNo ) {
		WriteJoutputDataBit(JOUT_BXFKM,(BitAction)onoff);
	}
	Delay10Ms(20);
	
  return onoff;
}

/*����ָʾ�� */
uint8 RelayOut_RunChargeLed(uint8 gunNo,uint8 onoff)
{
	if(AGUN_NO == gunNo ) {
		WriteJoutputDataBit(JOUT_ACHGLED, (BitAction)onoff);
	}else if(BGUN_NO == gunNo ) {
		WriteJoutputDataBit(JOUT_BCHGLED, (BitAction)onoff);
	}
	Delay10Ms(20);
	
	return onoff;
}

/* ����ָʾ�� */
uint8 RelayOut_BreakdownLed(uint8 gunNo,uint8 onoff)
{
	if(AGUN_NO == gunNo ) {
		WriteJoutputDataBit(JOUT_AERRLED, (BitAction)onoff);
	}else if(BGUN_NO == gunNo ) {
		WriteJoutputDataBit(JOUT_BERRLED, (BitAction)onoff);
	}
//patli 20200220	Delay10Ms(20);
	
	return onoff;
}
