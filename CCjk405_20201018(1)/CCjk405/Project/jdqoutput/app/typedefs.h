/**
  ******************************************************************************
  * @file    typedefs.h
  * @author  zqj
  * @version v1.0
  * @date    2016-02-24
  * @brief   
  ******************************************************************************
	*/
#ifndef _TYPEDEFS_H_0001
#define _TYPEDEFS_H_0001

typedef long long      uint64;
typedef unsigned char  uint8;                   /* defined for unsigned 8-bits integer variable 	�޷���8λ���ͱ���  */
typedef signed   char  int8;                    /* defined for signed 8-bits integer variable		  �з���8λ���ͱ���  */
typedef unsigned short uint16;                  /* defined for unsigned 16-bits integer variable 	�޷���16λ���ͱ��� */
typedef signed   short int16;                   /* defined for signed 16-bits integer variable 		�з���16λ���ͱ��� */
typedef unsigned int   uint32;                  /* defined for unsigned 32-bits integer variable 	�޷���32λ���ͱ��� */
typedef signed   int   int32;                   /* defined for signed 32-bits integer variable 		�з���32λ���ͱ��� */
typedef float          fp32;                    /* single precision floating point variable (32bits)    �����ȸ�������32λ���ȣ� */
typedef double         fp64;                    /* double precision floating point variable (64bits)    ˫���ȸ�������64λ���ȣ� */
//typedef unsigned long long  uint64;


//typedef	unsigned char BOOL;
typedef void VOID;

//#define  NULL   ( (void *)0 )

typedef int32 (*FptrWriteData)(uint8 *buf,uint16 len); 
typedef int32 (*FptrReadData)(uint8 *buf,uint16 len);

#undef  NULL
#define NULL (void *)0

//#ifndef __cplusplus
//typedef enum {FALSE = 0, TRUE = !FALSE} bool;
//#endif


#endif


