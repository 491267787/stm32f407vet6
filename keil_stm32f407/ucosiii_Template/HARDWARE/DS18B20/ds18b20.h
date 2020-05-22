#ifndef __DS18B20_H
#define __DS18B20_H 
#include "sys.h"   
//////////////////////////////////////////////////////////////////////////////////	 
//���ܣ�DS18B20��������
//���ߣ�
//��������:2020/05/10
//�汾��V1.0							  
//////////////////////////////////////////////////////////////////////////////////

//IO��������
#define DS18B20_IO_IN()  {GPIOE->MODER&=~(3<<(2*2));GPIOG->MODER|=0<<2*2;}	//PE2����ģʽ
#define DS18B20_IO_OUT() {GPIOG->MODER&=~(3<<(2*2));GPIOG->MODER|=1<<2*2;} 	//PE2���ģʽ
 
////IO��������											   
#define	DS18B20_DQ_OUT PEout(2) //���ݶ˿�	PE2
#define	DS18B20_DQ_IN  PEin(2)  //���ݶ˿�	PE2 
   	
u8 DS18B20_Init(void);			//��ʼ��DS18B20
short DS18B20_Get_Temp(void);	//��ȡ�¶�
void DS18B20_Start(void);		//��ʼ�¶�ת��
void DS18B20_Write_Byte(u8 dat);//д��һ���ֽ�
u8 DS18B20_Read_Byte(void);		//����һ���ֽ�
u8 DS18B20_Read_Bit(void);		//����һ��λ
u8 DS18B20_Check(void);			//����Ƿ����DS18B20
void DS18B20_Rst(void);			//��λDS18B20    
#endif














