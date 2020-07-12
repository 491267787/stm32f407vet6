#include "exfuns/exfuns.h"
#include "malloc.h"
//#include "usart.h"
#include "Debug.h"
#include "string.h"

	   
//////////////////////////////////////////////////////////////////////////////////	 
//���ܣ�FATFS ��չ���룬�ļ�����api  
//���ߣ�zyf
//��������:2020/05/29
//�汾��V1.0	
//������
//��ʷ��
////////////////////////////////////////////////////////////////////////////////// 							  


#define FILE_MAX_TYPE_NUM		7	//���FILE_MAX_TYPE_NUM������
#define FILE_MAX_SUBT_NUM		4	//���FILE_MAX_SUBT_NUM��С��

 //�ļ������б�
u8*const FILE_TYPE_TBL[FILE_MAX_TYPE_NUM][FILE_MAX_SUBT_NUM]=
{
{"BIN"},			//BIN�ļ�
{"LRC"},			//LRC�ļ�
{"NES"},			//NES�ļ�
{"TXT","C","H"},	//�ı��ļ�
{"WAV","MP3","APE","FLAC"},//֧�ֵ������ļ�
{"BMP","JPG","JPEG","GIF"},//ͼƬ�ļ�
{"AVI"},			//��Ƶ�ļ�
};
///////////////////////////////�����ļ���,ʹ��malloc��ʱ��////////////////////////////////////////////
FATFS *fs[_VOLUMES];//�߼����̹�����.	 
//FIL *file;	  		//�ļ�1
//FIL *ftemp;	  		//�ļ�2.
//UINT br,bw;			//��д����
//FILINFO fileinfo;	//�ļ���Ϣ
//DIR dir;  			//Ŀ¼

//u8 *fatbuf;			//SD�����ݻ�����
///////////////////////////////////////////////////////////////////////////////////////



FATFS *exfuns_GetfsArea(u8 fsnum)
{
	return fs[fsnum];
}

////Ϊexfuns�����ڴ�
////����ֵ:0,�ɹ�
////1,ʧ��
//u8 exfuns_init(void)
//{
//	u8 i;
//	for(i=0;i<_VOLUMES;i++)
//	{
//		fs[i]=(FATFS*)mymalloc(SRAMIN,sizeof(FATFS));	//Ϊ����i�����������ڴ�	
//		if(!fs[i])break;
//	}
////	file=(FIL*)mymalloc(SRAMIN,sizeof(FIL));		//Ϊfile�����ڴ�
////	ftemp=(FIL*)mymalloc(SRAMIN,sizeof(FIL));		//Ϊftemp�����ڴ�
////	fatbuf=(u8*)mymalloc(SRAMIN,512);				//Ϊfatbuf�����ڴ�
//	if(i==_VOLUMES)//&&file&&ftemp&&fatbuf)
//		return 0;  //������һ��ʧ��,��ʧ��.
//	else 
//		return 1;	
//}

//Ϊexfuns�����ڴ�
//����ֵ:0,�ɹ�
//1,ʧ��
u8 exfuns_init(u8 disknum)
{
	fs[disknum]=(FATFS*)mymalloc(SRAMIN,sizeof(FATFS));	//Ϊ����i�����������ڴ�	
	if(NULL == fs[disknum])
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


//��Сд��ĸתΪ��д��ĸ,���������,�򱣳ֲ���.
u8 char_upper(u8 c)
{
	if(c<'A')return c;//����,���ֲ���.
	if(c>='a')return c-0x20;//��Ϊ��д.
	else return c;//��д,���ֲ���
}	      
//�����ļ�������
//fname:�ļ���
//����ֵ:0XFF,��ʾ�޷�ʶ����ļ����ͱ��.
//		 ����,����λ��ʾ��������,����λ��ʾ����С��.
u8 f_typetell(u8 *fname)
{
	u8 tbuf[5];
	u8 *attr='\0';//��׺��
	u8 i=0,j;
	while(i<250)
	{
		i++;
		if(*fname=='\0')break;//ƫ�Ƶ��������.
		fname++;
	}
	if(i==250)return 0XFF;//������ַ���.
 	for(i=0;i<5;i++)//�õ���׺��
	{
		fname--;
		if(*fname=='.')
		{
			fname++;
			attr=fname;
			break;
		}
  	}
	strcpy((char *)tbuf,(const char*)attr);//copy
 	for(i=0;i<4;i++)tbuf[i]=char_upper(tbuf[i]);//ȫ����Ϊ��д 
	for(i=0;i<FILE_MAX_TYPE_NUM;i++)	//����Ա�
	{
		for(j=0;j<FILE_MAX_SUBT_NUM;j++)//����Ա�
		{
			if(*FILE_TYPE_TBL[i][j]==0)break;//�����Ѿ�û�пɶԱȵĳ�Ա��.
			if(strcmp((const char *)FILE_TYPE_TBL[i][j],(const char *)tbuf)==0)//�ҵ���
			{
				return (i<<4)|j;
			}
		}
	}
	return 0XFF;//û�ҵ�		 			   
}	 

//�õ�����ʣ������
//drv:���̱��("0:"/"1:")
//total:������	 ����λKB��
//free:ʣ������	 ����λKB��
//����ֵ:0,����.����,�������
u8 exf_getfree(u8 *drv,u32 *total,u32 *free)
{
	FATFS *fs1;
	u8 res;
  u32 fre_clust=0, fre_sect=0, tot_sect=0;
  //�õ�������Ϣ�����д�����
  res =(u32)f_getfree((const TCHAR*)drv, (DWORD*)&fre_clust, &fs1);
  if(res==0)
	{											   
	    tot_sect=(fs1->n_fatent-2)*fs1->csize;	//�õ���������
	    fre_sect=fre_clust*fs1->csize;			//�õ�����������	   
#if _MAX_SS!=512				  				//������С����512�ֽ�,��ת��Ϊ512�ֽ�
		tot_sect*=fs1->ssize/512;
		fre_sect*=fs1->ssize/512;
#endif	  
		*total=tot_sect>>1;	//��λΪKB
		*free=fre_sect>>1;	//��λΪKB 
 	}
	return res;
}	


/*
*********************************************************************************************************
*	�� �� ��: exfuns_ViewRootDir
*	����˵��: ��ʾ��Ŀ¼�µ��ļ���
*	��    �Σ�path:·��
*	�� �� ֵ: 
*********************************************************************************************************
*/
u8 exfuns_ViewRootDir(char *path)
{
	FRESULT result;
	DIR *DirInf = mymalloc(SRAMIN,sizeof(DirInf));		//ΪDirInf�����ڴ�
	FILINFO *FileInf = mymalloc(SRAMIN,sizeof(FileInf));		//ΪFileInf�����ڴ�
	
	if((NULL == DirInf) || (NULL == FileInf) )
	{
		DEBUG_Printf("malloc ViewRootDir failed!");
		goto ViewRootDir_END;
	}
	
	
	uint32_t cnt = 0;
	char lfname[100];

	/* �򿪸��ļ��� */
	result = f_opendir(DirInf, path);//"/"); /* ���������������ӵ�ǰĿ¼��ʼ */
	if (result != FR_OK)
	{
		printf("�򿪸�Ŀ¼ʧ�� (%d)\r\n", result);
		goto ViewRootDir_END;
	}

	/* ��ȡ��ǰ�ļ����µ��ļ���Ŀ¼ */
	FileInf->lfname = lfname;
	FileInf->lfsize = 256;

	printf("����        |  �ļ���С | ���ļ��� | ���ļ���\r\n");
	for (cnt = 0; ;cnt++)
	{
		result = f_readdir(DirInf,FileInf); 		/* ��ȡĿ¼��������Զ����� */
		if (result != FR_OK || FileInf->fname[0] == 0)
		{
			break;
		}

		if (FileInf->fname[0] == '.')
		{
			continue;
		}

		/* �ж����ļ�������Ŀ¼ */
		if (FileInf->fattrib & AM_DIR)
		{
			printf("(0x%02d)Ŀ¼  ", FileInf->fattrib);
		}
		else
		{
			printf("(0x%02d)�ļ�  ", FileInf->fattrib);
		}

		/* ��ӡ�ļ���С, ���4G */
		printf(" %10ld", FileInf->fsize);   //��λ���ֽ�

		printf("  %s |", FileInf->fname);	/* ���ļ��� */

		printf("  %s\r\n", (char *)FileInf->lfname);	/* ���ļ��� */
	}
	
ViewRootDir_END:
	myfree(SRAMIN,DirInf);//�ͷ��ڴ�
	myfree(SRAMIN,FileInf);//�ͷ��ڴ�
	
	return result;
}


//�������ļ�д����
u8 SST_Rd_BufferWrite(u8 *pBuffer, u32 WriteAddr, u16 NumByteToWrite, const TCHAR* path)
{
	OS_ERR err;
	u32 bytesToWrite = 0;
	FIL* file=(FIL*)mymalloc(SRAMIN,sizeof(FIL));		//Ϊfile�����ڴ�
	if(NULL == file)
	{
		myfree(SRAMIN,file);//�ͷ��ڴ�
		DEBUG_Printf("malloc writefile_ptr failed!");
		return 0;
	}
	
	
	OSSchedLock(&err);
	
	f_open(file, path, FA_WRITE);	
	f_lseek(file, WriteAddr);	
	f_write(file, pBuffer, NumByteToWrite, &bytesToWrite);
	f_sync(file);
	f_close(file);
	
	OSSchedUnlock(&err);
	
	myfree(SRAMIN,file);//�ͷ��ڴ�
	
	return 1;
}


//�������ļ�������
u8 SST_Rd_BufferRead(u8 *recv, u32 adr, u16 size, const TCHAR* path)
{
	OS_ERR err;
	u32 numOfReadBytes = 0;
	FIL* file=(FIL*)mymalloc(SRAMIN,sizeof(FIL));		//Ϊfile�����ڴ�
	if(NULL == file)
	{
		myfree(SRAMIN,file);//�ͷ��ڴ�
		DEBUG_Printf("malloc readfile_ptr failed!");
		return 0;
	}
	

	OSSchedLock(&err);
	
	f_open(file, path, FA_READ);	
	f_lseek(file, adr);
	f_read(file, recv, size, &numOfReadBytes);
	f_close(file);
	
	OSSchedUnlock(&err);
	
	myfree(SRAMIN,file);//�ͷ��ڴ�
	
	return 1;
}






