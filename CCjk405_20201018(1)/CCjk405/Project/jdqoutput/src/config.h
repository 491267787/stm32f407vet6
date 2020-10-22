/****************************************Copyright (c)**************************************************                                                           
**
**�ļ���: Config.h     
**
**����:  RM       �汾:v1.0        ����: 2010/4/26
**
**����:    			
**
**		�û������ļ�
**
**��ʷ��¼:        


****************************************************************************************************/
#ifndef __CONFIG_H 
#define __CONFIG_H

#include "ff.h"

//��һ������Ķ�
//This segment should not be modified
//#ifndef TRUE
//#define TRUE  1
//#endif
//
//#ifndef FALSE
//#define FALSE 0
//#endif
typedef long long      uint64;
typedef unsigned char  uint8;                   /* defined for unsigned 8-bits integer variable 	�޷���8λ���ͱ���  */
typedef signed   char  int8;                    /* defined for signed 8-bits integer variable		�з���8λ���ͱ���  */
typedef unsigned short uint16;                  /* defined for unsigned 16-bits integer variable 	�޷���16λ���ͱ��� */
typedef signed   short int16;                   /* defined for signed 16-bits integer variable 		�з���16λ���ͱ��� */
typedef unsigned int   uint32;                  /* defined for unsigned 32-bits integer variable 	�޷���32λ���ͱ��� */
typedef signed   int   int32;                   /* defined for signed 32-bits integer variable 		�з���32λ���ͱ��� */
typedef float          fp32;                    /* single precision floating point variable (32bits) �����ȸ�������32λ���ȣ� */
typedef double         fp64;                    /* double precision floating point variable (64bits) ˫���ȸ�������64λ���ȣ� */

#ifndef FALSE
typedef enum {FALSE = 0, TRUE = !FALSE} bool;
#endif

//rm add
typedef void 		(*FUNCPTR) ();	/* ptr to function returning int */
//typedef	unsigned char BOOL;
typedef void VOID;
/********************************/
/*      uC/OS-II specital code  */
/*      uC/OS-II���������      */
/********************************/

#define     USER_USING_MODE    0x10                    /*  User mode ,ARM 32BITS CODE �û�ģʽ,ARM����                  */
// 
                                                     /*  Chosen one from 0x10,0x30,0x1f,0x3f.ֻ����0x10,0x30,0x1f,0x3f֮һ       */
#include "Includes.h"
			



extern void print(uint8 data);
/********************************/
/*      ARM���������           */
/*      ARM specital code       */
/********************************/
//��һ������Ķ�
//This segment should not be modify
#if defined  STM32F10X_HD
	#include    "stm32f10x.h"
#else
	#include    "stm32f4xx.h"
#endif

/********************************/
/*     Ӧ�ó�������             */
/*Application Program Configurations*/
/********************************/
//���¸�����Ҫ�Ķ�
#include    <stdio.h>
#include    <stdlib.h>
#include    <setjmp.h>
#include    <rt_misc.h>


/********************************/
/*     �����ӵ�����             */
/*Configuration of the example */
/********************************/
/* System configuration .Fosc��Fcclk��Fcco��Fpclk must be defined */
/* ϵͳ����, Fosc��Fcclk��Fcco��Fpclk���붨��*/
#define Fosc            12000000                    //Crystal frequence,10MHz~25MHz��should be the same as actual status. 
						    //Ӧ����ʵ��һ������Ƶ��,10MHz~25MHz��Ӧ����ʵ��һ��
#define FCLK           ((Fosc/(5*2))*169)                  //System frequence,208MHZ
						   
#define FHCLK           (FCLK / 2)              //AHB BUS frequence

#define FPCLK           (FHCLK / 2)             //VPB clock frequence.
						            //VPBʱ��Ƶ�ʣ�ֻ��Ϊ(FCLK / 4)��1��2��4��

#define DISP_BGPIC 0 

#define Delay10Ms(x)  OSTimeDly((x)*2)		// ��ʱx*10����
#define Delay5Ms(x)   OSTimeDly((x))		  // ��ʱx*5����
#define DelaySec(x)   OSTimeDly(OS_TICKS_PER_SEC * x) 	// ��ʱx��

#define WAITMS(x)    ((x)/(1000/OS_TICKS_PER_SEC))   /*������ת��ϵͳ�δ���*/



#endif


/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/
