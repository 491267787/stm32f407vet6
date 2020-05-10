#ifndef _MAINDATA_H
#define _MAINDATA_H

#include "sys.h"


#include "RealTimeCheck.h"
#include "CardReader.h"
//////////////////////////////////////////////////////////////////////////////////	 
//���ܣ�   
//���ߣ�
//��������:2020/04/16
//�汾��V1.0									  
//////////////////////////////////////////////////////////////////////////////////

typedef struct
{
//	DEV_ISO_TYPE *iso;            /**/
//	DEV_METER_TYPE *meter;        /**/
//	DEV_GUN_TYPE *gun;
//	DEV_BMS_TYPE *bms;            /**/
//	DEV_RELAY_TYPE *relay;
	DEV_INPUT_TYPE *input;
	DEV_STATUS_TYPE *dev_status;    /*�豸�����������״̬*/
//	DEV_LOGIC_TYPE *logic;
//	START_PARAM *startparam;
//	CALCBILL_DATA_ST *bill;
//	CDMOD_DATA *dlmod;
//	DEV_ACMETER_TYPE *AcMet;
}CHARGE_TYPE;

#pragma pack(1)
typedef  struct
{
    uint16_t LocalSocket;            /*�����˿�*/
    int8_t   Ssid[32];               /*·����AP*/
    int8_t   Password[32];           /*·����AP����*/
    uint8_t  SecuEn;                 /*AP�Ƿ����*/
    uint8_t  DhcpEn;                 /*DHCP�Ƿ�ʹ��*/
    uint8_t  DnsSever1[4];           /*DNS��������ַ1*/
    uint8_t  DnsSever2[4];           /*DNS��������ַ2*/   
}ETH_STA_PARA;

typedef  struct
{
	uint8_t LocalIpAddr[4];            /*����IP��ַ*/
	uint8_t RemoteIpAddr[4];           /*Զ��IP��ַ*/
	uint16_t RemotePort;               /*Զ�̶˿ں�*/
	uint8_t NetMask[4];                /*��������*/
	uint8_t NetGate[4];                /*����*/
	uint8_t MacAdress[6];              /*mac��ַ*/
	ETH_STA_PARA StaPara;              /*wifiģ�����*/
}ETH_PARAM_T; 

typedef  struct
{
	uint8_t conntype;                /*��̨��ʽ*/
	uint8_t agreetype;               /*��̨ͨѶЭ��*/
	ETH_PARAM_T  netpara;            /*�������*/
}PARAM_COMM_TYPE; /*ͨѶ����*/

typedef  union
{
	uint32_t system;
	uint8_t  byte[4];
	 struct {
		uint32_t eleclock      :  1;    //0 
		uint32_t eleclocktype  :  1;    //1 
		uint32_t eleclockback  :  1;    //2 
		uint32_t curfewalarm   :  1;    //3 
		uint32_t curfewsignal  :  1;    //4 
		uint32_t opencharge    :  1;    //5 
		uint32_t metertype     :  1;    //6 
		uint32_t meteralarm    :  1;    //7 
		uint32_t chargemode    :  2;    //8 
		uint32_t BMSpower      :  1;    //10 
		uint32_t prevraysignl  :  1;    //11 
		uint32_t devicetype    :  1;    //12 
		uint32_t devicealarm   :  1;    //13 
		uint32_t batteryalarm  :  1;    //14 
		uint32_t fanalarm      :  1;    //15 
		uint32_t outputcheck   :  1;    //16 
		uint32_t EPOsignal     :  1;    //17 
		uint32_t insulalarm    :  1;    //18 
		uint32_t fandrive      :  1;    //19 
		uint32_t systemtype    :  2;    //20
		uint32_t clearecord    :  1;    //22 
		uint32_t eleunlock     :  1;    //23
		uint32_t powersplitt   :  1;    //24 
		uint32_t rsv           :  7;    // ����
		
	}bits;
}SYSTEM_ALARM;  

/*�� �ֽ� */
typedef  struct  
{
	uint16_t head;       /*�洢ʱʹ��*/
	uint16_t maxvolt;    /**/
	uint16_t minvolt;    /**/
	uint16_t maxcurr;    /**/
	uint16_t mincurr;    /**/
	uint16_t overcurr;   /**/
	uint16_t overdcvolt;
	uint16_t overdccurr;
	uint16_t underdcvolt;
	
	uint8_t  envmaxtemper; /*��������ֵ*/
	uint8_t  gunmaxtemper; /**/
	uint8_t  modnum;       /**/
	uint8_t  grpmodnum[2];   /**/
	uint16_t singmodcurr;  /**/
	
	uint16_t overacvolt;  /**/
	uint16_t underacvolt; /**/
	
	uint16_t isoresist;   /**/
	
	uint8_t elockallow;   /**/
	uint8_t elocktm;      /**/
	uint8_t bhmtm;
	uint8_t xftm;
	uint8_t brmtm;
	uint8_t bcptm;
	uint8_t brotm;
	uint8_t bcltm;
	uint8_t bcstm;
	uint8_t bsttm;
	uint8_t bsdtm;
	
	uint8_t gunnum;

	SYSTEM_ALARM  Sysparalarm;
	
	
	/************���������λ��**********************************/
 	uint16_t crc;      /*�洢ʱʹ�� ������ڽṹ�����*/
	
}PARAM_OPER_TYPE;  /*ҵ����� */



typedef struct
{
	uint16_t head;        /*�洢ʱʹ��*/
	uint16_t dcmetbaud;   /**/
	uint16_t dcmetAdr;    /**/
	uint8_t  chargeAddr;  /**/
	uint8_t  chargeId[16]; /**/
	uint8_t  onlinecard;  /**/
	uint8_t  musrpasswd[8];/**/
	uint8_t  telephone[16]; /**/
	uint8_t  safeclass;    /**/
	uint8_t  maxmodnum;     /**/
  CARD_PARAM_ST card; /**/
	PARAM_COMM_TYPE  bkcomm; /**/
	uint16_t crc;         /*�洢ʱʹ��*/
}PARAM_DEV_TYPE;      /*�豸�����*/



#pragma pack()




PARAM_OPER_TYPE *ChgData_GetRunParamPtr(void);
PARAM_DEV_TYPE *ChgData_GetDevParamPtr(void);
CHARGE_TYPE *ChgData_GetRunDataPtr(void);
void MainCtrlUnit_Init(void);
#endif

