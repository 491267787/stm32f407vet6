/**
  ******************************************************************************
  * @file    BillingUnit.h
  * @author  zqj
  * @version v1.0
  * @date    2018-04-09
  * @brief   
  ******************************************************************************
	*/
#ifndef _BILLINGUNIT_H_0001
#define _BILLINGUNIT_H_0001

#include "typedefs.h"
#include "TaskCardReader.h"
#include "TaskMainCtrl.h"

#define  MAXSFEILVNUMS            24

#pragma pack(1)
typedef  struct
{	
	uint8 serivetype;    /*����ѷ�ʽ 0���� 1�������� 2������*/
	uint8 bespktype;     /*ԤԼ�ѷ�ʽ 0���� 1����ʱ��*/
	uint8 parktype;      /*ͣ���ѷ�ʽ 0���� 1����ʱ��*/
	uint8 lossrate;    /*��֮�� ����ֵ �ٷֱ�*/
	uint8 sectNo[48];    /*�����ʱ�η��ʺ� */
	uint8 cdsectNo[MAXSFEILVNUMS];  /*������10�η��ʱ���*/  
	
	int32 price[MAXSFEILVNUMS];     /*��� */
  int32 srvrate[MAXSFEILVNUMS];   /*�����*/
	int32 bespkrate[MAXSFEILVNUMS]; /*ԤԼ���*/
	int32 parkrate[MAXSFEILVNUMS];  /*ͣ����*/
}SECT_PRICE_ST;
	
#define MAX_HALFHOUR_SECT  48


#define MAX_CHG_SECT 48
#define BILL_PRICE_HECI 10000
#define BILL_PRICE_YIDIAN 100
#define BILL_PRICE_BASE BILL_PRICE_HECI   //����2λС��
/* �˵���¼�ṹ */
typedef  struct
{
	uint32 storeNo;        /*��¼�洢���*/
	uint8  chgtype;  			 /*��緽ʽ 1:������� 2ԤԼ���            */
	uint8  chgmod;   			 /*���ģʽ 0 �Զ� 1.��� 2.ʱ�� 3.����      */
	uint8  cardtype; 			 /*��翨����*/
	uint8  startmod;       /*����ģʽ�� 1�����߿�  2�����߿� 3����̨app*/
	uint8  cardNo[20];     /*��翨��*/
	uint8  vincode[17];    /*VIN��*/
	uint8  serialNo[32];   /*��¼��ˮ�� 77*/
	
	int32 beforemoney;     /*���ǰ���*/
	int32 aftermoney;      /*������*/
 	int32 energymoney;     /*���*/
	int32 serivemoney;     /*�����*/
	int32 parkmoney;       /*ͣ����*/
	int32 bespkmoney;      /*ԤԼ��*/
	int32 billmoney;       /*�˵���� */
	
	uint8  startsoc;       /*��ʼsoc*/
	uint8  soc ;           /*��ǰsoc*/
	
	uint32 startenergy;    /*��ʼ����*/
	uint32 stopenergy;     /*��������*/
	uint32 energy;         /*������ ��ʵ*/
	
	uint8  startsect;      /*��ʼʱ�κ�*/
	uint8  stopsect;       /*����ʱ�κ� 44*/
	uint32 sectenergy[MAX_CHG_SECT]; /*ʱ�ε��� 0.01Kwh 96*/	
	
	uint8  termid[4];      /*�������*/
	uint32 parksecs;       /*ͣ��ʱ�� ��*/
	uint8  backtype;       /*�����ʹ�õĺ�̨����*/
	uint8  gunno;          /*���ǹ��*/
	
	uint8  starttm[6];     /*��翪ʼʱ��*/
    uint8  stoptm[6];      /*������ʱ��*/
	uint32 chgsecs;        /*���ʱ�� ��*/
	
	
	uint8  endreason;      /*������ԭ��*/
	uint8  errcode;        /*������*/
	uint8  IsReport;       /*�Ƿ��ϱ�*/
	uint8  IsPay;          /*�Ƿ���� 30*/
	
	uint8  cardNo2[12];    /*�����ĺ�̨,�û�����չ��32λ,����������*/
	uint8  stopreason[16]; /*������ԭ��,16�ֽڵ�ÿ��bitλ����һ������ԭ��*/
}CALCBILL_DATA_ST; 

typedef  struct
{	
	uint8   serivetype;    /*����ѷ�ʽ 0���� 1�������� 2������*/
	uint8   bespktype;     /*ԤԼ�ѷ�ʽ 0���� 1����ʱ��*/
	uint8   parktype;      /*ͣ���ѷ�ʽ 0���� 1����ʱ��*/
	
	int32   bespkmoney;
	int32   srvmoney;
	int32   parkmoney;
	
	uint8   cursectno;     /*��ǰʱ�κ�*/
	int32   currSrvrate;   /*��ǰ�����*/
	int32   currfeerate;   /*��ǰ���� (ʵʱ���) */
	int32   parkfeerate;   /*��ǰ���� (ʵʱ���) */
	uint32  startticks;
	uint32  lasttimes;       /*ʣ����ʱ��*/
	uint32  tmpstartenergy;  /*��ʱ��ʼ����*/

}REAL_DATA_ST;

typedef  struct
{
	CALCBILL_DATA_ST bill;
	REAL_DATA_ST     real;
	
}REAL_BILL_DATA_ST;

#pragma pack()

typedef enum {
	SRV_NULL_TYPE  =  0,
	SRV_ENERGY_TYPE = 1,
	SRV_TIMES_TYPE =  2,
	
	BESPK_NULL_TYPE = 0,
	BESPK_TIME_TYPE = 1,
	BESPK_TMES_TYPE = 2,
	
	PARKT_NULL_TYPE = 0,
	PARKT_TIME_TYPE = 1,
	PARKT_TMES_TYPE = 2,
}E_FEE;




extern CALCBILL_DATA_ST * Bill_GetBillData(uint8 gunNo);
extern REAL_BILL_DATA_ST *GetFeeData(uint8 gunNo);
extern void Bill_InitData(uint8 gunNo);

/* ����ϵͳʱ�����ʱ�Σ���0ʱ�ο�ʼ */
extern uint8 Bill_FindSectNo(void);

/*����ָ��ʱ�����ʱ�κ�*/
extern uint8 Bill_FindSectNoByTime(uint8 hour,uint8 min);

/*����ָ��ʱ�����ʱ�κ�*/
extern uint8 Bill_FindSectNoByTime(uint8 hour,uint8 min);

/*���ÿ������Ϣ*/
//extern void Bill_SetCardInfo(CARD_INFO *pCardinf);

/*��ʼ���Ʒ�����*/
extern void Bill_SetStartData(uint8 gunNo);


/*���ý����Ʒ�����*/
extern void Bill_SetStopData(uint8 gunNo,uint8 reason,uint8 errcode);

/*ʵʱ�Ʒ����ݼ���*/
extern uint32 Bill_RealCalc(uint8 gunNo,uint8 minEry);

extern void Bill_SetStartInfo(START_PARAM *startpara);


extern void Bill_GetCurrentFeeReate(uint8 gunNo);


extern REAL_BILL_DATA_ST *GetFeeData(uint8 gunNo);



extern void Bill_SetCarBmsData(uint8 gunNo);
void heli_Bill_SetCarBmsData(uint8 gunNo);
























#endif

