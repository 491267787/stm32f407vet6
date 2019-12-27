#include "led.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//LED��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/6/10
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	



//��ʼ��PE13,PE14��PF15Ϊ�����.��ʹ���������ڵ�ʱ��		    
//LED IO��ʼ��
void LED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE); //ʹ��GPIOD��ʱ��
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;//���
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;  //�������
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;  //�������
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz; //����GPIO
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	
	GPIO_SetBits(GPIOE,GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15); //GPIOE �ߵ�ƽ
}




/*
*********************************************************************************************************
*                                              LED_Off()
*
* Description : Turn OFF any or all the LEDs on the board.
*
* Argument(s) : led     The ID of the LED to control:
*
*                       0    turns OFF ALL the LEDs
*                       1    turns OFF user LED1
*                       2    turns OFF user LED2
*                       3    turns OFF user LED3
*                       4    turns OFF user LED4
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  LED_Off (CPU_INT08U  led)
{
    switch (led) {
        case LED_ALL:
             GPIO_SetBits(GPIOE, GPIO_Pin_13);
             GPIO_SetBits(GPIOE, GPIO_Pin_14);
             GPIO_SetBits(GPIOE, GPIO_Pin_15);
             break;


        case LED1:
             GPIO_SetBits(GPIOE, GPIO_Pin_13);
             break;


        case LED2:
             GPIO_SetBits(GPIOE, GPIO_Pin_14);
             break;


        case LED3:
             GPIO_SetBits(GPIOE, GPIO_Pin_15);
             break;


        default:
             break;
    }
}


/*
*********************************************************************************************************
*                                             BSP_LED_On()
*
* Description : Turn ON any or all the LEDs on the board.
*
* Argument(s) : led     The ID of the LED to control:
*
*                       0    turns ON ALL  LEDs
*                       1    turns ON user LED1
*                       2    turns ON user LED2
*                       3    turns ON user LED3
*                       4    turns ON user LED4
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  LED_On (CPU_INT08U led)
{
    switch (led) {
        case LED_ALL:
             GPIO_ResetBits(GPIOE, GPIO_Pin_13);
             GPIO_ResetBits(GPIOE, GPIO_Pin_14);
             GPIO_ResetBits(GPIOE, GPIO_Pin_15);
             break;


        case LED1:
             GPIO_ResetBits(GPIOE, GPIO_Pin_13);
             break;


        case LED2:
             GPIO_ResetBits(GPIOE, GPIO_Pin_14);
             break;


        case LED3:
             GPIO_ResetBits(GPIOE, GPIO_Pin_15);
             break;


        default:
             break;
    }
}


/*
*********************************************************************************************************
*                                            BSP_LED_Toggle()
*
* Description : TOGGLE any or all the LEDs on the board.
*
* Argument(s) : led     The ID of the LED to control:
*
*                       0    TOGGLE ALL the LEDs
*                       1    TOGGLE user LED1
*                       2    TOGGLE user LED2
*                       3    TOGGLE user LED3
*                       4    TOGGLE user LED4
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  LED_Toggle (CPU_INT08U  led)
{
    CPU_INT16U  pins;


    switch (led) {
        case LED_ALL:
             pins  =  GPIO_ReadOutputData(GPIOE);               /* Read data (current state of output pins) in GPIOA    */
             pins ^=  GPIO_Pin_13;                           /* Tells which pins (other than pin for LED1) are ON    */
             GPIO_SetBits(  GPIOE,   pins  &  GPIO_Pin_13);  /* If there are any,turn them OFF and leave LED1 ON     */
             GPIO_ResetBits(GPIOE, (~pins) &  GPIO_Pin_13);

             pins  =  GPIO_ReadOutputData(GPIOE);
             pins ^=  GPIO_Pin_14;
             GPIO_SetBits(  GPIOE,   pins  &  GPIO_Pin_14);
             GPIO_ResetBits(GPIOE, (~pins) &  GPIO_Pin_14);

             pins  =  GPIO_ReadOutputData(GPIOE);
             pins ^= (GPIO_Pin_15);
             GPIO_SetBits(  GPIOE,   pins  & GPIO_Pin_15);
             GPIO_ResetBits(GPIOE, (~pins) & GPIO_Pin_15);
             break;


        case LED1:
             pins  = GPIO_ReadOutputData(GPIOE);
             pins ^= GPIO_Pin_13;
             GPIO_SetBits(  GPIOE,   pins   &  GPIO_Pin_13);
             GPIO_ResetBits(GPIOE, (~pins)  &  GPIO_Pin_13);
             break;


        case LED2:
             pins  = GPIO_ReadOutputData(GPIOE);
             pins ^= GPIO_Pin_14;
             GPIO_SetBits(  GPIOE,   pins   &  GPIO_Pin_14);
             GPIO_ResetBits(GPIOE, (~pins)  &  GPIO_Pin_14);
             break;


        case LED3:
             pins  = GPIO_ReadOutputData(GPIOE);
             pins ^= GPIO_Pin_15;
             GPIO_SetBits(  GPIOE,   pins   &  GPIO_Pin_15);
             GPIO_ResetBits(GPIOE, (~pins)  &  GPIO_Pin_15);
             break;

        default:
             break;
    }
}