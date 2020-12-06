/**
  ******************************************************************************
  * @file    chgdata.h
  * @author  zqj
  * @version v1.0
  * @date    2018-04-09
  * @brief   
  ******************************************************************************
*/
#ifndef __CHGDATA_H_0001
#define __CHGDATA_H_0001

#include "TaskIsoCheck.h"
#include "RelayOut.h"
#include "TaskRealCheck.h"
#include "Bms.h"
#include "TaskDcMeter.h"
#include "BillingUnit.h"
#include "TaskCardReader.h"
#include "TaskAcMeter.h"
#include "CdModel.h"

#define MAIN_ADDR 0xf8

#define DEF_MAX_GUN_NO   2    /*����ǹ����*/

#define MODULE_MINVOL 400    //ģ�����С������ѹ 
#define MODULE_MAXVOL 4000   //patli 20201018 1100   //ģ����������ѹ

typedef union
{
	uint32 dword;
	uint16 word[2];
	uint8  byte[4];
	struct {
		uint32 modErr   : 1; //0
		uint32 bit1     : 1; //1
		uint32 bit2     : 1; //2
		uint32 bit3     : 1; //3
		uint32 bit4     : 1; //4
		uint32 bit5     : 1; //5
		uint32 bit6     : 1; //6
		uint32 bit7     : 1; //7  cc1
		uint32 bit8     : 1; //8
		uint32 bit9     : 1; //9
		uint32 bit10    : 1; //10
		uint32 bit11    : 1; //11
		uint32 bit12    : 1; //12
		uint32 bit13    : 1; //13
		uint32 bit14    : 1; //14
		uint32 flasherr : 1; //15 �洢����
		
		uint32 posFU    : 1;//16����о
		uint32 negFU    : 1;//17����о
		uint32 emcystop : 1; //18��ͣ
		uint32 acKm     : 1; //19�����Ӵ���״̬
		uint32 gunCC1   : 2; //20  0~3
		uint32 spd      : 1;  //22 ������״̬
		uint32 elockstu : 1; //23  

		uint32 rsv22    : 1; //24
		uint32 rsv23    : 1; //25
		uint32 rsv24    : 1; //26
		uint32 rsv25    : 1; //27
		uint32 rsv26    : 1; //28
		uint32 rsv2     : 1; //29
		uint32 rsv3     : 1; //30
		uint32 rsv5     : 1; //31
		
	}bits;
}START_STU_ST;

typedef enum
{
	CHG_AUTO_TYPE   = 1,
	CHG_TIME_TYPE   = 2,
	CHG_MONEY_TYPE  = 3,
	CHG_ENERGY_TYPE = 4,
	
}E_CHG_NTYEP;


//�豸����״̬�����
typedef struct {
	
	uint8  workstep;     /*���׶�*/
	uint8  startby;      /*������ʽ��0 ˢ�� 1 ��̨ 2 VIN 3 ��ǹ���� 4 ����*/
	uint8  chgmod;       /*��緽ʽ 1���Զ� 2����ʱ�� 3������� 4��������*/
	uint8  stopReason;   /*����ԭ��*/
	uint8  errCode;      /*������*/
	uint8  gunInUsed;    /*���ǹʹ����*/
	uint32 settime;
	int32  setmoney;
	uint32 setenergy;

	uint32 time;
	int32  money;
	uint32 energy;
	uint8  account[20];
	uint8  usrpin[3];
	uint8  startfg;      /*�����ɹ����������־*/
	uint8  recsavefg;    /*��¼�洢��־*/
	START_STU_ST runstu; /*���׮����״̬*/
	uint32 chgtimes;     /*���׮�����ϵ������*/
	uint32 u32TranPageTicks;  /*�������ȴ���תʱ��*/
}DEV_LOGIC_TYPE;

typedef enum
{
	START_BY_CARD = 0,
	START_BY_BKGROUND = 1,
	START_BY_ONLINECARD = 2,
	START_BY_VIN  = 3,
	START_BY_GUN  = 4, //
	START_BY_KEY  = 5,

	START_BY_PASSWD = 6,  //�˻��������
	
}ESTART_BY_WAY;

/* ��̨�������Ͷ���  */
typedef enum
{
	CONN_WIFI=0,		
	CONN_4G,
  CONN_ETH,	
	CONN_2G,	
	CONN_GPRS,
}CONNECT_TYPE;
typedef enum
{
	PARA_SCREEN = 1,
	PARA_BKGROUND = 2,
}ESTARTPARAM_BY;

//typedef enum
//{
//	CHARGE_WAY_AUTO = 1,
//	CHARGE_WAY_TIME = 2,
//	CHARGE_WAY_MONEY = 3,
//	CHARGE_WAY_ENERGY = 4,
//}ECHARGE_WAY;






typedef struct
{
	DEV_ISO_TYPE *iso;            /*�Ӵ����ڲ��ѹ VDC1 �Ӵ�������ѹVDC3*/
	DEV_METER_TYPE *meter;        /**/
	DEV_GUN_TYPE *gun;
	DEV_BMS_TYPE *bms;            /**/
	DEV_RELAY_TYPE *relay;
	DEV_INPUT_TYPE *input;
	DEV_LOGIC_TYPE *logic;
	START_PARAM *startparam;
	CALCBILL_DATA_ST *bill;
	CDMOD_DATA *dlmod;
	DEV_ACMETER_TYPE *AcMet;
}CHARGE_TYPE;

#pragma pack(1)
typedef  struct
{
    uint16 LocalSocket;            /*�����˿�*/
    int8   Ssid[32];               /*·����AP*/
    int8   Password[32];           /*·����AP����*/
    uint8  SecuEn;                 /*AP�Ƿ����*/
    uint8  DhcpEn;                 /*DHCP�Ƿ�ʹ��*/
    uint8  DnsSever1[4];           /*DNS��������ַ1*/
    uint8  DnsSever2[4];           /*DNS��������ַ2*/   
}ETH_STA_PARA;

typedef  struct
{
	uint8 LocalIpAddr[4];            /*����IP��ַ*/
	uint8 RemoteIpAddr[4];           /*Զ��IP��ַ*/
	uint16 RemotePort;               /*Զ�̶˿ں�*/
	uint8 NetMask[4];                /*��������*/
	uint8 NetGate[4];                /*����*/
	uint8 MacAdress[6];              /*mac��ַ*/
	ETH_STA_PARA StaPara;            /*wifiģ�����*/
}ETH_PARAM_T; 

typedef  struct
{
	uint8 conntype;                /*��̨��ʽ*/
	uint8 agreetype;                /*��̨ͨѶЭ��*/
	ETH_PARAM_T  netpara;        /*�������*/
}PARAM_COMM_TYPE; /*ͨѶ����*/


typedef  union
{
	uint32 system;
	uint8  byte[4];
	 struct {
		uint32 eleclock      :  1;    //0 ���������
		uint32 eleclocktype  :  1;    //1 ����������
		uint32 eleclockback  :  1;    //2 ����������
		uint32 curfewalarm   :  1;    //3 �Ž��澯
		uint32 curfewsignal  :  1;    //4 �Ž��ź�
		uint32 opencharge    :  1;    //5 ����ͣ��
		uint32 metertype     :  1;    //6 �������
		uint32 meteralarm    :  1;    //7 ���澯
		uint32 chargemode    :  2;    //8 ���ģʽ
		uint32 BMSpower      :  1;    //10 BMS��Դ
		uint32 prevraysignl  :  1;    //11 �����ź�
		uint32 devicetype    :  1;    //12 ˢ��������
		uint32 devicealarm   :  1;    //13 ˢ�����澯
		uint32 batteryalarm  :  1;    //14 ��ط��Ӹ澯
		uint32 fanalarm      :  1;    //15 ����澯
		uint32 outputcheck   :  1;    //16 ���У��
		uint32 EPOsignal     :  1;    //17 EPO�ź�
		uint32 insulalarm    :  1;    //18 ��Ե�澯
		uint32 fandrive      :  1;    //19 �������
		uint32 systemtype    :  2;    //20ϵͳ����  1 ��ǹģʽ  2˫ǹģʽ
		uint32 clearecord    :  1;    //22 �����¼
		uint32 eleunlock     :  1;    //23������������Ϊ1ʱ����粻���Ƿ���㣬�����Զ�����
		uint32 powersplitt   :  1;    //24 ���ʷ��� 0 ����������  1����������
		uint32 rsv           :  7;    // ����
		
	}bits;
}SYSTEM_ALARM;  //add tss 20180814


/*�� 52�ֽ� */
typedef  struct  
{
	uint16 head;       /*�洢ʱʹ��*/
	uint16 maxvolt;    /*��������ѹ 750*/
	uint16 minvolt;    /*��С�����ѹ*/
	uint16 maxcurr;    /*����������*/
	uint16 mincurr;    /*��С�������*/
	uint16 overcurr;   /*��ǹ����������ֵ*/
	uint16 overdcvolt;
	uint16 overdccurr;
	uint16 underdcvolt;
	
	uint8  envmaxtemper; /*��������ֵ*/
	uint8  gunmaxtemper; /*���ǹ����ֵ*/
	uint8  modnum;       /*ģ��������*/
	uint8  grpmodnum[2];   /*һ��ģ������  20181130*/
	uint16 singmodcurr;  /*��ģ��������*/
	
	uint16 overacvolt;  /*������ѹֵ*/
	uint16 underacvolt; /*����Ƿѹֵ*/
	
	uint16 isoresist;   /*��Ե����澯ֵ*/
	
	uint8 elockallow;   /*�������������� 0:������������ 1����������������*/
	uint8 elocktm;      /*s*/
	uint8 bhmtm;
	uint8 xftm;
	uint8 brmtm;
	uint8 bcptm;
	uint8 brotm;
	uint8 bcltm;
	uint8 bcstm;
	uint8 bsttm;
	uint8 bsdtm;
	
	uint8 gunnum;
	

	uint8 Fanstartemper;    //�����ת�¶�   add tss 20180814
	uint8 Fanstoptemper;    //���ֹͣ�¶�
	uint8 Fanturnstemper;   //���ת���¶�
	uint8 Weavercoeffic;    //����ϵ��
	uint8 WalkIntime;       //WalkInʱ��

	
	SYSTEM_ALARM  Sysparalarm;
	
//	uint8 SysPincod[6];    //������������
	
	/************���������λ��**********************************/
 	uint16 crc;      /*�洢ʱʹ�� ������ڽṹ�����*/
	
}PARAM_OPER_TYPE;  /*ҵ����� 60byte*/

/*ע�⣺��Ӳ��ܳ���300�ֽ� */
typedef struct
{
	uint16 head;        /*�洢ʱʹ��*/
	uint16 dcmetbaud;   /*���ܱ�����*/
	uint16 dcmetAdr;    /*ֱ�����ַ*/
	uint8  chargeAddr;  /*���׮ͨѶ��ַ*/
	uint8  chargeId[16]; /*׮ID*/
	uint8  onlinecard;  /*���߿�1 ���߿� 0 ˢ����ʽ*/
	uint8  musrpasswd[8];/*����Ա����*/
	uint8  telephone[16]; /*��ϵ�绰*/
	uint8  safeclass;    /*ϵͳ��ȫ����*/
	uint8  maxmodnum;     /*���ģ������*/
  CARD_PARAM_ST card; /*������*/
	PARAM_COMM_TYPE  bkcomm; /*��̨ͨѶ����*/
	uint16 crc;         /*�洢ʱʹ��*/
}PARAM_DEV_TYPE;      /*���׮�豸����� 168*/

/*��464�ֽ�*/
typedef struct
{
	uint16 head;                                   /*�洢ʱʹ��*/
	SECT_PRICE_ST SectTimeFee;  
	uint16 crc;                                    /*�洢ʱʹ��*/
}PARAM_FEE_TYPE;  /*���ʲ��� ��464�ֽ� */

typedef struct SetFeiLv
{
	 uint16 FeiLv;   //����
	 uint8 startH;
	 uint8 startM;
	 uint8 stopH;
	 uint8 stopM;
}SetFeiLv_t;

#ifdef CP_PERIOD_FEE
typedef struct
{
	uint16 head; 
	uint8  num;   //�׶���
	SetFeiLv_t BackFeiLv[MAXSFEILVNUMS];
	uint16 crc; 
	
}PARAM_CP_PERIOD_FEE_TYPE;  // ռ��2+1+6*24+2 = 148�ֽ�
#endif

#pragma pack()


//PARAM_DEV_TYPE    �豸��������
//PARAM_FEE_TYPE    ���ʲ�������
//PARAM_OPER_TYPE   ���в�������

typedef enum
{
	SAVE_ALL_TYPE = 0,
	SAVE_OPER_TYPE = 1,
	SAVE_DEV_TYPE  = 2,
  SAVE_FEE_TYPE = 3,
  #ifdef CP_PERIOD_FEE
  SAVE_CP_PERIOD_FEE_TYPE = 4,
  #endif
}E_SAVETYPE;

typedef enum
{
	GUNNUM_1 = 1,
	GUNNUM_2 = 2,
	GUNNUM_3 = 3,
  GUNNUM_4 = 4,
}E_GUNNUM;

typedef enum
{
	DEV_DC_S = 1,
	DEV_DC_D = 2,
}E_DEVTYPE;

/*���߿�1 ���߿� 0 ˢ����ʽ*/
typedef enum
{
	E_OFFLINE_CARD = 0,
	E_ONLINE_CARD = 1,
	
}E_STARTCARD_TYPE;

#define AGUN_NO  0
#define BGUN_NO  1


extern CHARGE_TYPE *ChgData_GetRunDataPtr(uint8 gunNo);

extern PARAM_OPER_TYPE *ChgData_GetRunParamPtr(void);

extern PARAM_FEE_TYPE *ChgData_GetFeeRate(uint8 gunNo);

extern PARAM_DEV_TYPE *ChgData_GetDevParamPtr(void);

extern DEV_LOGIC_TYPE * ChgData_GetLogicDataPtr(uint8 gunNo);


extern PARAM_COMM_TYPE *ChgData_GetCommParaPtr(void);

extern uint8 ChgData_InitDefaultOperPara(void);

extern uint8 ChgData_InitDefaultFeePara(void);

extern uint8 ChgData_InitDefaultDevPara(void);

extern START_PARAM *ChgData_GetStartParaPtr(uint8 gunNo);


#ifdef CP_PERIOD_FEE
extern PARAM_CP_PERIOD_FEE_TYPE *ChgData_GetCpPeriodFeeRate(void);

#endif




















#endif


