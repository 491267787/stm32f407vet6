/**
  ******************************************************************************
  * @file    TaskCardReader.h
  * @author  zqj
  * @version v1.0
  * @date    2018-04-09
  * @brief   
  ******************************************************************************
*/
#ifndef _TASKCARDREADER_H_0001
#define _TASKCARDREADER_H_0001
#include "typedefs.h"



typedef struct
{
	uint8   cardtype; 			/*��翨����*/
	uint8   cardNo[20];     /* ��翨��*/
	uint8   userPin[3];			/* �û�PIN�� */
	uint8   lockFlag;				/* ������־ */
	int32   money;					/* ����� */
	uint8   key[20];				/* key��  */
	int32  backMoney;			/* ���ݿ���� */	
}CARD_INFO;

#pragma pack(1)
typedef  struct
{
	uint8 sect0pwd[6];
	uint8 sect1pwd[6];
	uint8 sect2pwd[6];
	uint8 keyCode[16];
	uint8 defusrpin[8];
	uint8 defautusrpwd[6];
}CARD_PARA;


typedef  struct
{
	CARD_PARA safe;
	//CARD_INFO info;
}CARD_PARAM_ST;

#pragma pack()



typedef  union
{
	uint8 RfData[16];
	struct
	{
		uint8   ctype; /*����*/
		uint32  addr;
		struct{
			uint8 year;
			uint8 mon;
			uint8 date;
			uint8 hour;
			uint8 min;
			uint8 sec;
		}tim;
	  uint32 money;
	  uint8  sum;
	}rec;
}LAST_RECORD;




//typedef  struct
//{
//	LAST_RECORD lastRec;
//	uint8       usrpwd[3];
//	uint8       CardNo[8];
//	uint8       lockfg;
//	int32       money;
//	//uint8     keyCode[16];
//	int32       bakmoney;
//}CARD_INFO;



typedef struct
{
	uint8        u8Active;    /*0��Aǹ 1��Bǹ*/
	CARD_PARA    para;
	
	CARD_INFO    info;
	
	uint8        checkFg;
	
	uint8       lockFg;
	uint8       unLockFg;
	
	int32       unlockmoney;
	uint32       delaytime;
	uint8        lkcardNo[20];     /*����������*/
	
}CARD_OPER_DATA;

typedef enum
{
	ERR_NULL = 0,
	ERR_KEY = 1,
	ERR_USRPWD = 2,
	ERR_READ = 3,
	ERR_NOCARD = 4,
	ERR_ERRCARD = 5,
	ERR_MANGERCARD = 6,/*����*/
}E_RTNCARDINFO;

typedef enum
{
	E_USER_CARD = 0,   //������ͨ�û���
	E_MANG_CARD = 1,	//���ù��� {0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x88};
	E_SUPER_CARD = 2,   //���ӳ����Ĺ������ͣ������������ ���ַ���8��patli 20190926 add
	E_OFFLINE_USER_CARD = 3, //������ͨ�û��������ַ���7
	
	
}E_RDCARD_TYPES;

extern CARD_INFO *TskCard_GetCardInfPtr(void);




/*����������*/
extern void TaskCardReader(void *p_arg);


extern uint8 TskCReader_CpyLockCardNo(uint8 * CardNo);

extern void TskReader_Init(void);

//extern void CardReader_MsgProc(MSG_STRUCT *msg);

extern void TaskCardReader_main_proc(void);



#endif


