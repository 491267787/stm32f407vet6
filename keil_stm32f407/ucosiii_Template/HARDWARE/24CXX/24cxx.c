#include "24cxx.h" 
#include "delay.h" 
#include "os.h"
#include "stdio.h"
//////////////////////////////////////////////////////////////////////////////////	 
//���ܣ�24CXX ��������
//���ߣ�
//��������:2020/05/09
//�汾��V1.0									  
////////////////////////////////////////////////////////////////////////////////// 	

OS_SEM* AT24CXX_Sem;
static char *AT24CXX_SemName = "AT24CXX_Sem0";

//��ʼ��IIC�ӿ�
void AT24CXX_Init(void)
{
	OS_ERR err;
	IIC_Init();//IIC��ʼ��
	OSSemCreate(AT24CXX_Sem, AT24CXX_SemName, 1, &err);	      // �����ź���  ��ʼ��Ϊ�Ѵ�
}
//��AT24CXXָ����ַ����һ������
//ReadAddr:��ʼ�����ĵ�ַ  
//����ֵ  :����������
u8 AT24CXX_ReadOneByte(u16 ReadAddr)
{				  
	u8 temp=0;		  	    																 
  IIC_Start();  
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0XA0);	   //����д����
		IIC_Wait_Ack();
		IIC_Send_Byte(ReadAddr>>8);//���͸ߵ�ַ	    
	}
	else 
	{
		IIC_Send_Byte(0XA0+((ReadAddr/256)<<1));   //����������ַ0XA0,д���� 	   
	}
  IIC_Wait_Ack(); 
  IIC_Send_Byte(ReadAddr%256);   //���͵͵�ַ
	IIC_Wait_Ack();	    
	IIC_Start();  	 	   
	IIC_Send_Byte(0XA1);           //�������ģʽ			   
	IIC_Wait_Ack();	 
  temp=IIC_Read_Byte(0);		   
  IIC_Stop();//����һ��ֹͣ����	    
	return temp;
}
//��AT24CXXָ����ַд��һ������
//WriteAddr  :д�����ݵ�Ŀ�ĵ�ַ    
//DataToWrite:Ҫд�������
void AT24CXX_WriteOneByte(u16 WriteAddr,u8 DataToWrite)
{				   	  	    																 
  IIC_Start();  
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0XA0);	    //����д����
		IIC_Wait_Ack();
		IIC_Send_Byte(WriteAddr>>8);//���͸ߵ�ַ	  
	}
	else
	{
		IIC_Send_Byte(0XA0+((WriteAddr/256)<<1));   //����������ַ0XA0,д���� 	 
	}
  IIC_Wait_Ack();	   
  IIC_Send_Byte(WriteAddr%256);   //���͵͵�ַ
	IIC_Wait_Ack(); 	 										  		   
	IIC_Send_Byte(DataToWrite);     //�����ֽ�							   
	IIC_Wait_Ack();  		    	   
  IIC_Stop();//����һ��ֹͣ���� 
	delay_ms(10);	 //EEPROM д����̱Ƚ�������ȴ�һ��ʱ�䣬��д��һ��
}
//��AT24CXX�����ָ����ַ��ʼд�볤��ΪLen������
//�ú�������д��16bit����32bit������.
//WriteAddr  :��ʼд��ĵ�ַ  
//DataToWrite:���������׵�ַ
//Len        :Ҫд�����ݵĳ���2,4
void AT24CXX_WriteLenByte(u16 WriteAddr,u32 DataToWrite,u8 Len)
{  	
	u8 t;
	CPU_TS *p_ts = NULL;
	OS_ERR err;
	OSSemPend(AT24CXX_Sem, 0, OS_OPT_PEND_BLOCKING, p_ts, &err);   // ��ȡ�ź���
	
	for(t=0;t<Len;t++)
	{
		AT24CXX_WriteOneByte(WriteAddr+t,(DataToWrite>>(8*t))&0xff);
	}				
  
	OSSemPost(AT24CXX_Sem, OS_OPT_POST_1, &err);	           // �ͷ��ź��� 
}

//��AT24CXX�����ָ����ַ��ʼ��������ΪLen������
//�ú������ڶ���16bit����32bit������.
//ReadAddr   :��ʼ�����ĵ�ַ 
//����ֵ     :����
//Len        :Ҫ�������ݵĳ���2,4
u32 AT24CXX_ReadLenByte(u16 ReadAddr,u8 Len)
{  	
	u8 t;
	u32 temp=0;
	CPU_TS *p_ts = NULL;
	OS_ERR err;
	OSSemPend(AT24CXX_Sem, 0, OS_OPT_PEND_BLOCKING, p_ts, &err);   // ��ȡ�ź���
	for(t=0;t<Len;t++)
	{
		temp<<=8;
		temp+=AT24CXX_ReadOneByte(ReadAddr+Len-t-1); 	 				   
	}
	OSSemPost(AT24CXX_Sem, OS_OPT_POST_1, &err);	           // �ͷ��ź���
	return temp;												    
}
//���AT24CXX�Ƿ�����
//��������24XX�����һ����ַ(255)���洢��־��.
//���������24Cϵ��,�����ַҪ�޸�
//����1:���ʧ��
//����0:���ɹ�
u8 AT24CXX_Check(void)
{
	u8 temp,tmpdata[4] = {0};
	temp=AT24CXX_ReadOneByte(AT24Cxx_CheckFlag_ADDR);//����ÿ�ο�����дAT24CXX			   
	if(temp==0X55)
	{
		return 0;
	}		
	else//�ų���һ�γ�ʼ�������
	{
		AT24CXX_WriteOneByte(AT24Cxx_CheckFlag_ADDR,0X55);
	  temp=AT24CXX_ReadOneByte(AT24Cxx_CheckFlag_ADDR);	  
		
	  AT24CXX_Write(AT24Cxx_RebootTimes_ADDR,tmpdata,4);
		
		if(temp==0X55)
		{
			return 0;
		}
	}
	return 1;											  
}

//��AT24CXX�����ָ����ַ��ʼ����ָ������������
//ReadAddr :��ʼ�����ĵ�ַ ��24c02Ϊ0~255
//pBuffer  :���������׵�ַ
//NumToRead:Ҫ�������ݵĸ���
void AT24CXX_Read(u16 ReadAddr,u8 *pBuffer,u16 NumToRead)
{
	CPU_TS *p_ts = NULL;
	OS_ERR err;
	OSSemPend(AT24CXX_Sem, 0, OS_OPT_PEND_BLOCKING, p_ts, &err);   // ��ȡ�ź���
	while(NumToRead)
	{
		*pBuffer++=AT24CXX_ReadOneByte(ReadAddr++);	
		NumToRead--;
	}
	OSSemPost(AT24CXX_Sem, OS_OPT_POST_1, &err);	           // �ͷ��ź���
}  
//��AT24CXX�����ָ����ַ��ʼд��ָ������������
//WriteAddr :��ʼд��ĵ�ַ ��24c02Ϊ0~255
//pBuffer   :���������׵�ַ
//NumToWrite:Ҫд�����ݵĸ���
void AT24CXX_Write(u16 WriteAddr,u8 *pBuffer,u16 NumToWrite)
{
	CPU_TS *p_ts = NULL;
	OS_ERR err;
	OSSemPend(AT24CXX_Sem, 0, OS_OPT_PEND_BLOCKING, p_ts, &err);   // ��ȡ�ź���
	while(NumToWrite--)
	{
		AT24CXX_WriteOneByte(WriteAddr,*pBuffer);
		WriteAddr++;
		pBuffer++;
	}
	OSSemPost(AT24CXX_Sem, OS_OPT_POST_1, &err);	           // �ͷ��ź���
}








