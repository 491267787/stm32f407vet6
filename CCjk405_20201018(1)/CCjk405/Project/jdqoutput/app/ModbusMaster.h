/**
  ******************************************************************************
  * @file    ModbusMaster.h
  * @author  zqj
  * @version v1.0
  * @date    2018-04-09
  * @brief   
  ******************************************************************************
	*/

#ifndef _MODBUSMASTER_H_0001
#define _MODBUSMASTER_H_0001
#include "typedefs.h"

#define MODBUS_CMD_03  0x03
#define MODBUS_CMD_06  0x06


/*Modbus 06����д�����Ĵ��� ���ݴ��*/
uint8 Modbus_SingleRegPkg(uint8 *buf,uint8 devadr,uint8 cmd,uint16 wreg,uint16 val);
/*Modbus 06����д�����Ĵ��� ������*/
extern uint8 Modbus_WritSingleRegPkg(uint8 *buf,uint8 devadr,uint16 wreg,uint16 val);

/*Modbus 03������Ĵ��� ������*/
extern uint8 Modbus_ReadPkg(uint8 *buf,uint8 devadr,uint16 startadr,uint8 regNum);

/*Modbus У������ 03 06 ������У�� 03��ȷ���� ����� 06 ����1 �����󷵻�0*/
extern uint8 Modbus_CheckReadPkg(uint8 *buf,uint8 devadr,uint8 cmd,uint8 regNum,uint8 len ) ;

























#endif



