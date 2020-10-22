/*HcModel.h*/
#ifndef __HCMODEL_H_001
#define __HCMODEL_H_001
#include "typedefs.h"
#include "message.h"
#include "can.h"


#ifdef STM32F10X_HD
#define RS485_CDMOD
#else
#define CAN_CDMOD
#endif

#define CDMOD_CAN 	CAN_2   //patli 20200318 CAN_COM3

#define CD_HOSTID  0xF0
#define CDPROTO_NO 0x60

#define MOD_CMD_ON   0
#define MOD_CMD_OFF  1
#define MOD_CMD_GRP_OFF 2

#define CD_MAX_NUM  8     /*60KW  4 * 15KW */

#define CD_GROUP_No    1
#define CD_global_add  0

#define BRAODCAST_ADR  0
#define CMD_SEND_TIMES 2

#define MAX_MOD_GROUP  2
#define MAX_GPMOD_NUM  8



#define CD_ModeAC_EndVlot  3100   //0.1V   ���304V
#define CD_ModeAC_TopVlot  3900   //0.1V   ���394V

#define CD_ModeIn_EndVlotDC  2050   //0.1V    ģ�����200V
#define CD_ModeIn_TopVlotDC  7750   //0.1V     ģ�����780V


typedef enum
{  
	 SETTING_DATA = 0x0,      /*��������*/
	 ACK_SETTINGDATA = 0x1,      /*����Ӧ������*/
	 REQ_BYTEDATA = 0x2,      /*��ѯ����*/
	 ACK_REQ_DATA  = 0x3,   /*��ѯӦ������*/
	 MULTI_DATA_CMD = 0x4,    /*�ۺ�������������*/
	
}CD_MSGTYPE;

/*CAN Э��*/
typedef enum 
{
	ERR_CMD   = 0x0000,
	OUT_VOLTE = 0x0001,          /*ģ��˿������ѹ*/
	OUT_CURR  = 0x0002,          //ȡģ��˿��������
	MOD_LIMIT_POIN = 0x0003,     //ģ��������    ����������Ĵ���
	INPUT_TEMPER = 0x000B,       //����ڻ����¶�
	INPUT_AC_AVOLT = 0x000C,       //ȡģ��AC�����ѹ
	INPUT_AC_BVOLT = 0x000D,       //ȡģ��AC�����ѹ
	INPUT_AC_CVOLT = 0x000E,       //ȡģ��AC�����ѹ
	ALARM_WORDS = 0x0040,            /*��ȡ��ǰ�澯״̬*/

	
	SETTING_MOD_VOLTE = 0x0021,    /*����ģ���ѹ*/
	SETTING_LIMIT_POINT = 0x22,    /*��ģ��������*/
	START_STOP_OPER = 0x0030,       /*���ÿ��ػ�����*/ 
  GROUP_NUMBER = 0x00BD,         //����ģ�����	
  
}CD_VALUETYPE;



typedef enum
{
	START_HCMOD = 0x0000,
	STOP_HCMOD = 0x0001,
	WAIT_OPER  = 0x0000,
	RECF_CURR  = 0x0001,   /*����*/
	CONTRA_CURR = 0x0002,  /*���*/
	
}CD_CMD;

typedef enum
{ 
	 CAP_DISENABLE = 0x0000,
   CAP_ENABLE = 0x0001,
	
}CD_CAP;

typedef struct {
	uint8 vaild;
	uint8 num;
	uint8 AddrList[CD_MAX_NUM+1];
}COMM_MODLIST;


#ifdef  CAN_CDMOD
 #define MOD_ONOFF    START_STOP_OPER
 #define MOD_SETVOLT  SETTING_MOD_VOLTE
 #define MOD_SETCURR  SETTING_LIMIT_POINT
 #define COFF_DATA    1024/10 
 #define MOD_SETGROUP  GROUP_NUMBER
#else
 #define MOD_ONOFF     EONOFF
 #define MOD_SETVOLT   EVREF
 #define MOD_SETCURR   EILMT
 #define COFF_DATA     100
 #define MOD_SETGROUP  EGRPADRESS
#endif



#pragma pack(1)
typedef union
{
  uint32 ExtId;
	struct
	{
		uint32 res2:1;      /*���� 1*/
		uint32 res1:1;      /*���� 1*/
		uint32 cnt :1;      /*����֡*/
		uint32 src:8;       /*Դ��ַ*/
		uint32 dst:8;       /*Ŀ�ĵ�ַ*/
		uint32 ptp:1;       /*��Ե�*/
		uint32 protno:9;    /*Э���*/
		uint32 rsv:3;       /*����*/
		
	}sbit;
		
}CDFRAME_HEAD;


typedef enum
{
	ESET_DATA = 0,
	ESET_RESP = 1,
	EREAD_DATA= 2,
	EREAD_RESP = 3,
}EMSG_TYPE;

/*RS485 Э�� */
typedef enum
{
	EVOUT = 0,
	ECURR = 1,
	EVREF = 2,
	EILMT = 3,
	EONOFF= 4,
	EMODST = 8,
	ETEMP  = 30,
	EGRPADRESS = 89,
}ECMD_TYPE;




typedef  union{
  uint8 uc[4];
  uint16 us[2];
  uint32 u32d;
  float  f32d;	
}DATA4_TYPE;





typedef  union
{
	uint16 us;
	uint8 uc[2];
}DATA2_TYPE;


typedef  union
{
   uint8 u8;
	  struct 
		{ 
			uint8 msgType:4;	
		 	uint8 groupAddress:4;
			 
	  }bits;
}GRPTYPE;

typedef union
{
   uint8 dat[8];
     struct DA
		{
		  GRPTYPE ntype;
		  uint8 errType;
		  DATA2_TYPE vtype;
      DATA4_TYPE val;	
		}db;
}CDFRAME_DATA;



/*����V1.2�� 2017-11-03*/
typedef  union
{
	uint32 u32d;
	 struct {
		uint32 ovAcVolt:1;      /*0���������ѹ*/
		uint32 underAcVolt:1;   /*1��������Ƿѹ*/
		uint32 ovAcVoltstop:1;  /*2������ѹ�ػ���ģ�鱣��*/
		uint32 rsv0:3;          /*3,4,5*/
	 	uint32 ovVolt:1 ;       /*6ֱ�������ѹ*/
		uint32 ovVoltstop:1;    /*7ֱ����ѹ�ػ�*/
		
		uint32 underVolt:1 ;    /*8ֱ�����Ƿѹ*/
		uint32 ErrFan:1;        /*9���Ȳ�����*/
		uint32 rsv1:2;          /*10,11����*/
		uint32 ovTermp:1;       /*12���±���*/
		uint32 rsv2:1;          /*13����*/
		uint32 PFCovTermp1:1;   /*14PFC���±���1*/
		uint32 PFCovTermp2:1;   /*15PFC���±���2*/
		uint32 DCovTermp1:1;    /*16DC���¶ȱ���1*/
		uint32 DCovTermp2:1;    /*17DC���±���2*/
		uint32 ErrModel:1;      /*18ģ����ϣ�PFC��DCDCͨ�Ź���*/
		uint32 rsv3:1;          /*19����*/
		uint32 PFCErr:1;        /*20PFC����*/
		uint32 DCDCErr:1;       /*21DCDC����*/
		uint32 rsv4:1;          /*22����*/
		uint32 rsv5:1;          /*23����*/
		uint32 rsv6:1;          /*24����*/
		uint32 ModStu:1;        /*25ģ��״̬ 1���ػ� 0������*/ 
		uint32 ModWork:1;       /*26ģ�鹤�ʷ�ʽ 1������ 0����ѹ*/ 
		uint32 rsv7:1;          /*27����*/
		uint32 rsv8:1;          /*28����*/
		uint32 rsv9:1;          /*29����*/
		uint32 rsv10:1;         /*30����*/
		uint32 rsv11:1;         /*31����*/
	}bits;
}ALARM_STATU;

#define DLMOD_ON   0
#define DLMOD_OFF  1


typedef  union
{
	 uint32 u32d;
	  struct
	 {
		 uint8 pfchw;
		 uint8 pfcsw;
		 uint8 dcdchw;
		 uint8 dcdcsw;
	 }us;
}VERSION_ST;



typedef  struct 
{
	uint16 inVolteACA;       /*�����ѹ*/
	uint16 inVolteACB;       /*�����ѹ*/
	uint16 inVolteACC;       /*�����ѹ*/
	uint16 outVolte;         /*�����ѹ*/
	uint16 outCurr;          /*�������*/
	uint16 limtPoint;        /*������*/
	uint16 temper;           /*�¶�*/
	ALARM_STATU alarmInfo;
	uint8 onoff;             //1 �ر�  ��0 ����
	uint8 commErr;           /*ģ��ͨ�Ź���*/
	uint8  grpNo;           /*ģ�����*/
	uint8  addr;            /*ģ���ַ*/
	uint8  vaild;           /*�Ƿ���Ч*/
	uint32 lastticks;       /*���ͨѶʱ��ticks*/
	
}CDMOD_DATA;



typedef  struct 
{
	uint16 outVolte;    /*�����ѹ*/
	uint16 limtPoint;
	uint8 onoff;
	uint8 modegroupnum;
	
}CDMOD_Set;

#pragma pack()


typedef struct
{
	 uint8  succ;
	 uint8  exec;
	 uint8  dstAddr;
	 uint8  cmd;
	 uint8  groupAddress;
	 uint16 vtype;
   uint32 value;
}CMD_LIST;

typedef struct
{
	uint8  u8Num;            /*����ģ������*/
	uint8  u8GpNo;           /*���*/
	uint8  u8CommStu;        /*��ģ��ͨѶ״̬ 1��ͨѶ������0��ͨѶ*/
	uint8  u8SplitNum;       /*���η���ģ����*/
	uint8  u16Allowed;       /*�Ƿ��������*/
	uint8  u8SlipCnt;        /*�������*/
	uint16 u16Curr;          /*�������*/
	uint16 u16Volt;          /*�����ѹ*/
	uint16 u16StepCurr;      /*���䲽������*/
	uint8  u8Step;           /*���Ʋ���*/
	uint8 u8Addr[MAX_GPMOD_NUM];        /*����ģ���ַ*/
	uint8 u8WorkStu[MAX_GPMOD_NUM];     /*����ģ��״̬*/
	uint8 u8HwGpModNum;     /*Ӳ������ģ������*/
	uint8 u8HwGpAddr[MAX_GPMOD_NUM];    /*Ӳ������ģ���ַ����Ӳ���̻�*/
	uint16 u16HwTolCurr;       /*Ӳ������ģ���ܵ���*/
	uint32 u32WaitTicks;
	uint32 u32ExitTicks;      /*�˳���ʱ*/
	uint32 u32StepTicks;
}MOD_CTRL_PARA;



//extern CDMOD_DATA gCdmodData[];


extern void CdModData_Init(uint8 grpNo);



extern CDMOD_DATA * CdModData_GetDataPtr(uint8 no);


extern struct list *CdMod_GetHead(void);

extern int CdMod_InsertOneCmd(CMD_LIST * scmd);
extern int CdMod_deleteCmd(int id);

extern int CdMod_ExecuCmd(void);

extern int CdMod_IsListExec(int id,uint16 wmsecs) ;

extern int CdMod_DeleteAllCmd(void);


/*����ͨѶ��ģ���Ƿ�������һ�£��Դ��ж��Ƿ���ͨѶ����*/
extern uint8 CdModData_CheckCommErr(uint8 grpNo);

extern uint8 CdModData_CheckOnOffStu(uint8 grpNo,uint8 stu);

extern uint8 CdModData_GetModCommErr(uint8 grpNo);

extern uint8 CdModData_DelAllCmd(void);


extern int CdModData_InsertModCmdInList(uint8 adr,uint8 grpNo,uint16 cmd,uint32 value);


extern void CdModData_MsgDeal(MSG_STRUCT* msg);

extern uint8 CdModData_SetGroupNo(uint8 adr,uint8 grpNo);


extern MOD_CTRL_PARA *CdModData_GetModGpPara(uint8 grpNo);

/*�����㲥����*/
extern uint8 CdModData_SetGroupCmd(uint8 grpNo,uint8 cmd,uint32 value);

extern uint8 CdModData_SetSingleCmd(uint8 adr,uint8 cmd,uint32 value);



#endif


