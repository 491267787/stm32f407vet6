#ifndef _CARDREADER_H
#define _CARDREADER_H

#include "sys.h"

//////////////////////////////////////////////////////////////////////////////////	 
//���ܣ�   
//���ߣ�
//��������:2020/04/25
//�汾��V1.0									  
//////////////////////////////////////////////////////////////////////////////////

typedef struct
{
	uint8_t   cardtype; 			/*��翨����*/
	uint8_t   cardNo[20];     /* ��翨��*/
	uint8_t   userPin[3];			/* �û�PIN�� */
	uint8_t   lockFlag;				/* ������־ */
	int32_t   money;					/* ����� */
	uint8_t   key[20];				/* key��  */
	int32_t  backMoney;			/* ���ݿ���� */	
}CARD_INFO;

#pragma pack(1)
typedef  struct
{
	uint8_t sect0pwd[6];
	uint8_t sect1pwd[6];
	uint8_t sect2pwd[6];
	uint8_t keyCode[16];
	uint8_t defusrpin[8];
	uint8_t defautusrpwd[6];
}CARD_PARA;

typedef  struct
{
	CARD_PARA safe;
	//CARD_INFO info;
}CARD_PARAM_ST;

#pragma pack()

typedef  union
{
	uint8_t RfData[16];
	struct
	{
		uint8_t   ctype; /*����*/
		uint32_t  addr;
		struct{
			uint8_t year;
			uint8_t mon;
			uint8_t date;
			uint8_t hour;
			uint8_t min;
			uint8_t sec;
		}tim;
	  uint32_t money;
	  uint8_t  sum;
	}rec;
}LAST_RECORD;


typedef struct
{
	uint8_t        u8Active;    /*0��A  1��B*/
	CARD_PARA    para;
	
	CARD_INFO    info;
	
	uint8_t        checkFg;
	
	uint8_t       lockFg;
	uint8_t       unLockFg;
	
	int32_t       unlockmoney;
	uint32_t       delaytime;
	uint8_t        lkcardNo[20];     /*����������*/
	
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
	E_USER_CARD = 0,    /*������ͨ�û���*/
	E_MANG_CARD = 1,	  /*���ù���*/ 
	E_SUPER_CARD = 2,   /*����Ȩ���û���*/
	E_OFFLINE_USER_CARD = 3, /*������ͨ�û��������ַ���7*/
	
	
}E_RDCARD_TYPES;

#endif

