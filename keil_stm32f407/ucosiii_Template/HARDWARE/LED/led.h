#ifndef _LED_H
#define _LED_H
#include "sys.h"
#include "cpu.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//LED��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/5/2
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

#define LED_ALL    0
#define LED1       1
#define LED2       2
#define LED3       3


void LED_Init(void);  //��ʼ��
void LED_Off (CPU_INT08U  led);
void LED_On (CPU_INT08U led);
void LED_Toggle (CPU_INT08U  led);
#endif
