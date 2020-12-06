/****************************************************************************************
Filename: 
Author: 
Datetime: 
Version: 
****************************************************************************************/

#ifndef __ADCCALC_H
#define __ADCCALC_H
#include "typedefs.h"
//#include "M_Global.h"

#define  JY_START 0x0A
#define  JY_STOP  0x00

#define RTN_JYCONTINUE  1
#define RTN_JYEND       2
#define RTN_ERR         3


#define RELAY1  GPIO_Pin_10
#define RELAY2  GPIO_Pin_11




typedef union
{
  uint16 JyCtrl;
  struct
  {
     uint16 b1Allow:4;   /*һ·0x0A������0x00ֹͣ */
     uint16 b2Allow:4;   /*��·0x0A������0x00ֹͣ */
     uint16 bchkTm:8;    /*bit8-15�����ʱ��S*/
  }sbit;
}ST_JYCTRL;


typedef struct
{
//	uint16 uab;    /*Uab*/
//	uint16 ubc;    /*Ubc*/
//	uint16 uca;    /*Uca*/
//	int32 viso[2];  /*��Եһ·�������Եص�ѹ*/
//	int32 vdc[4];   /*ĸ��һ·��ѹ 1,2·������Ե��3,4·ֻ�ɼ�ֱ����ѹ*/
    uint16 visopos;  /*Aĸ�����Եص�ѹ*/
    uint16 visoneg;  /*Aĸ�߸��Եص�ѹ*/
    uint16 visodiff; /*Aĸ��ѹ��*/
    uint16 Respos;   /*Aĸ�����Եص���*/  
    uint16 Resneg;   /*Aĸ�߸��Եص���*/
//	uint16 vdciso[2];   /*vdciso[0]Ϊ��ɼ����ĽӴ����ڲ��ѹ��vdciso[1]Ϊ����ѹ*/
		int32 vdciso[2];   /*vdciso[0]Ϊ��ɼ����ĽӴ����ڲ��ѹ��vdciso[1]Ϊ����ѹ*/
//    uint16 xfStatu;      /*��ѹй�ſ���״̬ bit0��һ·  bit1����·*/
    uint32 ResSeting;    /*��Ե�����趨ֵ*/
	
	  uint16 temprature;  /*�¶�*/
    uint16 cc1;  /*cc1*/
	  
    uint16 guntemprature[2];  /*ǹ�¶�2������ֵ*/
}ST_SAMPDATA;

ST_SAMPDATA *AdcCalc_GetValue(void);
void sys_ADC1_Init(void);
#endif                                                  
