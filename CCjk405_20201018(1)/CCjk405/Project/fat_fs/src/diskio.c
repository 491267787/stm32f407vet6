/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2013        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control module to the FatFs module with a defined API.        */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		  /* FatFs lower layer API */	
#include "SST_FLASH.h"

#define EX_FLASH            1	                 //�ⲿflash,���Ϊ1

#define FLASH_SECTOR_SIZE 	512       		  

#define FLASH_BLOCK_SIZE   	(SST_BLOCK_SIZE / FLASH_SECTOR_SIZE)     
#define	FLASH_SECTOR_COUNT  (SST_CAPACITY_SIZE / FLASH_SECTOR_SIZE)    //4M�洢������������


//��ʼ������
DSTATUS disk_initialize(BYTE pdrv)	  /* Physical drive nmuber (0..) */
{
	switch (pdrv)
	{
		case EX_FLASH:                           //�ⲿflash
 			break;
	}		 
	
	return RES_OK; 
}  

//��ô���״̬
/* Physical drive nmuber (0..) */
DSTATUS disk_status(BYTE pdrv)     
{ 
	return 0;
} 

//������
//drv:���̱��0~9
//*buff:���ݽ��ջ����׵�ַ
//sector:������ַ
//count:��Ҫ��ȡ��������
/* Physical drive nmuber (0..) */
/* Data buffer to store read data */
/* Sector address (LBA) */
/* Number of sectors to read (1..128) */
DRESULT disk_read(BYTE pdrv, BYTE *buff, DWORD sector, UINT count)
{
	switch (pdrv)
	{
		case EX_FLASH:                           //�ⲿflash
			
			while (count > 0)
			{
				Flash_BufferRead(buff, sector * FLASH_SECTOR_SIZE, FLASH_SECTOR_SIZE);
				buff += FLASH_SECTOR_SIZE;
				sector++;
				count--;
			}
			
			break; 
	}
	
  return RES_OK;	   
}

//д����
//drv:���̱��0~9
//*buff:���������׵�ַ
//sector:������ַ
//count:��Ҫд���������
/* Physical drive nmuber (0..) */
/* Data to be written */
/* Sector address (LBA) */
/* Number of sectors to write (1..128) */
DRESULT disk_write(BYTE pdrv, const BYTE *buff, DWORD sector, UINT count)
{ 
	switch (pdrv)
	{
		case EX_FLASH:                           //�ⲿflash
			
			while (count > 0)
			{										    
				Flash_BufferWrite((u8*)buff, sector * FLASH_SECTOR_SIZE, FLASH_SECTOR_SIZE);
				buff += FLASH_SECTOR_SIZE;
				sector++;
				count--;
			}

			break; 
	}
  
	return RES_OK;
}

//����������Ļ��
//drv:���̱��0~9
//ctrl:���ƴ���
//*buff:����/���ջ�����ָ��
/* Physical drive nmuber (0..) */
/* Control code */
/* Buffer to send/receive control data */
DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void *buff)
{
	if (pdrv == EX_FLASH)	//�ⲿFLASH  
	{
		switch (cmd)
		{
			case CTRL_SYNC:
					break;	 
			
			case GET_SECTOR_SIZE:
					*(WORD*)buff = FLASH_SECTOR_SIZE;
					break;	 
			
			case GET_BLOCK_SIZE:
					*(WORD*)buff = FLASH_BLOCK_SIZE;
					break;	 
			
			case GET_SECTOR_COUNT:
					*(DWORD*)buff = FLASH_SECTOR_COUNT;
					break;
		}
	}
  
	return RES_OK;
}


















