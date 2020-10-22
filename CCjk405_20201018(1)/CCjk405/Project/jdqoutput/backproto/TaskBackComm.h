/**
  ******************************************************************************
  * @file    TaskBackComm.h
  * @author  zqj
  * @version v1.0
  * @date    2017-03-09
  * @brief   
  ******************************************************************************
	*/
	
#ifndef _TASKBACKCOMM_H
#include "typedefs.h"
#include "Message.h"
#include "ChgData.h"
#include "Back_Inter.h"

//#include "NetLay.h"

#define WIFI_RESET_L   Gpio_WritiLoutputIo(OUT9,Bit_RESET)
#define WIFI_RESET_H	 Gpio_WritiLoutputIo(OUT9,Bit_SET)	
#define TEST_COM   MOD_COM
#define DEV_GUNNO   2

#define  BACK_DATA_MAX    512

/* ����״̬���� */
typedef enum
{
	CONNECT_OK=0,			/* ����OK */
	CONNECT_OFF,			/* ���ӶϿ� */
}CONNECT_DEFINE;
/*��̨ͨѶЭ��**/
typedef enum
{	
	CONN_CheDian = 0, /* ��������̨ */
	CONN_Aunice,      /* ���ͺ�̨ */
	CONN_CHGBIRD,     /* ��֮���̨ */    
	CONN_CHGZPLD,     /* ��ƽ¡�غ�̨ */
	CONN_CHCP,        /* ������̨ */
}Back_AgreeTYPE;

#define  SUPPORT_Agree  CONN_CheDian 
/* ����Э�� */
typedef struct
{
	void 	(*InitAgree)(void);/* ��ʼ������ */
	void  (*DealBackMsg)(MSG_STRUCT *msg);/* �����������*/
	void  (*RunAgree)(void);/* �������� */
}Back_Agreement;	

/* ��̨���ƽṹ */
typedef struct
{
//	uint8 BackConStatus;				/* ��������״̬ 0:�Ͽ������������ź� 1:���� ���Ѿ������ź���  */
	uint8 AutoSndCmdFlag;				/* �Զ��������� */
	uint8 CmdCode;							/* ������ */
	uint8 CmdCnt;								/* ����������� */
	uint8 CmdRrdCnt;						/* ��¼���ʹ��� */
	uint8 CardType;							/* �������� 4�������� 5��δ֪�� */
	
	uint8  Runflag[DEV_GUNNO];					/* ��̨����־ */
	uint8  AgreeType;					/* ͨ������ */
	uint8  InterType;					/* ͨ�Žӿ�*/
	uint8  CommStatus;					/* ͨ��״̬ */
	uint8  u8ShowCommStatus;
	uint16 errCount;
	uint32 CommTimers;					/* ͨ�Ŷ�ʱ�� */
	
//	uint8  Stage;								/* ͨѶ���ӽ׶� */
	uint32 DealyTimers;					/* ͨѶ��λ��ʱ��ʱ�� */
	uint32 SerialNo;					/* �ϴ���¼�� */
	uint32 RecordAddr;					/* �ϴ���¼��ַ */
}BACK_CTRL_STRUCT;

typedef struct BackRunPara
{  
	 uint32  UpCardNo;
	 uint8  Data[BACK_DATA_MAX+20];   //���������÷ֿ���֡ȫ����Ϣ	 
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
	 uint8* ptr_msg;
	 uint16 msg_readed_num;
	 uint16 msg_len;
}BackRun_t;

extern uint32 DevRunSec;     //�豸���ж����

/*��̨����*/
extern BACK_CTRL_STRUCT g_BackCtrl;
extern  MSG_STRUCT Backmsg;
extern  NET_DATA_INTER NetHandler;/* �ײ������� */
extern  void InitTCPData(void);
extern  uint8 TskBack_CommStatus(void);
extern  uint8 TskBack_CommStatusSet(uint8 staus);
/*��̨����*/
extern void Task_BackComm(void *p_arg);

//extern int32 TskBack_ReadData(NET_DATA_INTER *pNet,uint8 *pbuf, uint16 MaxSize);

extern void Get_BcdCardNo(uint8 *CardNo,uint8 *buff);
	
extern uint8 UpCardNoASCII(uint8 *buff,uint8 *CardNo);	

extern uint8 updataTime(uint8 *puff ,uint8 tpye);

extern uint8 GetConnectType(void);

extern uint8 GetWorkStep(uint8 gunNo);

extern uint8 GetWorkStatus(uint8 gunNo);
extern int32 Back_Read(uint8 *buf,uint16 len);

	
	
	
	
	























	
	
#endif
