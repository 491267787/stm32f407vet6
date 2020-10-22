/**
  ******************************************************************************
  * @file    TaskMainCtrl.h
  * @author  zqj
  * @version v1.0
  * @date    2018-04-09
  * @brief   
  ******************************************************************************
	*/
#ifndef  __TASKMAINCTRL_H_0001
#define  __TASKMAINCTRL_H_0001
#include "typedefs.h"
#include "TaskCardReader.h"
#include "TaskBmsComm.h"


/**
[ISO] 
[Meter]
[Gun]
[module]
[BMS]
[Elock]
[Relay]
[Input]
[Logic]  �߼���
***/




#define RTN_MAIN_TRUE   1
#define RTN_MAIN_FALSE  0



#define VOLT_TRAN(a) ((a)*10)
#define CURR_10TRAN(a) ((a)*10)

#define SECS2TICKS(a)  ((a)*OS_TICKS_PER_SEC)

#define PASSWD_NUM  3

#define MAX_ERRBUF_SIZE 20
typedef struct
{
	uint8 gunNo;
	uint8 state;
	uint8 u8ClearFg;
	uint8 u8AccFg;
	uint16 u16TmpCnt;
	uint8 u8ErrFlag;
	uint32 u32WaitTicks;
	uint32 u32IdleTicks;
	uint32 u32Bms24Ticks;
	uint32 u32VINTicks;
	uint8  errBuf[MAX_ERRBUF_SIZE];
}CTRL_STEP;

typedef struct
{
	CTRL_STEP ctrl;
	
}GUN_CTRL_ST;

typedef struct {
	uint8  gunNo;             /*ǹ��*/
	uint8  vailfg;            /*��������ʹ�ñ�־,��Ϊ0ʱ,����ʹ��*/
	uint8  startby;           /*������ʽ��0 ˢ�� 1 ��̨ 2 VIN 3 ��ǹ���� 4 ����  5.�˺�����*/  
	uint8  chgmod;            /*���ģʽ 1���Զ� 2����ʱ�� 3������� 4��������*/
	uint32 settime;
	int32  setmoney;
	uint32 setenergy;
	
	uint32 time;
	int32  money;
	uint32 energy;
	uint8 serilNo[32];
	uint8 account[20];        /*�˻�  BCD�룬������A*/
	uint8 password[20];       /*����*/
	uint8 vincode[17];        /*VIN��*/
	uint8 vinback;            /*VIN���̨���ݱ�ʶ*/
	uint32 vinbackTicks;
  uint8 account2[12];       /* �����ĺ�̨�û�����չ��32λ,���������� */
	
}START_PARAM;

typedef enum
{
	STEP_IDEL = 0x00,         /*���н׶�*/
	STEP_START = 0x11,        /*�����׶�*/
	STEP_LINK = 0x12,         /*�������ӽ׶�*/
	STEP_BPOWER= 0x13,        /*�����ϵ�׶�*/
	
	
	
	STEP_SKHANDS = 0x14,      /*���ֽ׶�*/
	
	//STEP_RCV_BHM = 0x50,     /*�յ�BHM*/
	
	STEP_PARAMSET = 0x15,     /*�������ý׶�*/
	STEP_CHARGEING = 0x16,    /*���׶�*/
	STEP_CHGEND  = 0x17,      /*�������׶�*/
	STEP_WAITOVER = 0x18,     /*�ȴ���ǹ�׶�*/
	
	STEP_BIRDWAITOVER = 0x19,     /*�ȴ���ǹ�׶�*/
	
	STEP_ISOCHECK_SUB = 0x20, /*��Ե���׶�*/
	STEP_ISOWAIT_SUB  = 0x21, /*�ȴ���Ե������*/
	STEP_ISOFINISH_SUB = 0x22,/*��Ե�����ɣ�й��*/
	
  STEP_BRM_WAIT_SUB = 0x30,
	STEP_BRM_SUB      = 0x31, /*�յ�BRM*/
	STEP_BCPWAIT_SUB  = 0x32,
	STEP_BCP_SUB      = 0x33,
	STEP_BROWAIT_SUB  = 0x34,
	STEP_BRO_SUB      = 0x35,
	
	STEP_ADJUST_SUB   = 0x37, /*��ѹ����*/
	
	STEP_OLD_GBT_SUB = 0x38,/*�����Ϲ���*/
		
	STEP_WAIT_SUB = 0x41,
	
}E_CHARGE_STEP;

extern void TskMain_SetWorkStep(uint8 gunNo,uint8 step);
extern uint8 TskMain_GetWorkStep(uint8 gunNo);

//extern CHARGE_TYPE *TskMainGetRunDataPtr(void);
#define SET_STOPING(a,b,c) TskMain_SetStoping((a),(b),(c))
#define SET_WORKSTEP(a,b) TskMain_SetWorkStep((a),(b))
#define GET_WORKSTEP(no)  TskMain_GetWorkStep((no))

#define ASSIST_POWER_ON  0x01
#define ASSIST_POWER_OFF 0x00

#define JOUT_ON    0x01
#define JOUT_OFF   0x00





typedef enum {
	LOCK_TM = 0x01,    /*������*/
	BHM_TM  = 0x02,    /*bhm*/
	XF_TM   = 0x03,    /*й��*/
	BRM_TM  = 0x04,    /*brm*/
	BCP_TM  = 0x05,    /*bcp*/
	BRO_TM  = 0x06,    /*bro*/
	BCL_TM  = 0x07,    /*bcl*/
	BCS_TM  = 0x08,    /*bcs*/
	BST_TM  = 0x09,    /*bct*/
	CST_TM  = 0x0A,    /*cst 100ms*/
	BSD_TM  = 0x0B,    /*bsd*/
	
	ISO_ERR = 0x0c,      /*��Ե����*/
	KMVOLT10_ERR = 0x20, /*�Ӵ�������ѹ<10V*/
	KMVOLT60_ERR = 0x21, /*�Ӵ�������ѹ<60V*/
	PARAM_ERR    = 0x22, /*�����Ƿ����*/
	CARVOL_ERR   = 0x23, /*�뱨�ĵ�ص�ѹƫ�� <��5%*/
	BATSTU_ERR   = 0x24, /*���״̬�Ƿ�����*/
	DISALLOW_FG  = 0x25, /*����ֹ��־*/
	CC1LINKED_ERR = 0x26, /*CC1����״̬*/
	MODVOLT_ERR   = 0x27, /*ģ���ѹ���δ��Ӧ*/
	KMINOUT_ERR   = 0x28, /*�Ӵ���������ѹ�����10V*/

	EMET_COMM_ERR = 0x40,   /*ֱ����ͨѶ����*/
	ISO_COMM_ERR   = 0x42,  /*��Եģ��ͨѶ����*/
	DYMOD_COMM_ERR  = 0x43, /*��Դģ��ͨѶ����*/
	EACMET_COMM_ERR = 0x44, /*������ͨѶͨѶ����*/
	
  EMOD_LOC_ERR       = 0x50, /*ģ�����*/
	EGUN_NOLINK_ERR    = 0x60,
	
	
	
}E_STEP_ERR;



#define UNLOCKED_END  1
#define LOCKED_END    0

#define GUNNO 0
#define RTN_TRUE  1
#define RTN_FALSE 0
#define RTN_WAIT  0x20
#define RTN_TMOUT 0x22
#define BCL_TMOUT 0x07
#define BCS_TMOUT 0x08

#define MIN_MOD_CURRENT  5

typedef enum
{
	E0_READY = 0,
	E1_BMSPW = 1,
	E2_ELECK = 2,
	E3_WTBHM = 3,
	E4_CHKISO = 4,
	E5_OLDGBT = 5,
	E6_DCOUT  = 6,
	E7_WTISOF = 7,
	E8_WTXFF  = 8,
	E9_WTBRM  = 9,
	E10_WTBCP = 10,
	E11_WTBRO = 11,
	E12_CHKPARA = 12,
	E13_DCOUTADJ = 13,
	E14_STRATCHG = 14,
	E15_VINCHECK = 15,
	
}E_PROCESS_CODE;

#define START_CHARGE(a) (gPtrRunData[(a)]->logic->workstep = STEP_START)
#define STOP_CHARGE(a)  (gPtrRunData[(a)]->logic->workstep = STEP_CHGEND)



/*���ǹ��Ϣ����*/
//extern void TaskMain_DealMsg(uint8 gunNo);
extern void TaskMain_DealMsg(void);

extern void TaskAGunMainCtrl_proc(CTRL_STEP* gun_ctrl, PROTO_ST *ptrProto);

extern void TaskBGunMainCtrl_proc(CTRL_STEP* gun_ctrl, PROTO_ST *ptrProto);


extern void ChargeCtrlStep(CTRL_STEP *ptrCtrl,PROTO_ST *proto);
#endif

