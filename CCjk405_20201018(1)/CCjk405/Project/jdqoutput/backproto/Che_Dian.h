/**
  ******************************************************************************
  * @file    Che_Dian.h
  * @author  lw
  * @version v1.0
  * @date    2018-07-31
  * @brief   ������ͨѶЭ��
  ******************************************************************************
	*/
#ifndef _Che_Dian_H_0001
#define _Che_Dian_H_0001

#include "typedefs.h"
#include "Message.h"
#include "ChgData.h"


#define CheDian_1S(n)		(OS_TICKS_PER_SEC*n) /* ??10ms??,??1S */
#define CheDian_1MIN(n)	(CheDian_1S(60)*n)/* 1min */

/***********************
0:������ǹ
4:ֱ����ǹ
5:һ��˫ǹ

************************/
#define  DEV_TYPE_S  4
#define  DEV_TYPE_D  5
#define  CheDian_MAX    255
#define  CheDian_FrameOther    16    //��Ϣ�����������������
#define  DataStartAddr    11    //��Ϣ���ʼ��ַ
#define  CheDian_LifeTime    0x10    //S�������

#if 0

typedef struct CheDianPara
{  
	 uint32  UpCardNo;
	 uint8  Data[CheDian_MAX+20];   //���������÷ֿ���֡ȫ����Ϣ	 
	 uint8  Commforbidden;  //ͨѶ���� 0������    1�����ã������˳�
//	 uint8  RecordAutoEN;  //��̨�����ϴ���¼ʹ�ܣ�0 �������ϴ���1������ѯ�ϴ�
	 uint8  RecordCnt;     //δ�ϴ���̨����¼����
	 uint8  RecordMsg;
	 uint8  RcvbeatCnt;   /*����������*/
//	 uint32  Recordno;     //δ�ϴ���̨����¼��
	 uint16 Len;           /*֡�ܳ����ݳ���*/
	
	 uint32 LifeTime;
	 uint32 RecordTime;
	 uint32 ErrTime;
	 uint32 UpDataTime[DEF_MAX_GUN_NO];
	 uint32 CardnoTime;
	 uint32 RunTimeSec;
}CheDianRun_t;

#endif

typedef union  
{
	uint32 LongData;
	uint8  ByteData[4];
}U32Data;

typedef union  
{
	uint16 ShortData;
	uint8  ByteData[2];
}U16Data;

typedef struct CheDian
{
	uint8   cmd;                 /*����*/
//	uint16  Writeaddr;                 /*����д�뿪ʼ��ַ*/
//	uint16  Readaddr;                 /*����������ʼ��ַ*/
	void  (*func)(uint8 GunNo,uint8 dir);   /*���� GunNoǹ��0��ȫ�֣�1��Aǹ 2��Bǹ dir�շ�����*/
}CheDian_t;

typedef enum
{
	Nothing   = 0,  
	RcvFlag   = 1, 
	SendEN    = 2,
	
}CheDian_busy;

typedef enum
{
	CheDian_GUN0   = 0,  /*�����豸״̬*/
	CheDian_GUNA   = 1, 
	CheDian_GUNB    = 2,
	
}CheDian_DEV;
typedef enum
{
	ON_SUCC   = 0,  
	ON_FAIL   = 1, 
	OFF_SUCC   = 2,  
	OFF_FAIL   = 3,
	
	OTHER_ERR = 0xff,
	
}CheDian_OnOff;

typedef enum
{
	RequestFrame    = 0,  
	ResponseFrame   = 1, 
	AutoFrame       = 2,
	
}CheDian_FrameType;

typedef enum
{
	ServerFrame   = 0x00,  
	CLIENTO   = 0x10,    /*�豸ȫ��*/
	CLIENTA   = 0x11,    /*�豸Aǹ*/
	CLIENTB  = 0x12,     /*�豸Bǹ*/
	
}CheDian_FrameSoure;

typedef enum
{
	AUTO_LIFE     = 0x01,  /*����*/
	AUTO_LOGIN    = 0x02,  /*����*/
	AUTO_QUIT     = 0x03,/*�˳�����*/
	GET_DEVPARA   = 0x04,  /*��ȡ�ն�����*/
	GET_CHGDATA   = 0x09,  /*��ȡʵʱ����*/
	SET_PARA      = 0x05,  /*���ò���*/
	AUTO_RECORD   = 0x06,    /*������ʷ��¼*/ 
	AUTO_ERR      = 0x07,  /*��ȡ�澯����*/
	AUTO_UPDATA   = 0x14,  /*�Զ��ϱ�ʵʱ����*/
	SET_TIME      = 0x08,  /*��ʱ*/
	SET_ONOFF     = 0x10,  /*��ͣ����*/
	AUTO_CARDNO   = 0x30,/*���Ϳ���Ϣ*/
	
}CheDian_ClientCmd;

//extern CheDianRun_t  CheDianRun;

//uint8  CheDian_RcvUploadDataDeal(uint8 *buff);
//uint8  CheDian_fun_Deal(uint8 cmd,uint8 GunNo,uint8 dir);
//void GetDevicRunTime(void);
uint8 CheDian_GetAuto(uint8 *CtrData);
void CheDian_Init(void);
void CheDian_PreBackMsg(MSG_STRUCT *msg);
void CheDian_Run(void);
//BackRun_t *GetCheDianRun(void);
uint8 Monitor_NetConnect(uint8 linkfg);

#endif


