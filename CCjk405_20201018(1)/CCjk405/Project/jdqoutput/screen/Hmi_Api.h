/**
  ******************************************************************************
  * @file    Hmi_Api.h
  * @author  zqj
  * @version v1.0
  * @date    2016-03-09
  * @brief   
  ******************************************************************************
	*/
#include "typedefs.h"

#ifndef _HMI_API_H_0001
#define _HMI_API_H_0001


#define RT_TRUE  1
#define RT_FALSE 0

#define RT_EOK   1
#define RT_ERROR 0

#define FRAME_HEAD1      0x5A    //֡ͷ1
#define FRAME_HEAD2      0xA5     //֡ͷ2
#define MIN_FRAME_LEN    6

#define CMD_WR_REG 0x80
#define CMD_RD_REG 0x81
#define CMD_WR_VAR 0x82
#define CMD_RD_VAR 0x83
#define CMD_WR_QUX 0x84

#define REG_PIC_ID 0x03

#define CLOCK_REG_ADDR   0x0001   //
#define CLOCK_REG        0x1f     //ʱ�ӼĴ���


#define   LED_NOW_REG     0x01    //LED ���� 0x00~0x40
#define   BZ_TIME_REG     0x02    //���������� val*10
#define   PIC_ID_REG      0x03    //ҳ��Ĵ���

#define   LED_NOW_REG_LEN   1
#define   BZ_TIME_REG_LEN   1
#define   PIC_ID_REG_LEN    2


/*������������Ĵ�������*/
extern uint8 Hmi_PackQueryReg(uint8 *buf,uint16 regAdr,\
                                         uint8 nCmd,uint8 regNum);

/*�������������Ĵ�������*/
extern uint8 Hmi_PackClearReg(uint8 *obj,uint16 regAdr,uint8 nCmd,uint8 regNum);

/*���������ر��ļ��*/
extern uint8  Hmi_PackgCheck(uint8 *buf,uint8 rxLen);

/*���д�������Ĵ�������*/
extern uint8 Hmi_PackWriteReg(uint8 *obj,uint8 *src,\
                       uint16 regAdr,uint8  nCmd,uint8 byNum);
											 
/*�����л�ҳ�������*/		
extern void Hmi_ChangePicture(uint8 picNo);

/*�ı���ʾ����*/
extern void Hmi_ShowText(uint16 adr,uint16 len,uint8 *text);

/*�رյ�������*/
extern void Hmi_ShutDownKeyBoard(uint8 value);

/*���ָ��λ�ÿ�ʼ�ļĴ���*/
extern uint8 Hmi_ClearReg(uint16 regAdr,uint8 regNum);

/*����������*/
extern void Hmi_BuzzerBeep(uint8 value);

/*��������ʱ��*/
extern void Hmi_RequestScreenTime(void);

/*дһ�������Ĵ���*/
extern uint8 Hmi_WriteOneVar(uint16 regAdr,uint16 data);

/*���ô���������*/ 
extern void Hmi_SetLcdLight(uint8 value);


/*����ͼƬ��*/
extern void Hmi_RequestPicNo(void);

/*�����Ƿ��д���*/
extern void Hmi_RequestIstouch(void);

/*д����Ĵ�������*/
extern uint8 Hmi_WriteMulitVar(uint16 regAdr,uint16 *data,uint16 regNum);

/*�������*/
extern uint8 Hmi_CheckIstouch(void);

/*�رձ���*/
extern void Hmi_CloseBackLight(void);

extern void Hmi_RegStrCheck(uint8 *str,uint8 len);

/*��ȡ��ǰ��Ļ��*/
extern uint8 Hmi_GetCurrPicNo(void);

/*��ȡ���������ص��ַ���*/
extern uint8 Hmi_GetRcvString(char *objSrc, uint8 *buf,uint8 len);

											 

											 
























#endif


