#ifndef _MESSAGE_H
#define _MESSAGE_H

#ifdef  __cplusplus
extern  "C" {
#endif
#include "config.h"
	
#define RES_SUCC   1
#define RES_FAIL   0

#define MSG_DATA_SIZE 32

typedef enum E_MSG
{
	MSG_NONE = 0,
	MSG_PARAM_INPUT = 1,        /*��������       ������-->����*/
  	MSG_RESQ_PASSWD = 2,        /*������������   ����-->������*/   
	MSG_GET_PASSWD = 3,         /*��ȡ������     ������-->����*/ 
  	MSG_VERIFY_RESULT = 4,      /*����У����   ����-->������*/ 
  	MSG_WORK_STATU = 5,         /*����״̬0 1 2 3 4����-->������*/ 
	
	MSG_CHECK_CARD = 6,
	MSG_MANAGE_CARD = 7,
	MSG_CARD_INFO  = 8,			/*byte0-��Ϣ:1-�п�0-�޿�*/
	MSG_LOCKED_CARD = 9,        /*������         ����-->������*/
	MSG_UNLOCK_CARD = 10,       /*������         ������-->����*/
	MSG_LOCK_RESULT = 11,       /*�������byte0-ǹ�ţ� byte1-���:0-ʧ��1-�ɹ� */
	MSG_UNLOCK_RESULT = 12,     /*�⿨���       ����-->������  byte0-ǹ�ţ�byte1- ���:0-ʧ��1-�ɹ�2-��������*/
	MSG_SYSTEM_SETTING = 13,    /*ϵͳ����       ����-->������*/
	MSG_PARAM_STORE = 14,       /*������� 0,1,2,3     -->����*/
  	MSG_PARAM_CLEAR = 15,       /*������� 0,1,2,3,4   -->����*/
	MSG_RECORD_CLEAR= 16,       /*�������¼*/
	
	MSG_START_STOP = 20,        /*������ֹͣ��磬����ǰ����д��ز��� 0 ֹͣ 1 ���� byte0-����:0-ֹͣ��1-������byte1-ǹ��*/
	MSG_START_STOP_RESULT = 21, /*����ֹͣ��� ���ط���byte0-���:1-�ɹ���2-ʧ��; byte1-ԭ��; byte2-ǹ��*/
	MSG_UP_RECORD = 22,         /*�ϴ���ʷ��¼ ����--> ��̨ */
	MSG_CHGSTU_END = 23,        /*������*/
	MSG_RECORD_RESULT = 24,         /*�ϴ���ʷ��¼��� ��̨-->����*/
	
	MSG_ISO_START = 30,         /*������Ե���   ����-->ISO*/
	MSG_ISO_STOP = 31,          /*ֹͣ��Ե���   ����-->ISO*/
	MSG_ISO_FINISH = 32,        /*��Ե������   ISO-->����*/
	
	MSG_MOD_ONOFF = 40,         /*ģ�鿪�ػ� 0�ػ� 1����    ����-->����ģ��*/
	MSG_MOD_ADJUST = 41,        /*ģ��������� 1��ѹ 2 ���� ����-->����ģ��*/
	MSG_MOD_GRPSET = 42,        /*ģ���������              ����-->����ģ��*/ 
	MSG_MOD_DELALLCMD = 43,     /*ɾ�������б�����������*/
	MSG_MODDATA_INIT = 44,      /*ģ�����ݳ�ʼ��*/
	MSG_MOD_PRESTOP = 45,      /*����Ԥ����ǰ��ģ��ػ�*/
	MSG_MOD_DGRPSET = 46,      /*˫ǹ�������ã������õ�ģ�����*/
	MSG_MODGRP_ADJUST = 47,    /*˫ǹ���ʷ�����ڵ�ѹ������*/
	
	MSG_CARD_CHECK = 50,        /*��⵽��Ч��        ������-->����   */
	MSG_CARDREADER_IDEL = 51,   /*����������IDEL״̬  ����-->������*/
	
	MSG_ONLINE_CARD = 60 ,      /*���߿�֪ͨ��̨*/
	MSG_VIN_CARD = 61 ,      /*VIN֪ͨ��̨*/
	
	MSG_UPPLUG_GUN = 62 ,      /*�ȴ���ǹ  ��֮��*/
	MSG_WAIT_RECORD = 63 ,      /*�ȴ�����   ��֮��*/
	
	MSG_SELECT_GUNCARD = 80,    /*ѡ��ˢ�������ĸ�ǹ*/

	MSG_TCPCLIENT_DATA_TO_BACK = 90,  	//TCP �������ݷ��͸�BACK���񣻲���������ָ��
	MSG_DATA_TO_TCPCLIENT = 91,      	//BACK �������ݷ��͸�TCP����	������������ָ��

	MSG_RESET_ETH = 92,					//������λ��Ϣ��������

	MSG_DEV_TO_DEBUG_HB=93,				//����NET DEBUG��������������
	
	MSG_DATA_TO_NETDEBUG = 94,  		//���ݷ��͸�NETDEBUG����	

	MSG_DATA_PTR_TO_NETDEBUG = 95,		//���͵������ݸ�netdebug;����������ָ��

	MSG_TO_NETDEBUG_FTPUPGRADE_OK = 96, 		//FTP �����û�����ɹ�������
	
	MSG_TO_ENTER_IDLE = 97, 		//�豸�������״̬��Ϣ��������
	
	MSG_TO_PRINT_TASK_STATUS = 98, 		//���Դ�ӡ��ǰ�������ƣ�������

	MSG_MOD_GET_INFO = 99,

 /*****************���²��ã�����ɾ��*****************************************/
//	MSG_STARTJY_CTRL   = 1, 
//	MSG_FINISH_JY      = 2, 
//	MSG_CHECK_CARD     = 3, 
//	MSG_VALID_CARD     = 4, 
//	MSG_UALOCK_CARD    = 5, 
//	MSG_ACCOUNT_CARD   = 6, /*????*/
//  MSG_UALOCK_RESULT  = 7,	/*??????*/
//	MSG_NOT_STARTCARD  = 8, /*????*/
//	MSG_MANAGE_CARD    = 9, /*??????*/
//	MSG_GET_PASSWORD   = 10,/*?????*/
//	MSG_IDEL_STATU     = 11,/*??????*/
//	MSG_READY_CHARGE   = 12,/*??????*/
//	MSG_CHARGEING_STATU =13,/*???*/
//	MSG_ERR_STATU      = 14,/*????*/
//	
//	MSG_START_CTRL     = 15,/*????*/
//	MSG_REQ_PW         = 16,/*????*/
//	MSG_ADJUST_PW      = 17,/*????*/
//	MSG_STOPOUT_PW     = 18,/*??????*/
//	MSG_STOP_CTRL      = 19,/*????*/
//	MSG_ADJUST_CURR    = 20,/*?????*/
//	MSG_ADJUST_VOLT    = 22,/*????*/
////	MSG_MOD_ONOFF      = 23,/*??????*/
//	MSG_MOD_GROUP      = 24,/*????*/
//	
//	MSG_MC_GET_IC_INFO = 80,/**/
//	MSG_MC_UP_CUR_RECORD=81,
//	MSG_MC_START_FAULT  =82,
//	MSG_MC_START_SUCCESS=83,
//	MSG_MC_END_CHARGE   =84,
}MSG_TYPE;


typedef enum {
	CARD_PASSWD_ERR = 1,  //��������,�����ԣ�
	CARD_ILLEGAL_ERR =2,  //����ʶ��
	CARD_NOMONEY_ERR = 3, //����
	CARD_NOTIDLE_ERR = 4, //���Ƿǿ���
	CARD_VIN_NO_MATCH = 5,  //VIN�벻ƥ��  patli 20190926
}E_CARRDCHECK_ERR;
	



typedef struct 
{
	uint8		 Valid; /* 0?? 1?? */
	uint8    MsgType;
	uint8    MsgLenth;
	uint8    MsgData[MSG_DATA_SIZE];
	uint8	 *PtrMsg;
}MSG_STRUCT;

typedef struct 
{
	uint8	gunno; /* 0?? 1?? */
	uint8   status;
	uint8   reason;
	uint8   errcode;
	uint8	accFg;
	uint8   valid;
}MSG_WORK_STATUS_STRUCT;



extern BOOL  Message_QueueCreat(uint8 TaskPrio);
extern BOOL RcvMsgFromQueue(MSG_STRUCT* outmsg);

extern BOOL RcvPrioMsgFromQueue(MSG_STRUCT* outmsg,uint8 prio);

extern BOOL  SendMsg2Queue(MSG_STRUCT *msg, uint8 DestPrio);

extern void SendMsgPack(uint8 Type,uint8 Len,uint8 Data,uint8 Prio);
//extern void SendMsgDoubleByte(uint8 Type,uint8 Len,void *Data,uint8 Prio);

extern void SendMsgWithNByte(uint8 Type,uint8 Len,void *Data,uint8 Prio);

extern void SendMsgWithNByteByPtr(uint8 Type,uint8 Len,void *Data,uint8 Prio);



#ifdef  __cplusplus
}
#endif
#endif

