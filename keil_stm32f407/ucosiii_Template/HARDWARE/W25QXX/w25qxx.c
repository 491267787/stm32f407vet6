#include "w25qxx.h" 
#include "spi.h"
#include "delay.h"	   
#include "usart.h"	  
//////////////////////////////////////////////////////////////////////////////////	 
//���ܣ�W25QXX ��������
//���ߣ�
//��������:2020/05/12
//�汾��V1.0			
//������W25Q64���ԣ�����Ϊ8M�ֽ�,����64��Block,2048��Sector��16������Ϊ1��Block��
//      4KbytesΪһ��Sector
////////////////////////////////////////////////////////////////////////////////// 	
 
u16 W25QXX_TYPE = W25Q128;	//Ĭ����W25Q128



/*
** ��������: 
** ��������: ��ʼ��SPI FLASH��IO��
** �䡡��: 
** �䡡��: 
** ȫ�ֱ���:
** ����:
*/
void W25QXX_Init(void)
{ 
  GPIO_InitTypeDef  GPIO_InitStructure;
 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

	  //GPIOE3
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PE3
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//���
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_Init(GPIOE, &GPIO_InitStructure);//��ʼ��
 
//	GPIO_SetBits(GPIOG,GPIO_Pin_7);//PG7���1,��ֹNRF����SPI FLASH��ͨ�� 
	W25QXX_CS=1;			//SPI FLASH��ѡ��
	SPI2_Init();		   			//��ʼ��SPI
	SPI2_SetSpeed(SPI_BaudRatePrescaler_4);		//����Ϊ21Mʱ��,����ģʽ 
	W25QXX_TYPE=W25QXX_ReadID();	//��ȡFLASH ID.
}  


/*
** ��������: 
** ��������: ��ȡW25QXX��״̬�Ĵ���
** �䡡��: 
** �䡡��: 
** ȫ�ֱ���:
** ����:
				BIT7  6   5   4   3   2   1   0
				SPR   RV  TB BP2 BP1 BP0 WEL BUSY
				SPR:Ĭ��0,״̬�Ĵ�������λ,���WPʹ��
				TB,BP2,BP1,BP0:FLASH����д��������
				WEL:дʹ������
				BUSY:æ���λ(1,æ;0,����)
				Ĭ��:0x00
*/
u8 W25QXX_ReadSR(void)   
{  
	u8 byte=0;   
	W25QXX_CS=0;                            //ʹ������   
	SPI2_ReadWriteByte(W25X_ReadStatusReg);    //���Ͷ�ȡ״̬�Ĵ�������    
	byte=SPI2_ReadWriteByte(0Xff);             //��ȡһ���ֽ�  
	W25QXX_CS=1;                            //ȡ��Ƭѡ     
	return byte;   
} 


/*
** ��������: 
** ��������: дW25QXX״̬�Ĵ���
** �䡡��: 
** �䡡��: 
** ȫ�ֱ���:
** ����: ֻ��SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)����д!!!
*/
void W25QXX_Write_SR(u8 sr)   
{   
	W25QXX_CS=0;                            //ʹ������   
	SPI2_ReadWriteByte(W25X_WriteStatusReg);   //����дȡ״̬�Ĵ�������    
	SPI2_ReadWriteByte(sr);               //д��һ���ֽ�  
	W25QXX_CS=1;                            //ȡ��Ƭѡ     	      
}   


/*
** ��������: 
** ��������: W25QXXдʹ��	
** �䡡��: 
** �䡡��: 
** ȫ�ֱ���:
** ����: ��WEL��λ 
*/
void W25QXX_Write_Enable(void)   
{
	W25QXX_CS=0;                            //ʹ������   
  SPI2_ReadWriteByte(W25X_WriteEnable);      //����дʹ��  
	W25QXX_CS=1;                            //ȡ��Ƭѡ     	      
} 


/*
** ��������: 
** ��������: W25QXXд��ֹ	
** �䡡��: 
** �䡡��: 
** ȫ�ֱ���:
** ����: ��WEL����
*/
void W25QXX_Write_Disable(void)   
{  
	W25QXX_CS=0;                            //ʹ������   
  SPI2_ReadWriteByte(W25X_WriteDisable);     //����д��ָֹ��    
	W25QXX_CS=1;                            //ȡ��Ƭѡ     	      
} 		


/*
** ��������: 
** ��������: ��ȡоƬID
** �䡡��: 
** �䡡��: 
					0XEF13,��ʾоƬ�ͺ�ΪW25Q80  
					0XEF14,��ʾоƬ�ͺ�ΪW25Q16    
					0XEF15,��ʾоƬ�ͺ�ΪW25Q32  
					0XEF16,��ʾоƬ�ͺ�ΪW25Q64 
					0XEF17,��ʾоƬ�ͺ�ΪW25Q128 	 
** ȫ�ֱ���:
** ����: ��WEL����
*/
u16 W25QXX_ReadID(void)
{
	u16 Temp = 0;	  
	W25QXX_CS=0;				    
	SPI2_ReadWriteByte(0x90);//���Ͷ�ȡID����	    
	SPI2_ReadWriteByte(0x00); 	    
	SPI2_ReadWriteByte(0x00); 	    
	SPI2_ReadWriteByte(0x00); 	 			   
	Temp|=SPI2_ReadWriteByte(0xFF)<<8;  
	Temp|=SPI2_ReadWriteByte(0xFF);	 
	W25QXX_CS=1;				    
	return Temp;
}   	



/*
** ��������: 
** ��������: ��ȡSPI FLASH  
** �䡡��: 
						pBuffer:���ݴ洢��
						ReadAddr:��ʼ��ȡ�ĵ�ַ(24bit)
						NumByteToRead:Ҫ��ȡ���ֽ���(���65535)
** �䡡��: 
** ȫ�ֱ���:
** ����: ��ָ����ַ��ʼ��ȡָ�����ȵ�����,
*/
void W25QXX_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead)   
{ 
 	u16 i;   										    
	W25QXX_CS=0;                            //ʹ������   
  SPI2_ReadWriteByte(W25X_ReadData);         //���Ͷ�ȡ����   
  SPI2_ReadWriteByte((u8)((ReadAddr)>>16));  //����24bit��ַ    
  SPI2_ReadWriteByte((u8)((ReadAddr)>>8));   
  SPI2_ReadWriteByte((u8)ReadAddr);   
  for(i=0;i<NumByteToRead;i++)
	{ 
    pBuffer[i]=SPI2_ReadWriteByte(0XFF);   //ѭ������  
  }
	W25QXX_CS=1;  				    	      
}  


/*
** ��������: 
** ��������: SPI��һҳ(0~65535)��д������256���ֽڵ�����
** �䡡��: 
						pBuffer:���ݴ洢��
						WriteAddr:��ʼд��ĵ�ַ(24bit)
						NumByteToWrite:Ҫд����ֽ���(���256),������Ӧ�ó�����ҳ��ʣ���ֽ���!!!	
** �䡡��: 
** ȫ�ֱ���:
** ����: ��ָ����ַ��ʼд�����256�ֽڵ�����
*/
void W25QXX_Write_Page(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)
{
 	u16 i;  
  W25QXX_Write_Enable();                  //SET WEL 
	W25QXX_CS=0;                            //ʹ������   
  SPI2_ReadWriteByte(W25X_PageProgram);      //����дҳ����   
  SPI2_ReadWriteByte((u8)((WriteAddr)>>16)); //����24bit��ַ    
  SPI2_ReadWriteByte((u8)((WriteAddr)>>8));   
  SPI2_ReadWriteByte((u8)WriteAddr);   
  for(i=0;i<NumByteToWrite;i++)
	{
		SPI2_ReadWriteByte(pBuffer[i]);//ѭ��д��  
	}
	W25QXX_CS=1;                            //ȡ��Ƭѡ 
	W25QXX_Wait_Busy();					   //�ȴ�д�����
} 


/*
** ��������: 
** ��������: �޼���дSPI FLASH,����ȷ����д�ĵ�ַ��Χ�ڵ�����ȫ��Ϊ0XFF,�����ڷ�0XFF��
             д������ݽ�ʧ��!�����Զ���ҳ���� 
** �䡡��: 
						pBuffer:���ݴ洢��
						WriteAddr:��ʼд��ĵ�ַ(24bit)
						NumByteToWrite:Ҫд����ֽ���(���65535)	
** �䡡��: 
** ȫ�ֱ���:
** ����: ��ָ����ַ��ʼд��ָ�����ȵ�����,����Ҫȷ����ַ��Խ��!
*/
void W25QXX_Write_NoCheck(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{ 			 		 
	u16 pageremain;	   
	pageremain = 256-WriteAddr%256; //��ҳʣ����ֽ���		 	    
	if(NumByteToWrite <= pageremain)
	{
		pageremain = NumByteToWrite;//������256���ֽ�
	}
	
	while(1)
	{	   
		W25QXX_Write_Page(pBuffer,WriteAddr,pageremain);
		if(NumByteToWrite == pageremain)
		{
			break;//д�������
		}
	 	else //NumByteToWrite>pageremain
		{
			pBuffer += pageremain;
			WriteAddr += pageremain;	

			NumByteToWrite -= pageremain;			  //��ȥ�Ѿ�д���˵��ֽ���
			if(NumByteToWrite>256)
			{
				pageremain=256; //һ�ο���д��256���ֽ�
			}
			else
			{				
				pageremain=NumByteToWrite; 	  //����256���ֽ���
			}
		}
	};	    
} 



/*
** ��������: 
** ��������: дSPI FLASH  
** �䡡��: 
						pBuffer:���ݴ洢��
						WriteAddr:��ʼд��ĵ�ַ(24bit)
						NumByteToWrite:Ҫд����ֽ���(���65535)	
** �䡡��: 
** ȫ�ֱ���:
** ����: ��ָ����ַ��ʼд��ָ�����ȵ�����,�ú�������������!
*/
u8 W25QXX_BUFFER[4096];		 
void W25QXX_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{ 
	u32 secpos;
	u16 secoff;
	u16 secremain;	   
 	u16 i;    
	u8 * W25QXX_BUF;	  
  W25QXX_BUF = W25QXX_BUFFER;	     
 	secpos = WriteAddr/4096;//������ַ  
	secoff = WriteAddr%4096;//�������ڵ�ƫ��
	secremain = 4096-secoff;//����ʣ��ռ��С   
 	//printf("ad:%X,nb:%X\r\n",WriteAddr,NumByteToWrite);//������
 	if(NumByteToWrite <= secremain)
	{
		secremain=NumByteToWrite;//������4096���ֽ�
	}
	
	while(1) 
	{	
		W25QXX_Read(W25QXX_BUF,secpos*4096,4096);//������������������
		for(i=0;i<secremain;i++)//У������
		{
			if(W25QXX_BUF[secoff+i]!=0XFF)break;//��Ҫ����  	  
		}
		if(i<secremain)//��Ҫ����
		{
			W25QXX_Erase_Sector(secpos);//�����������
			for(i=0;i<secremain;i++)	   //����
			{
				W25QXX_BUF[i+secoff]=pBuffer[i];	  
			}
			W25QXX_Write_NoCheck(W25QXX_BUF,secpos*4096,4096);//д����������  

		}
		else 
		{
			W25QXX_Write_NoCheck(pBuffer,WriteAddr,secremain);//д�Ѿ������˵�,ֱ��д������ʣ������.
		}
		
		if(NumByteToWrite==secremain)
		{
			break;//д�������
		}
		else//д��δ����
		{
			secpos++;//������ַ��1
			secoff=0;//ƫ��λ��Ϊ0 	 

		  pBuffer+=secremain;  //ָ��ƫ��
			WriteAddr+=secremain;//д��ַƫ��	   
		  NumByteToWrite-=secremain;				//�ֽ����ݼ�
			if(NumByteToWrite>4096)
			{
				secremain=4096;	//��һ����������д����
			}
			else
			{	
				secremain=NumByteToWrite;			//��һ����������д����
			}
		}	 
	};	 
}



/*
** ��������: 
** ��������: ��������оƬ,�ȴ�ʱ�䳬��...
** �䡡��: 
** �䡡��: 
** ȫ�ֱ���:
** ����: 
*/
void W25QXX_Erase_Chip(void)   
{                                   
  W25QXX_Write_Enable();                  //SET WEL 
  W25QXX_Wait_Busy();   
  W25QXX_CS=0;                            //ʹ������   
  SPI2_ReadWriteByte(W25X_ChipErase);        //����Ƭ��������  
	W25QXX_CS=1;                            //ȡ��Ƭѡ     	      
	W25QXX_Wait_Busy();   				   //�ȴ�оƬ��������
}   



/*
** ��������: 
** ��������: ����һ������,����һ������������ʱ��:150ms
** �䡡��: 
						Dst_Addr:������ַ ����ʵ����������
** �䡡��: 
** ȫ�ֱ���:
** ����: 
*/
void W25QXX_Erase_Sector(u32 Dst_Addr)   
{  
	//����falsh�������,������   
// 	printf("fe:%x\r\n",Dst_Addr);	  
 	Dst_Addr*=4096;
  W25QXX_Write_Enable();                  //SET WEL 	 
  W25QXX_Wait_Busy();   
  W25QXX_CS=0;                            //ʹ������   
  SPI2_ReadWriteByte(W25X_SectorErase);      //������������ָ�� 
  SPI2_ReadWriteByte((u8)((Dst_Addr)>>16));  //����24bit��ַ    
  SPI2_ReadWriteByte((u8)((Dst_Addr)>>8));   
  SPI2_ReadWriteByte((u8)Dst_Addr);  
	W25QXX_CS=1;                            //ȡ��Ƭѡ     	      
  W25QXX_Wait_Busy();   				   //�ȴ��������
}  
//�ȴ�����


/*
** ��������: 
** ��������: �ȴ�����
** �䡡��: 
** �䡡��: 
** ȫ�ֱ���:
** ����: 
*/
void W25QXX_Wait_Busy(void)   
{   
	while((W25QXX_ReadSR()&0x01)==0x01);   // �ȴ�BUSYλ���
}  



/*
** ��������: 
** ��������: �������ģʽ
** �䡡��: 
** �䡡��: 
** ȫ�ֱ���:
** ����: 
*/
void W25QXX_PowerDown(void)   
{ 
  W25QXX_CS=0;                            //ʹ������   
  SPI2_ReadWriteByte(W25X_PowerDown);        //���͵�������  
	W25QXX_CS=1;                            //ȡ��Ƭѡ     	      
  delay_us(3);                               //�ȴ�TPD  
}   



/*
** ��������: 
** ��������: ����
** �䡡��: 
** �䡡��: 
** ȫ�ֱ���:
** ����: 
*/
void W25QXX_WAKEUP(void)   
{  
  W25QXX_CS=0;                            //ʹ������   
  SPI2_ReadWriteByte(W25X_ReleasePowerDown);   //  send W25X_PowerDown command 0xAB    
	W25QXX_CS=1;                            //ȡ��Ƭѡ     	      
  delay_us(3);                               //�ȴ�TRES1
}   


























