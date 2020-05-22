#include "ds18b20.h"
#include "delay.h"	
//////////////////////////////////////////////////////////////////////////////////	    
//���ܣ�DS18B20��������
//���ߣ�
//��������:2020/05/10
//�汾��V1.0	
//������DS18B20 �ĵ����¶ȶ�ȡ����Ϊ����λ -> �� SKIP ROM ���0XCC��-> ����ʼת�����0X44��-> \
        ��ʱ-> ��λ -> ���� SKIP ROM ���0XCC��-> �����洢�����0XBE��-> �������������ֽ�����(�� \
        �¶�) -> ������
//��ע��������ʱ�����
//1����λ�����Ӧ������
//�������ϵ�����ͨ�Ŷ����Գ�ʼ�����п�ʼ����������͵�ƽ�����ֵ͵�ƽʱ������ 480
//us�����Բ�����λ���塣���������ͷ����ߣ�4.7K ���������轫���������ߣ���ʱ 15��60 us��
//���������ģʽ(Rx)������ DS18B20 �������� 60~240 us���Բ����͵�ƽӦ�����壬
//��Ϊ�͵�ƽ������ʱ 480 us��
//2��дʱ��
//дʱ�����д 0 ʱ���д 1 ʱ������дʱ��������Ҫ 60us������ 2 �ζ�����дʱ��֮��
//������Ҫ 1us �Ļָ�ʱ�䣬����дʱ�����ʼ�������������ߡ�д 1 ʱ����������͵�ƽ��
//��ʱ 2us��Ȼ���ͷ����ߣ���ʱ 60us��д 0 ʱ����������͵�ƽ����ʱ 60us��Ȼ���ͷ����ߣ�
//��ʱ 2us��
//3����ʱ��
//������������������������ʱ��ʱ�����������������ݣ����ԣ����������������������
//�������ϲ�����ʱ���Ա�ӻ��ܹ��������ݡ����ж�ʱ��������Ҫ 60us������ 2 �ζ����Ķ�
//ʱ��֮��������Ҫ 1us �Ļָ�ʱ�䡣ÿ����ʱ������������������������ 1us�������ڶ�
//ʱ���ڼ�����ͷ����ߣ�������ʱ����ʼ��� 15us ֮�ڲ�������״̬�����͵Ķ�ʱ�����Ϊ��
//��������͵�ƽ��ʱ 2us��Ȼ������ת������ģʽ��ʱ 12us��Ȼ���ȡ�����ߵ�ǰ�ĵ�ƽ��Ȼ
//����ʱ 50us
//////////////////////////////////////////////////////////////////////////////////
  

/****************************************************************************************
** ��������: 
** ��������: ��λDS18B20
** �䡡��: 
** �䡡��: 
** ȫ�ֱ���:
** ����:   
******************************************************************************************/
void DS18B20_Rst(void)	   
{                 
	DS18B20_IO_OUT(); //SET PE2 OUTPUT
  DS18B20_DQ_OUT=0; //����DQ
  delay_us(750);    //����750us
  DS18B20_DQ_OUT=1; //DQ=1 
	delay_us(15);     //15US
}

/****************************************************************************************
** ��������: 
** ��������: �ȴ�DS18B20�Ļ�Ӧ
** �䡡��: 
** �䡡��: 1:δ��⵽DS18B20�Ĵ���        0:����
** ȫ�ֱ���:
** ����:   
******************************************************************************************/
u8 DS18B20_Check(void) 	   
{   
	u8 retry=0;
	DS18B20_IO_IN();//SET PE2 INPUT	 
  while (DS18B20_DQ_IN&&retry<200)
	{
		retry++;
		delay_us(1);
	};	 
	if(retry>=200)
		return 1;
	else 
		retry=0;
  while (!DS18B20_DQ_IN&&retry<240)
	{
		retry++;
		delay_us(1);
	};
	if(retry>=240)
		return 1;	    
	return 0;
}

/****************************************************************************************
** ��������: 
** ��������: ��DS18B20��ȡһ��λ
** �䡡��: 
** �䡡��: 1/0
** ȫ�ֱ���:
** ����:   
******************************************************************************************/
u8 DS18B20_Read_Bit(void) 			 // read one bit
{
  u8 data;
	DS18B20_IO_OUT();//SET PE2 OUTPUT
  DS18B20_DQ_OUT=0; 
	delay_us(2);
  DS18B20_DQ_OUT=1; 
	DS18B20_IO_IN();//SET PE2 INPUT
	delay_us(12);
	if(DS18B20_DQ_IN)
		data=1;
  else 
		data=0;	 
  delay_us(50);           
  return data;
}

/****************************************************************************************
** ��������: 
** ��������: ��DS18B20��ȡһ���ֽ�
** �䡡��: 
** �䡡��: ����������
** ȫ�ֱ���:
** ����:   
******************************************************************************************/
u8 DS18B20_Read_Byte(void)    // read one byte
{        
  u8 i,j,dat;
  dat=0;
	for (i=1;i<=8;i++) 
	{
    j=DS18B20_Read_Bit();
    dat=(j<<7)|(dat>>1);
  }						    
  return dat;
}

/****************************************************************************************
** ��������: 
** ��������: дһ���ֽڵ�DS18B20
** �䡡��: Ҫд����ֽ�
** �䡡��: 
** ȫ�ֱ���:
** ����:   
******************************************************************************************/
void DS18B20_Write_Byte(u8 dat)     
{             
  u8 j;
  u8 testb;
	DS18B20_IO_OUT();//SET PE2 OUTPUT;
  for (j=1;j<=8;j++) 
	{
    testb=dat&0x01;
    dat=dat>>1;
    if (testb) 
    {
      DS18B20_DQ_OUT=0;// Write 1
      delay_us(2);                            
      DS18B20_DQ_OUT=1;
      delay_us(60);             
    }
    else 
    {
      DS18B20_DQ_OUT=0;// Write 0
      delay_us(60);             
      DS18B20_DQ_OUT=1;
      delay_us(2);                          
    }
  }
}
 
/****************************************************************************************
** ��������: 
** ��������: ��ʼ�¶�ת��
** �䡡��: 
** �䡡��: 
** ȫ�ֱ���:
** ����:   
******************************************************************************************/
void DS18B20_Start(void)// ds1820 start convert
{   						               
    DS18B20_Rst();	   
	  DS18B20_Check();	 
    DS18B20_Write_Byte(0xcc);// skip rom
    DS18B20_Write_Byte(0x44);// convert
} 

/****************************************************************************************
** ��������: 
** ��������: ��ʼ��DS18B20��IO�� DQ ͬʱ���DS�Ĵ���
** �䡡��: 
** �䡡��: 1:������    0:����  
** ȫ�ֱ���:
** ����:   
******************************************************************************************/  	 
u8 DS18B20_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);//ʹ��GPIOEʱ��

  //GPIOE2
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//50MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_Init(GPIOE, &GPIO_InitStructure);//��ʼ��
 
 	DS18B20_Rst();
//	return DS18B20_Check();
}  

/****************************************************************************************
** ��������: 
** ��������: ��ds18b20�õ��¶�ֵ     ���ȣ�0.1C
** �䡡��: 
** �䡡��: �¶�ֵ ��-550~1250�� 
** ȫ�ֱ���:
** ����:   
******************************************************************************************/
short DS18B20_Get_Temp(void)
{
  u8 temp;
  u8 TL,TH;
	short tem;
  DS18B20_Start ();                    // ds1820 start convert
  DS18B20_Rst();
  DS18B20_Check();	 
  DS18B20_Write_Byte(0xcc);// skip rom
  DS18B20_Write_Byte(0xbe);// convert	    
  TL=DS18B20_Read_Byte(); // LSB   
  TH=DS18B20_Read_Byte(); // MSB   
  if(TH>7)
  {
    TH=~TH;
    TL=~TL; 
    temp=0;//�¶�Ϊ��  
  }
	else 
	  temp=1;//�¶�Ϊ��	  	  
  tem=TH; //��ø߰�λ
  tem<<=8;    
  tem+=TL;//��õװ�λ
  tem=(double)tem*0.625;//ת��     
	if(temp)
		return tem; //�����¶�ֵ
	else 
		return -tem;    
}















