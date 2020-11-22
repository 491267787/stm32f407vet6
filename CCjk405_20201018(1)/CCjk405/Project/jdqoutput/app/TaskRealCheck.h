/**
  ******************************************************************************
  * @file    RealCheck.h
  * @author  zqj
  * @version v1.0
  * @date    2018-04-09
  * @brief   
  ******************************************************************************
	*/

#ifndef __CHECKSTATUS_H_0001
#define __CHECKSTATUS_H_0001

#include "typedefs.h"

#define CHECK_TRUE  1
#define CHECK_FALSE 0

#define MAX_ERR_BYTE 15

/*���֧��120��������*/
typedef enum
{
	ECODE_NORMAL = 0,    			/*�������*/
	ECODE1_ELOCK  = 1,   			/*����������  */
	ECODE2_BHMTM  = 2,   			/*BHM��ʱ*/
	ECODE3_XFTM   = 3,   			/*й�ų�ʱ*/
	ECODE4_BRMTM  = 4,   			/*BRM��ʱ */
	ECODE5_BCPTM  = 5,   			/*BCP��ʱ*/
	ECODE6_BROTM  = 6,   			/*BRO��ʱ*/
	ECODE7_BCLTM  = 7,   			/*BCL��ʱ*/
	ECODE8_BCSTM  = 8,   			/*BCS��ʱ*/
	ECODE9_BSTTM  = 9,  		  /*BST��ʱ*/
	ECODE10_CSTTM  = 10,  		/*CST��ʱ*/
	ECODE11_BSDTM  = 11,   		/*BSD��ʱ*/
	ECODE12_KM10V  = 12,     	/*�Ӵ�������ѹ����10V*/
	ECODE13_KM60V  = 13,     	/*й�ŵ�ѹ������60V*/
	EOCDE14_PARAM  = 14,     	/*������������*/
	ECODE15_PKGVOLT= 15,    	/*���ĵ�ѹ��ƥ��*/
	ECODE16_BATSTU = 16,      /*���״̬����*/
	ECODE17_FORBID = 17,      /*����ֹ*/
	ECODE18_CC1LNK = 18,      /*���ǹ cc1���Ӵ���*/
	ECODE19_MODVOLT= 19,      /*ģ���ѹ��������*/
	ECODE20_INOUT10V = 20,    /*�Ӵ���������ѹ��С��10V*/
	ECODE31_BMSCOMM= 31,      /*BMSͨѶ����*/
	
	ECODE32_GUNTMPOVER = 32,  /*���ǹ����*/
	/*��Ե��Ԫ*/
	ECODE35_ISOCOMM =35,      /*��Ե��ԪͨѶ����*/
	ECODE36_1POSERR= 36,      /*һ·���Եع���*/
	ECODE37_1NEGERR= 37,      /*һ·���Եع���*/
	ECODE38_2POSERR= 38,      /*��·���Եع���*/
	ECODE39_2NEGERR= 39,      /*��·���Եع���*/
	
	ECODE41_MODCOMMERR = 41,  /*ģ��ͨѶ����*/
	ECODE42_DC1OVER =42,      /*ֱ�������ѹ*/
	ECODE43_DC2OVER =43,      /*2ǹֱ�������ѹ*/ 
	ECODE44_DC3OVER =44,      /*3ǹֱ�������ѹ*/ 
	ECODE45_DC4OVER =45,      /*4ǹֱ�������ѹ*/ 
	
	
	
	/*BMS*/
	ECODE50_BMSFINISH = 50,   /*�յ�BMSֹͣ����,�ﵽSOC�趨ֵ*/ 
	ECODE51_BMSTVOLT=51,      /*�յ�BMSֹͣ����,�ﵽ�ܵ�ѹ�趨ֵ*/   
	ECODE52_BMSSVOLT=52,      /*�յ�BMSֹͣ����,�ﵽ�����ѹ�趨ֵ*/ 
	ECODE53_BMSCST  =53,      /*�յ�BMSֹͣ����,����������ֹ*/ 

	ECODE55_BMSISO  = 55,      /*�յ�BMS��Ե���ϱ���*/ 
	ECODE56_BMSLKTMP= 56,      /*�յ�BMS������������¶ȹ���*/ 
	ECODE57_BMSPARTTMP=57,     /*�յ�BMSԪ������*/ 
	ECODE58_BMSCLKERR =58,     /*�յ�BMS�������������*/
	ECODE59_BMSBATTMP = 59,    /*�յ�BMS������¶ȹ���*/ 
	ECODE60_BMSKMERR  = 60,    /*�յ�BMS��ѹ�̵�������*/ 
	ECODE61_BMSCHK2ERR= 61,    /*�յ�BMS����2��ѹ������*/ 
	ECODE62_BMSOTHERERR=62,    /*�յ�BMS��������*/ 
	ECODE64_BMSCURROVER=64,    /*�յ�BMS�����������*/ 
	ECODE65_BMSVOLTERR = 65,   /*�յ�BMS��ѹ�쳣����*/ 
	
	ECODE67_BMSCRMTM  = 67,    /*BMS���ճ���CRM 00���ĳ�ʱ*/
	ECODE68_BMSCRMATM = 68,    /*BMS���ճ���CRM AA���ĳ�ʱ*/
	ECODE69_BMSCTSTM  = 69,    /*BMS���ճ���CTS��CML���ĳ�ʱ*/
	ECODE70_BMSCROTM  = 70,    /*BMS���ճ���CRO���ĳ�ʱ*/
	ECODE71_BMSCCSTM  = 71,    /*BMS���ճ���CCS���ĳ�ʱ*/
	ECODE72_BMSCSTTM  = 72,    /*BMS���ճ���CST���ĳ�ʱ*/
	ECODE73_BMSCSDTM  = 73,    /*BMS���ճ���CSD���ĳ�ʱ*/
	ECODE74_BMSST10MIN = 74 ,  /*�����ͣ����10����*/
	
	//SPN3521 ������ֹ���ԭ��
	ECODE80_CHGFINISH = 80,    /*�ﵽ�����趨������ֹ*/
	ECODE81_CHGHUMST  = 81,    /*�˹�����ֹͣ*/
	ECODE82_CHGERRST  = 82,    /*����ֹͣ*/
	ECODE83_CHGBST    = 83,    /*�յ�BMS��ֹ����ֹͣ*/
	ECODE84_NOMONEY   = 84,    /*�˻�����*/
	
	//SPN3522 ������ֹ������ԭ��
	ECODE85_CHGTMOVER = 85,    /*�������¶ȹ���*/
	ECODE86_CHGLKERR  = 86,    /*�������������*/
	ECODE87_CHGINTMPERR = 87,  /*�����ڲ����¹���*/
	ECODE88_CHGENERGY = 88,    /*�������ܴ���*/
  ECODE89_CHGJTST   = 89,    /*������ͣ����*/
	ECODE90_CHGOTHER  = 90,    /*��������*/
	
	//SPN3523 ������ֹ������ԭ��
	ECODE91_CHGCURRERR= 91,     /*��������ƥ��*/
	ECODE92_CHGVOLTERR= 92,     /*������ѹ�쳣*/
	
	/*DC Meter*/
	ECODE93_METERCOMMERR=93,    /*ֱ����ͨѶ����*/
	ECODE94_CURROVER = 94,      /*ֱ���������*/  
	
	/*AC METER*******/
	ECODE95_ACMETCOMMERR = 95,   /*������ͨѶ����*/
	ECODE96_ACVOVERR  = 96,      /*���������ѹ*/
	ECODE97_ACUnerERR = 97,
	ECODE98_ACDEFICY = 98,       /*����ȱ��*/
	ECODE99_DooracsERR = 99,
	ECODE100_DoorafERR = 100,    /*�Ž� ����*/
	ECODE101_DCFAUSEERR = 101,   /*ֱ�������˿����*/   //20181206 add
	ECODE102_SPDERR = 102,       /*����������*/         //20181206 add
	ECODE103_GUNOVERTEMPER = 103,     /*ǹ���¶ȹ���*/
	ECODE104_PAVOUTSIDEREVERSE = 104,  /*ǹ����ѹ����(BMS��)*/
	ECODE105_PAVINSIDEREVERSE = 105,   /*ǹ�ڲ��ѹ����(ģ���)*/
	ECODE106_VINBACKTM = 106,   /*��̨��ȡVIN��Ϣ��ʱ*/
	ECODE107_BACKOFF = 107,   /*��̨���ӶϿ�*/
	ECODE108_BCPOVERVOLT = 108, /*����ѹ����BCP�����������ֵ*/
}E_ERR_CODE;

typedef enum
{
	ENULL_REASON = 0, /*δ֪ԭ��ֹͣ*/
	ECARD_REASON = 1, /*ˢ��ֹͣ*/
	EAPP_REASON  = 2, /*�ƶ��ͻ���ֹͣ*/
	EERR_REASON  = 3, /*����ֹͣ*/
	EAUTO_REASON = 4, /*�ﵽ�趨����*/
	EBMS_REASON  = 5, /*BMS����ֹͣ*/
	ESYS_PWDOWN  = 6, /*ϵͳ����*/
	KEY_REASON  = 7,  /*����������ֹͣ*/
	
}E_STOPREASON_CODE;

typedef struct {
	uint16 temper1;
	union {
		  uint32 word;
			uint8  byte[4];
			struct { 
			uint32	 stop    : 1;   /*0��ͣ*/
			uint32	 acqf    : 1;   /*1������·��*/
			uint32	 ackm    : 1;   /*2�����Ӵ���*/
			uint32	 spd     : 1;   /*3������*/
			uint32	 posfuse : 1;   /*4����˿*/
			uint32	 negfuse : 1;   /*5����˿*/
			uint32	 dooracs : 1;   /*6�Ž�1*/	
			uint32	 dooraf  : 1;   /*7�Ž�2*/
			uint32	 fan     : 1;   /*8���*/
			uint32	 smoke   : 1;   /*9����澯*/
			uint32	 water   : 1;   /*10ˮ���澯*/
			uint32	 key1    : 1;   /*11���뿪��1*/
			uint32	 key2    : 1;   /*12���뿪��2*/
			uint32	 key3    : 1;   /*13���뿪��3*/
			uint32	 key4    : 1;   /*14���뿪��4*/
			uint32	 dckmp   : 1;   /*15 1#��ֱ���Ӵ���״̬*/
			uint32   dckmn   : 1;   /*16 1#��ֱ���Ӵ���״̬*/
			uint32	 dc2kmp   : 1;   /*17 2#��ֱ���Ӵ���״̬*/
			uint32   dc2kmn   : 1;   /*18 2#��ֱ���Ӵ���״̬*/	
			uint32   rsv     : 13;
	    }bits;
	}statu;
}DEV_INPUT_TYPE;

typedef struct {
	 uint16 u16temp;
	 union {
		uint16 word;
			uint8  byte[2];
			struct { 
				uint16 cc1stu     : 2; /* 1: cc1 4V; 2: cc1 6v; 3: cc1 12v */
				uint16 elockstu   : 1; /* 2 ������״̬*/
		    uint16 elockctrl  : 1; /* 3 ����������*/
				uint16 overtemper : 1; /* 4 ���¶�ֵ*/
				uint16  kmpstu    : 1; /* 5 �Ӵ�����״̬*/
				uint16  kmnstu    : 1; /* 6 �Ӵ�����״̬*/
				uint16  bcpovervolt : 1; /* 7 ������ѹ����BCP������ѹ */
				uint16 rsv        : 8;
	    }bits;
	}statu;
  int32 i32temper[2];

}DEV_GUN_TYPE;


/*���CC1״̬*/
extern uint8 Check_CC1Statu(uint8 gunNo);
/*��������״̬*/
extern uint8 Check_ElockStatu(void);
/*���K3K4״̬*/
extern uint8 Check_K3K4Statu(void);
/*�жϽӴ�������ѹ�Ƿ�С��ĳ��ֵ*/
extern uint8 Check_KmOutVoltLess(uint8 gunNo,uint16 volt);
/*��⳵����ѹ�Ƿ��뱨����� <��5%*/
extern uint8 Check_CarVolt(uint8 gunNo);
/*���Ӵ����ڲ��ѹ�Ƿ��ڽӴ�������ѹ 10V����*/
extern uint8 Check_KmInAndOut(uint8 gunNo);
/*��⳵�������Ƿ���� */
extern uint8 Check_CarParam(uint8 gunNo);
/*���ֱ������Ӵ���*/
extern uint8 Check_KmStatu(void);
/*���߾�Ե��������ѹ*/
extern uint16 Check_SuitableVolt(uint8 gunNo);
/*���ǹ��������*/
extern DEV_GUN_TYPE *Check_GetGunDataPtr(uint8 gunNo);



/*�жϽӴ����ڲ��ѹ�Ƿ�С��ĳ��ֵ*/
extern uint8 Check_KmInVoltLess(uint8 gunNo,uint16 volt);

extern uint8 Check_BcpToModSetVolt(uint8 gunNo);


/*�ж�ģ�������ѹ�Ƿ���ʵ�ʲ������*/
extern uint8 Check_OutVoltIsOk(uint8 gunNo,uint16 volt);


extern uint8 Check_SetErrCode(uint8 gunNo,E_ERR_CODE errCode);

/*������״̬���Ƿ�����*/
extern uint8 *GetgErrBitData(uint8 gunNo);

extern uint8 Check_GetErrCode(uint8 gunNo,E_ERR_CODE errCode);

extern uint8 Check_ClearErrBit(uint8 gunNo,E_ERR_CODE errCode );

extern uint8 Check_ClearAllErrBit(uint8 gunNo);

extern uint8 Check_GetErrBit(uint8 gunNo,uint8 *buf,uint8 bufsize,uint8 *num);

extern void SoftReset(void);

extern void resetInput(void);



extern void InitCC1ADCValue(uint8 Gunno);
extern uint8 Check_AllInputIo(void);
extern void Check_TranIOStatus(void);
extern void Check_stopBtn(void);
extern void Check_DialcodeBoard(void);
extern void Check_GunTemper(void);
extern void Check_BcpDCOverVolt(void);
extern void CheckGunFirstConnc(uint8 gunno);

#ifdef STM32F10X_HD

extern uint8 Check_N1_2_3_InputIo(void);

extern void Check_SoftRSTKeyBoard(void);
#endif

#endif

