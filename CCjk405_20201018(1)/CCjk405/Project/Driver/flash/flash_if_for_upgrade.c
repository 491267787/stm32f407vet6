/****************************************************************************************
���ڰ��͵����������޹�˾

Filename: flash_if_for_upgrade.c
Author: patli
Datetime: 20200206
Version: v0.01
****************************************************************************************/
#include "flash_if_for_upgrade.h"

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Unlocks Flash for write access
  * @param  None
  * @retval None
  */
void FLASH_If_Init(void)
{ 
  FLASH_Unlock(); 

  /* Clear pending flags (if any) */  
  FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
                  FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);

  FLASH_DataCacheCmd(DISABLE);
  
}

/**
  * @brief  Locks Flash for write unaccess
  * @param  None
  * @retval None
  */
void FLASH_If_Lock(void)
{
	FLASH_DataCacheCmd(ENABLE);

  FLASH_Lock(); 

}

/**
  * @brief  This function writes a data buffer in flash (data are 32-bit aligned).
  * @note   After writing data buffer, the flash content is checked.
  * @param  FlashAddress: start address for writing data buffer
  * @param  Data: pointer on data buffer
  * @param  DataLength: length of data buffer (unit is 32-bit word)   
  * @retval 0: Data successfully written to Flash memory
  *         1: Error occurred while writing data in Flash memory
  *         2: Written Data in flash memory is different from expected one
  */
uint32 FLASH_If_WriteByte(__IO uint8* FlashAddress, uint8* Data ,uint32 DataLength)
{
  uint32 i = 0;

  for (i = 0; i < DataLength; i++)
  {
    /* Device voltage range supposed to be [2.7V to 3.6V], the operation will
       be done by word */ 
    if (FLASH_ProgramByte(*FlashAddress, *(uint8*)(Data+i)) == FLASH_COMPLETE)
    {
     /* Check the written value */
      if (*(uint8*)*FlashAddress != *(uint8*)(Data+i))
      {
        /* Flash content doesn't match SRAM content */
        return(2);
      }
      /* Increment FLASH destination address */
      *FlashAddress += 1;
    }
    else
    {
      /* Error occurred while writing data in Flash memory */
      return (1);
    }
  }

  return (0);
  
}



/**
  * @brief  This function writes a data buffer in flash (data are 32-bit aligned).
  * @note   After writing data buffer, the flash content is checked.
  * @param  FlashAddress: start address for writing data buffer
  * @param  Data: pointer on data buffer
  * @param  DataLength: length of data buffer (unit is 32-bit word)   
  * @retval 0: Data successfully written to Flash memory
  *         1: Error occurred while writing data in Flash memory
  *         2: Written Data in flash memory is different from expected one
  */
uint32 FLASH_If_WriteDWord(__IO uint32* FlashAddress, uint32* Data ,uint32 DataLength)
{
  uint32 i = 0;

  for (i = 0; i < DataLength; i++)
  {
    /* Device voltage range supposed to be [2.7V to 3.6V], the operation will
       be done by word */ 
    if (FLASH_ProgramWord(*FlashAddress, *(uint32*)(Data+i)) == FLASH_COMPLETE)
    {
     /* Check the written value */
      if (*(uint32*)*FlashAddress != *(uint32*)(Data+i))
      {
        /* Flash content doesn't match SRAM content */
        return(2);
      }
      /* Increment FLASH destination address */
      *FlashAddress += 4;
    }
    else
    {
      /* Error occurred while writing data in Flash memory */
      return (1);
    }
  }

  return (0);
  
}


/****************************************************************************
* ��    ��: ��ȡ��ַAddress��Ӧ��sector���
* ��ڲ�������ַ
* ���ڲ�����sector���
* ˵    ������
* ���÷�������
****************************************************************************/


uint16 Flash_GetSector(uint32 Address)
{
	uint16 sector = 0;
	if((Address < ADDR_FLASH_SECTOR_1) && (Address >= ADDR_FLASH_SECTOR_0))
	{
		sector = FLASH_Sector_0;  
	}
	else if((Address < ADDR_FLASH_SECTOR_2) && (Address >= ADDR_FLASH_SECTOR_1))
	{
		sector = FLASH_Sector_1;  
	}
	else if((Address < ADDR_FLASH_SECTOR_3) && (Address >= ADDR_FLASH_SECTOR_2))
	{
		sector = FLASH_Sector_2;  
	}
	else if((Address < ADDR_FLASH_SECTOR_4) && (Address >= ADDR_FLASH_SECTOR_3))
	{
		sector = FLASH_Sector_3;  
	}
	else if((Address < ADDR_FLASH_SECTOR_5) && (Address >= ADDR_FLASH_SECTOR_4))
	{
		sector = FLASH_Sector_4;  
	}
	else if((Address < ADDR_FLASH_SECTOR_6) && (Address >= ADDR_FLASH_SECTOR_5))
	{
		sector = FLASH_Sector_5;  
	}
	else if((Address < ADDR_FLASH_SECTOR_7) && (Address >= ADDR_FLASH_SECTOR_6))
	{
		sector = FLASH_Sector_6;  
	}	
	else if((Address < ADDR_FLASH_SECTOR_8) && (Address >= ADDR_FLASH_SECTOR_7))
	{
		sector = FLASH_Sector_7;  
	}
	else if((Address < ADDR_FLASH_SECTOR_9) && (Address >= ADDR_FLASH_SECTOR_8))
	{
		sector = FLASH_Sector_8;  
	}
	else if((Address < ADDR_FLASH_SECTOR_10) && (Address >= ADDR_FLASH_SECTOR_9))
	{
		sector = FLASH_Sector_9;  
	}
	else if((Address < ADDR_FLASH_SECTOR_11) && (Address >= ADDR_FLASH_SECTOR_10))
	{
		sector = FLASH_Sector_10;  
	}
	else/*(Address < FLASH_END_ADDR) && (Address >= ADDR_FLASH_SECTOR_11))*/
	{
		sector = FLASH_Sector_11;  
	}
	return sector;
}
 
/****************************************************************************
* ��    ��: ����ָ������
* ��ڲ�����SectorNum ������
* ���ڲ�������
* ˵    ������
* ���÷�������
****************************************************************************/
void Flash_EraseSector(uint16 SectorNum)
{
	FLASH_Unlock(); 
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
                  FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);
	if (FLASH_EraseSector(SectorNum, VoltageRange_3) != FLASH_COMPLETE) while (1);
	FLASH_Lock(); 
}

/****************************************************************************
* ��    ��: ������������
* ��ڲ�����address����ʼ��ַ
* ��ڲ�����sector_num��sector��Ŀ
* ���ڲ�������
* ˵    ������
* ���÷�������
****************************************************************************/
void Flash_EraseSectors(uint32 address, uint16 sector_num)
{
	uint16 StartSector, EndSector, i;
	
	FLASH_Unlock(); //����FLASH�������FLASH�в������ݡ�
	
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
                  FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);
	
	StartSector = Flash_GetSector(address);  //��ȡFLASH��Sector���
	EndSector = StartSector + sector_num*8;
	
	for (i = StartSector; i < EndSector; i += 8)  //ÿ��FLASH�������8���ɲο��ϱ�FLASH Sector�Ķ��塣
	{
		if (FLASH_EraseSector(i, VoltageRange_3) != FLASH_COMPLETE) while (1);
	}

	
	FLASH_Lock(); //����FLASH�������FLASH��д���ݡ�

}


/****************************************************************************
* ��    ��: д�볤��Ϊlength��32λ����
* ��ڲ�����address����ַ
			length�� ���ݳ���
			data_32��Ҫд�������ָ��
* ���ڲ�������
* ˵    ������
* ���÷�������
****************************************************************************/
void Flash_Write32BitDatas(uint32 address, uint16 length, uint32* data_32)
{
	uint16 i;
	FLASH_Status status;

	for(i=0; i<length; i++)
	{
		do{			
			status = FLASH_ProgramWord(address, data_32[i]);  // == FLASH_COMPLETE)	//��DATA_32д����Ӧ�ĵ�ַ��
		}while(status != FLASH_COMPLETE);
		
//		if (FLASH_ProgramWord(address, data_32[i]) == FLASH_COMPLETE)   //��DATA_32д����Ӧ�ĵ�ַ��
//		{
		    address = address + 4;
//		}
//		else
//		{ 
//			while (1);
//		}
	}
	
}
 
/****************************************************************************
* ��    ��: д�볤��Ϊlength��32λ����
* ��ڲ�����address����ַ
			length�� ���ݳ���
			data_32��Ҫд�������ָ��
* ���ڲ�������
* ˵    ������
* ���÷�������
****************************************************************************/
void Flash_Write32BitDatasWithErase(uint32 address, uint16 length, uint32* data_32)
{
	uint16 StartSector, EndSector,i;
	
	FLASH_Unlock(); //����FLASH�������FLASH��д���ݡ�
	
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
                  FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);
	
	StartSector = Flash_GetSector(address);  //��ȡFLASH��Sector���
	EndSector = Flash_GetSector(address+4*length);
	
	for (i = StartSector; i <= EndSector; i += 8)  //ÿ��FLASH�������8���ɲο��ϱ�FLASH Sector�Ķ��塣
	{
		if (FLASH_EraseSector(i, VoltageRange_3) != FLASH_COMPLETE) while (1);
	}
	
	for(i=0; i<length; i++)
	{
		if (FLASH_ProgramWord(address, data_32[i]) == FLASH_COMPLETE)   //��DATA_32д����Ӧ�ĵ�ַ��
		{
		    address = address + 4;
		}
		else
		{ 
			while (1);
		}
	}
	
	FLASH_Lock();  //��FLASH����ҪFLASH���ڽ���״̬��
}
 
/****************************************************************************
* ��    ��: ��ȡ����Ϊlength��32λ����
* ��ڲ�����address����ַ
			length�� ���ݳ���
			data_32  ָ�����������
* ���ڲ�������
* ˵    ������
* ���÷�������
****************************************************************************/
void Flash_Read32BitDatas(uint32 address, uint16 length, uint32* data_32)
{
	uint8_t i;
	for(i=0; i<length; i++)
	{
		data_32[i]=*(__IO int32_t*)address;
		address=address + 4;
	}
}
 
/****************************************************************************
* ��    ��: д�볤��Ϊlength��16λ����
* ��ڲ�����address����ַ
			length�� ���ݳ���
			data_16��Ҫд�������ָ��
* ���ڲ�������
* ˵    ������
* ���÷�������
****************************************************************************/
void Flash_Write16BitDatasWithErase(uint32 address, uint16 length, uint16* data_16)
{
	uint16 StartSector, EndSector,i;
	
	FLASH_Unlock(); //����FLASH�������FLASH��д���ݡ�
	
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
                  FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);
	
	StartSector = Flash_GetSector(address);  //��ȡFLASH��Sector���
	EndSector = Flash_GetSector(address+2*length);
	
	for (i = StartSector; i <= EndSector; i += 8)  //ÿ��FLASH�������8���ɲο��ϱ�FLASH Sector�Ķ��塣
	{
		if (FLASH_EraseSector(i, VoltageRange_3) != FLASH_COMPLETE) while (1);
	}
	for(i=0; i<length; i++)
	{
		if (FLASH_ProgramHalfWord(address, data_16[i]) == FLASH_COMPLETE)   
		{
		    address = address + 2;
		}
		else
		{ 
			while (1);
		}
	}
	
	FLASH_Lock();  //��FLASH����ҪFLASH���ڽ���״̬��
}
 
/****************************************************************************
* ��    ��: ��ȡ����Ϊlength��16λ����
* ��ڲ�����address����ַ
			length�� ���ݳ���
			data_16  ָ�����������
* ���ڲ�������
* ˵    ������
* ���÷�������
****************************************************************************/
void Flash_Read16BitDatas(uint32 address, uint16 length, uint16* data_16)
{
	uint8_t i;
	for(i=0; i<length; i++)
	{
		data_16[i]=*(__IO int16_t*)address;
		address=address + 2;
	}
}

/****************************************************************************
* ��    ��: д�볤��Ϊlength��8λ����
* ��ڲ�����address����ַ
			length�� ���ݳ���
			data_8��Ҫд�������ָ��
* ���ڲ�������
* ˵    ������
* ���÷�������
****************************************************************************/
void Flash_Write8BitDatas(uint32 address, uint16 length, uint8* data_8)
{
	uint16 i;
	FLASH_Status status;
	
	for(i=0; i<length; i++)
	{

#if 0	
		do{ 		
			status = FLASH_ProgramByte(address, data_8[i]);  // == FLASH_COMPLETE) //��DATA_32д����Ӧ�ĵ�ַ��
		}while(status != FLASH_COMPLETE);
		
		address++;

#else		
		if (FLASH_ProgramByte(address, data_8[i]) == FLASH_COMPLETE)   
		{
		    address++;
		}
		else
		{ 
//			while (1);
		}
#endif
	}
	
}
 

 
/****************************************************************************
* ��    ��: д�볤��Ϊlength��8λ����
* ��ڲ�����address����ַ
			length�� ���ݳ���
			data_8��Ҫд�������ָ��
* ���ڲ�������
* ˵    ������
* ���÷�������
****************************************************************************/
void Flash_Write8BitDatasWithErase(uint32 address, uint16 length, uint8* data_8)
{
	uint16 StartSector, EndSector,i;
	FLASH_Unlock(); //����FLASH�������FLASH��д���ݡ�
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
                  FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);
	
	StartSector = Flash_GetSector(address);  //��ȡFLASH��Sector���
	EndSector = Flash_GetSector(address+length);
	
	for (i = StartSector; i <= EndSector; i += 8)  //ÿ��FLASH�������8���ɲο��ϱ�FLASH Sector�Ķ��塣
	{
		if (FLASH_EraseSector(i, VoltageRange_3) != FLASH_COMPLETE) while (1);
	}
	
	for(i=0; i<length; i++)
	{
		if (FLASH_ProgramByte(address, data_8[i]) == FLASH_COMPLETE)   
		{
		    address++;
		}
		else
		{ 
			while (1);
		}
	}
	
	FLASH_Lock();  //��FLASH����ҪFLASH���ڽ���״̬��
}
 
/****************************************************************************
* ��    ��: ��ȡ����Ϊlength��8λ����
* ��ڲ�����address����ַ
			length�� ���ݳ���
			data_8  ָ�����������
* ���ڲ�������
* ˵    ������
* ���÷�������
****************************************************************************/
void Flash_Read8BitDatas(uint32 address, uint16 length, uint8* data_8)
{
	uint8_t i;
	for(i=0; i<length; i++)
	{
		data_8[i]=*(__IO int8_t*)address;
		address++;
	}

}

