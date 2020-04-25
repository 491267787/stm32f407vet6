
/**
  ******************************************************************************
  * @file    crc.h
  * @author  
  * @version v1.0
  * @date    
  * @brief  
  ******************************************************************************
	*/
	
#ifndef _CRC_H_0001
#define _CRC_H_0001

#include "includes.h"
	
	/*����crc16���*/
//	unsigned short Crc16_Calc_for_screen(unsigned char *updata, unsigned char len);
//	unsigned short Crc16_Calc_for_other(unsigned char *updata, unsigned short len);
unsigned short Crc16_Calc(unsigned char *updata, unsigned short len);
	
	
uint32_t Calc_CRC32_Crypto(uint8_t pStart[], int32_t uSize);
	
	

	
#endif
	

