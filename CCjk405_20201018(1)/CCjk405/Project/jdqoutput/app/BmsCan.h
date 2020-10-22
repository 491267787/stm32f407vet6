/**
  ******************************************************************************
  * @file    BmsCan.h
  * @author  zqj
  * @version v1.0
  * @date    2018-04-09
  * @brief   
  ******************************************************************************
	*/
	
#ifndef BMSCAN_H_00001
#define BMSCAN_H_00001

#include "typedefs.h"
#include "can.h"
#define CAN_COM_MAX   2
#define CAN_COM1			CAN_1				/* ʹ�õ�CAN1 */
#define CAN_COM2			CAN_2				/* ʹ�õ�CAN2 */

extern void Init_CAN(uint8 CanNo);
extern int32 Bms_CanRead(CAN_MSG* buf,uint32 timeOut,uint8 portNo);
extern int32 Bms_CanWrite(CAN_MSG* msg,uint8 portNo);

#endif
