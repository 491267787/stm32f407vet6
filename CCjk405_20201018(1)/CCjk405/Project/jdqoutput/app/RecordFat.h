/**
  ******************************************************************************
  * @file    RecordFat.h
  * @author  zqj
  * @version v1.0
  * @date    2018-04-09
  * @brief   
  ******************************************************************************
	*/
#ifndef __RECORDFAT_H_0001
#define __RECORDFAT_H_0001
#include "typedefs.h"
#include "BillingUnit.h"

#pragma pack(1)

typedef enum
{
	SST_IC = 0,
	FM_IC    = 1,
}E_STOREDEV;

typedef  struct
{
	uint16 head;
	CALCBILL_DATA_ST bill;
	uint16 crc;
}RECORD_ST;

typedef  struct
{
	uint16 head;
	uint8  nType;         /*�洢����*/
	uint16 tablrdno;      /*�洢���¼��*/
	uint32 currrdno;      /*��ǰ��¼��*/
	uint32 totlrdno;      /*�ܼ�¼��  */
	uint32 baktolrdno;    /*���ݼ�¼��*/
	uint16 crc;           /*У���*/
}RD_PARA_ST;  /*�ýṹ����С��32byte ,����������*/

#pragma pack()

extern void Wdg_feeddog(void);
	
#define SINGLE_SECT_LEN							  (4096UL)	 /* ������������ */

//#define MAX_SECT_NO_FULL_NUM     		40
//#define MAX_SECT_FULL_NUM        		200

#define SINGLE_RECORD_LEN			   			(sizeof(RECORD_ST))		/* ������¼���� */
//#define SINGLE_SECT_REORD_NUM		 			(SINGLE_SECT_LEN/SINGLE_RECORD_LEN)		/* ���������洢��¼������ *�� */

//#define MAX_NO_FULL_NUM	  			  		(MAX_SECT_NO_FULL_NUM * SINGLE_SECT_REORD_NUM)
//#define MAX_FULL_NUM	     			  		(MAX_SECT_FULL_NUM * SINGLE_SECT_REORD_NUM)

#define SINGLE_PARA_LEN						  	(sizeof(RD_PARA_ST))		/* ������¼�������� */
#define MAX_RECORD_PARA_NUM				  	(SINGLE_SECT_LEN/SINGLE_PARA_LEN)		/* ���ļ�¼��������  1������*/

#define SINGLE_SYS_PARA_LEN				   	sizeof(SET_PARA)	/* ����ϵͳ�������� */ 
#define MAX_SYS_PARA_NUM					   	(SINGLE_SECT_LEN/SINGLE_SYS_PARA_LEN)		/* ����ϵͳ�������� 1������*/

#define ONLINE_SINGLE_PARA_LEN				(sizeof(ONLINE_RECORD_PARA))		/* ������¼�������� */
#define ONLINE_MAX_RECORD_PARA_NUM		(SINGLE_SECT_LEN/ONLINE_SINGLE_PARA_LEN)		/* ���ļ�¼�������� */

#define ONLINE_MAX_SECT_NUM	          10
#define ONLINE_SINGLE_RECORD_LEN			(sizeof(ONLINE_BILL_STRUCT))													/* ������¼���� */
#define ONLINE_SINGLE_SECT_REORD_NUM	(SINGLE_SECT_LEN/ONLINE_SINGLE_RECORD_LEN) 					  /* ���������洢��¼���� 4096/180= 22.75 �洢22�� */
#define ONLINE_MAX_RECORD_NUM					(ONLINE_MAX_SECT_NUM * ONLINE_SINGLE_SECT_REORD_NUM)	/* �洢��¼������ */

/********************************************************************
����������PARAM_DEV_TYPE   	�豸����  						   
          PARAM_OPER_TYPE  	���в���  						 
          PARAM_FEE_TYPE   	���ʲ���(��1108�ֽڣ�  
					�洢����4K byte�ռ�
*********************************************************************/
/* �洢�ռ䶨�� */
#define STORE_HEAD                0xF5A5
#define STORE_USE_HEAD_ADDR				0x7FF000		  /* �״γ�ʼ����־�ռ� */
#define STORE_USE_HEAD_FLAG				0xA5A5B5B4	  /* �״γ�ʼ����־ֵ */

#define STORE_BASE_ADDR						(0x000000UL)	/* �洢����ַ */

/*�����洢������ռһ������*/
#define SYS_DEV_PARA_ADDR					STORE_BASE_ADDR/* ϵͳ�����洢��4KB */
#define SYS_DEV_PARA_SIZE					0x1000  /*1������*/

#define SYS_OPER_PARA_ADDR				(SYS_DEV_PARA_ADDR+SYS_DEV_PARA_SIZE)/* ϵͳ�����洢��4KB */
#define SYS_OPER_PARA_SIZE				0x1000  /*1������*/

#define SYS_FEE_PARA_ADDR					(SYS_OPER_PARA_ADDR+SYS_OPER_PARA_SIZE)/* ϵͳ�����洢��4KB */
#define SYS_FEE_PARA_SIZE					0x1000  /*1������*/

/**/
#define RECORD_PARA_ADDR					(SYS_FEE_PARA_ADDR+SYS_FEE_PARA_SIZE)/* ��¼���Ʋ����洢��4KB*/
#define RECORD_PARA_SIZE					0x1000

#define RECORD_TEMP_ADDR					(RECORD_PARA_ADDR + RECORD_PARA_SIZE)/* ��ʱ��¼�洢�� 4KB*/
#define TEMP_SIZE									0x1000

#define RECORD_NO_FULL_ADDR				(RECORD_TEMP_ADDR + TEMP_SIZE) /* δ������¼�洢��  1.5MB*/
#define NO_FULL_SIZE							MAX_SECT_NO_FULL_NUM*0x1000

#define RECORD_FULL_ADDR					(RECORD_NO_FULL_ADDR + NO_FULL_SIZE) /* ������¼�洢�� 2MB */
#define FULL_SIZE									MAX_SECT_FULL_NUM*0x1000

/*****************************������¼�궨��*****************************************/
#define ONLINE_PARA_ADDR					(RECORD_FULL_ADDR + FULL_SIZE)/* ������¼�����洢�� */
#define ONLINE_PARA_SIZE					0x1000

#define ONLINE_TEMP_ADDR					(ONLINE_PARA_ADDR+ONLINE_PARA_SIZE)/* ������ʱ��¼��ַ */
#define ONLINE_TEMP_SIZE					 0x1000

#define ONLINE_RECORD_ADDR				(ONLINE_TEMP_ADDR+ONLINE_TEMP_SIZE)/* ������¼�洢�� */
#define ONLINE_RECORD_SIZE				(ONLINE_MAX_SECT_NUM*0x1000)
/*********************************END***********************************************/

/*********************************EEROM******************************************/
/*1Kbyte һ�飬��8�� */
#define EFULL_RECORD                0x3A
#define ETEMP_RECORD                0x3B

/*��¼������*/
#define  EEROM_TEMPPARATABL_ADR     0
#define  EEROM_TEMPBACK1TABL_ADR    32
#define  EEROM_FULLPARATABL_ADR     64
#define  EEROM_FULLBACK1TABL_ADR    96      

/*������*/
#define  EEROM_OPERPARA_ADR         (128 + 64)  /* ʵ��52�ֽ�  */
#define  EEROM_DEVPARA_ADR          (256 + 64)  /* ʵ��168�ֽ� */
#define  EEROM_FEEPARA_ADR          (700 + 64)  /* ʵ��464�ֽ� */ 

/*��ʱ��¼��*/
#define  EEROM_TEMPRD_ADR           1280 
#define  EEROM_TEMPRD_SIZE          2048
#define  MAX_TEMPRD_NUM             (EEROM_TEMPRD_SIZE / SINGLE_RECORD_LEN)     /*8����¼*/

#ifdef CP_PERIOD_FEE
#define  EEROM_CP_PEROID_FEE_ADR           (EEROM_TEMPRD_ADR+EEROM_TEMPRD_SIZE)  //3328
#define  EEROM_CP_PEROID_FEE_SIZE          149
#endif


//END 3328 (EEROM_CP_PEROID_FEE_ADR+EEROM_CP_PEROID_FEE_SIZE)

/*FMIC��ʷ��¼��*/
/*��λ�����ⲿ��SST�洢�� һ����¼��251 + 16�ֽ�*/
#define  EEROM_RECORD_ADR       		0
#define  EEROM_RECORD_SIZE      		(3072 * 1024)
#define  MAX_RECORD_NUM         		(EEROM_RECORD_SIZE / SINGLE_RECORD_LEN)    /*10000����¼*/

/*****************************************************************************/

/*
** ��������������ʱgRunParam�ṹ�е���������
*/
extern uint8 Rd_SaveSysParam(void);

/*
** ����������gRunParam�ṹ�е���������
*/
extern uint8 Rd_ReadSysParam(void);


/*��ʼ���洢��¼����*/
extern void Rd_InitRecord(void);
extern void Rd_SaveRdCrlParam(RD_PARA_ST *SaveTabl) ;
extern uint16 Rd_ReadRdCtrlPara(RD_PARA_ST *SaveTabl);
extern uint16 Rd_SaveRecord(RD_PARA_ST *SaveTabl,CALCBILL_DATA_ST *bill);
/*��ȡָ����¼�ŵļ�¼����¼��1--n,��ǰ��¼Ϊ��һ��Ϊ��һ����¼�����ض�ȡ�ļ�¼�ţ�0��δ��ȡ��*/
extern uint16 Rd_ReadCurrentRecord(uint8 rdno,CALCBILL_DATA_ST *bill);
extern uint8 Rd_InitFlashData(void);

/*******************E2ROM*************************************************/
extern int Rd_E2romSaveCtrlParam(uint8 devtype, RD_PARA_ST *E2romTabl,uint16 addr);
extern int Rd_E2romReadCtrlParam(uint8 devtype, RD_PARA_ST *E2romTabl,uint16 addr);
extern uint16 Rd_E2romSaveTempRecord(RD_PARA_ST *E2romTabl,CALCBILL_DATA_ST *bill);

extern uint16 Rd_E2romSaveFullRecord(RD_PARA_ST *E2romTabl,CALCBILL_DATA_ST *bill);

/*��ȡָ����¼�ŵļ�¼����¼��1--n�����ض�ȡ�ļ�¼�ţ�0��δ��ȡ��*/
extern uint16 Rd_E2romReadRecord(uint16 absNo,CALCBILL_DATA_ST *bill);
extern uint16 Rd_SaveOneRecord(uint8 rdno,CALCBILL_DATA_ST *bill);

extern uint8 Rd_InitE2romData(void) ;
extern uint8 Rd_ClearE2romRecordData(void);

/*��ѯһ��δ�ϴ���¼*/
extern uint8 Rd_FindUnAccountRecord(CALCBILL_DATA_ST bill[]);

/*����ϴ���־*/
extern uint8 Clear_RecordIsReport(uint8  *BillserialNo);
/*����ϴ���־ �޸ļ�¼����*/
extern uint8 Clear_RecordIsReportBird(uint8  *BillserialNo,uint8 *buf);

/*��ȡδ�����¼*/
extern uint16 Rd_E2romReadCurrTempRecord(RD_PARA_ST *E2romTabl,CALCBILL_DATA_ST *bill);


extern uint16 Rd_E2romTempToFullSave(uint8 gunNo);
extern uint16 Rd_E2romTempToFullBillBird(uint8 gunNo,CALCBILL_DATA_ST bill[]);

extern void Rd_E2romClearTempRecord(RD_PARA_ST *E2romTabl);

extern uint8 Rd_IsHaveTempRecord(void);

/*******************************************************/
extern uint8 Rd_SaveDevParam(uint8 devtype);

extern uint8 Rd_SaveOperParam(uint8 devtype);

extern uint16 Rd_SaveFeePara(uint8 devtype);

extern uint32 Rd_GetMaxRecord(void);

/*��֮���������*/
extern uint16 SST_Rd_E2romSaveUpData(uint16 num,uint8 *buf);
extern uint16 SST_Rd_E2romSaveUpDataHead(uint8 *buf,uint16 len);


#endif

