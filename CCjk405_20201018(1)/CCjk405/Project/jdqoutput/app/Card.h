#ifndef __CARD_H
#define __CARD_H

#define CARD_COMM_TIMEOUT		100 	/* ͨѶ��ʱ500ms */


/* �洢�������� */
typedef enum
{
	STORE_LOG = 0,	/* ���һ�γ���¼ */
	STORE_PIN	= 1,	/* �û����� */
	
	STORE_IcNo = 2,	/* IC���� */
	STORE_LockFg = 3,		/* ������־ */
	STORE_Money = 4,		/* IC����� */
	
	STORE_KEY = 5,	/* ��KEY��Կ */
	STORE_BakMoney = 6,	/* IC��������� */
	
	STORE_ESAM = 7,	/* ESAM�� */
}STROE_TYPE;

/* ����IC��Ӧ */
typedef enum
{
	RP_ERR=0,/* ͨѶ���� */
	RP_NO_IC,/* �޿� */
	RP_SUCC,/* �ɹ� */
	RP_FAIL,/* ʧ�� */
	RP_REP,/* ���佻�� */
}OPERATER_IC_RESULT;

/* ��������־ */
typedef enum  
{
	IC_UNLOCK = 0x30,		// ����
	IC_LOCK 	= 0x31,	 	// ���� 
	
	CARD_LOCKED_FG = 0x01,
	CARD_UNLOCKED_FG = 0x00,
}LOCK_CARD_STATUS;


/* ����ṹ */
typedef struct
{
	uint8 Pin[6];/* ESAM��RF������ */
	uint8 SectNo;/* ����RF�� */
	uint8 BlockNo;/* ����RF��*/
}CARD_PWD_STRUCT;

/* �����ṹ */
typedef struct
{
	uint8 SectNo;
	uint8 BlockNo;
	uint8 StoreType;
}CARD_SINGLE_SECT_STRUCT;

typedef struct
{
	uint8 UserPin[3];/* �û�PIN�� */
	
	union
	{
		uint8 Data[16];
		struct
		{/* ESAM�� */
			uint8 IcNo[8];/* IC���� */
			uint8 LockFlag;/* ������־ */
			int32 Money;	/* ����� */
		}Esam;
		struct
		{/* RF�� */
			uint8 IcNo[8];/* IC���� */
			uint8 LockFlag;/* ������־ */
			int32 Money;	/* ����� */
		}Rf;
	}Comm;
	
	uint8 Key[16];/* key��  */
	uint32 BackMoney;	/* ���ݿ���� */
}IC_DATA;


typedef union
{
	uint8 RfData[16];
	struct
	{/* ESAM�� */
		uint8 IcNo[8];/* IC���� */
		uint8 LockFlag;/* ������־ */
		uint32 Money;	/* ����� */
	}Esam;
}IC_API_DATA;

/********************ESAM�����ú���******************************/
/* Ѱ�� */
extern uint8 ESAM_CheckCard(void);
//======================================
/* У������ */
extern uint8 ESAM_VerfiyPwd(uint8 Type,IC_API_DATA *data);
/* ���� */
extern uint8 ESAM_LockIc(void);
/* �⿨ */
extern uint8 ESAM_UnLockIc(uint32 Money,IC_API_DATA *data);
/* ���佻�� */
extern uint8 ESAM_ReplenDeal(IC_API_DATA *data);
/***********************END**************************************/



/********************RF�����ú���******************************/
/* Ѱ�� */
extern uint8 RF_CheckCard(void);
/* У������ */
extern uint8 RF_VerfiySectPwd(uint8 Type,IC_API_DATA *data);
/* ���������� */
extern uint8 RF_ReadSectData(uint8 Type,IC_API_DATA *Data);
/* д�������� */
extern uint8 RF_WriteSectData(uint8 Type,IC_API_DATA *Data);
/* ��ֵ */
extern uint8 RF_MinusVaule(uint8 Type,uint32 Money);
/* ��ֵ */
extern uint8 RF_AddVaule(uint8 Type,uint32 Money);
/***********************END**************************************/


/*************************�����ú����� ��ע���������ݽṹ��ͬ���޸ĺ���******************/
extern void CARD_GetIcSecrtPin(uint8 Type,uint8 *Data);




//extern CARD_PWD_STRUCT GetPinWithStoreType(uint8 Type);



/*�쿨*/
extern uint8 RF_CheckChgCard(uint8 ComNo);


/*��ȡһ��������һ������*/
extern uint8 RF_ReadSectorData(uint8 ComNo,CARD_PWD_STRUCT *Pwd,uint8 *data);//16byte


/*дһ��������һ������*/
extern uint8 RF_WriteSectorData(uint8 ComNo,CARD_PWD_STRUCT *Pwd,uint8 *data);//16byte


/*RF��ֵ*/
extern uint8 RF_DecMoney(uint8 ComNo,CARD_PWD_STRUCT *Pwd,int32 Money);








#endif
