/****************************************Copyright (c)**************************************************

****************************************************************************************************/
#ifndef _UART_H
#define _UART_H

#ifdef  __cplusplus
extern  "C" {
#endif

#include "config.h"
#include "../queue/queue.h"

//#define STM32F4XX  1 
#define UART_SUPORT_GPS_EN	1   //����֧��GPS

#define DEF_PORT_UART 0

#define COM1        0
#define COM2        1
#define COM3        2
#define COM4        3
#define COM5        4
#define COM6		5

 
#define BACK_COM   		COM1  
#define SCREEN_COM    	COM2  //COM1 	COM4
#define CARD_COM    	COM5 

#if 0
#define ISO_COM     COM3
#define SCREEN_COM  COM4
#define CARD_COM    COM5 
#define ACMETER_COM COM6
#endif

enum HFControl							 
{
	HFC_NONE,
	HFC_CTRL,
};

#define N_UART_CHANNELS		6 	        // rs232/485 ����

#define	RS485TYPE	0
#define	RS232TYPE	1
#define	IRDATYPE	2			   //����ģʽ
#define	IR38KHZTYPE	3			   //38K�����ͺ���ģʽ


#define SIO_BAUD_SET		0x1003	  	/* serial device I/O controls */
#define SIO_BAUD_GET		0x1004

#define SIO_HW_OPTS_SET		0x1005
#define SIO_HW_OPTS_GET		0x1006

#define SIO_MODE_SET		0x1007
#define SIO_MODE_GET		0x1008
#define SIO_AVAIL_MODES_GET	0x1009
#define SIO_START			0x2008 /*channel start*/
#define SIO_WORK_TYPE_SET	0x2009 /*type set(rs232 or rs485)*/
#define SIO_WORK_TYPE_GET	0x200A /*type Get(rs232 or rs485)*/

#define SIO_PARITY_BIT_SET	0x3001/*Parity set*/

#define SIO_STOP_BIT_SET	0x3003/*STOP BIT set*/

#define SIO_DATA_BIT_SET	0x3005/*DATA set*/

#define SIO_SEND_STATUS_GET		0x1020 //��÷���״̬
#define SIO_SEND_BUF_FULL_STATUS_GET		0x1022 //��÷��ͻ�����״̬

#define SIO_USE_TYPE_SET		0x1040 //�����豸ʹ������
#define SIO_USE_TYPE_GET		0x1041 //����豸ʹ������

#define SIO_GPS_SYN_TIME_GET		0x1051 //�������GPSͬ��ʱ�̵�ϵͳ������ʱ��

#define SIO_GET_PORT_HANDLE     0x1060  //��ö˿ھ��
//

/* options to SIO_HW_OPTS_SET (ala POSIX), bitwise or'ed together */ 
#define CSIZE		0xc	/* bits 3 and 4 encode the character size */
#define CS5			0x0	/* 5 bits */
#define CS6			0x4	/* 6 bits */
#define CS7			0x8	/* 7 bits */
#define CS8			0xc	/* 8 bits */

#define HUPCL		0x10	/* hang up on last close */
#define STOPB		0x20	/* send two stop bits (else one) */

#define PARENB		0x1c0	/* parity detection setect */
#define PARODD		0x40	/* odd parity*/
#define PAREVEN		0x80	/* even parity  */
#define PARMASK		0xc0	/*Forced ��1�� stick parity*/
#define PARSPACE	0x100	/* Forced ��0�� stick parity  */



#define MAX_UART_BUF_LENTH 		256  //patli 2020018 512    //   �����շ���������С
 
typedef enum{
	UART_SEND_IDLE,//���Ϳ���
	UART_SEND_BUSY,//����æ
}UART_SND_STA;//���ڷ���״̬

//UART���ͻ�����״̬
typedef enum{
	UART_SEND_BUF_NO_FULL,//���Ͳ���
	UART_SEND_BUF_FULL,//������
}UART_SND_BUF_FULL_STA;//CAN�ڷ�����״̬

//UART��;����
typedef enum{
	UART_USE_TYPE_NOMAL=0,//ͨ�ô���
	UART_USE_TYPE_GPS,//GPRS����
}UART_USE_TYPE;//UART����;����

typedef struct
{
	uint8 startflag;
	uint8 head1;
	uint8 head2;
	uint8 count;
	uint32 RevGpsSynTime;//�����յ�GPSͬ����ʱ��
	uint8 gpsbuf[50];
}gpsuart_t;	

typedef struct {
	void*		baseAddr;	          	// register base address
	void*		directionGPIO;	      // direction contrl pin's GPIO
	uint32	rs485ContrlPin;       // rs485  direction contrl pin
	uint8		devicType;		      	// 0= rs232 , 1= rs485*/
	uint32	defBuardRate;         // Ĭ�ϲ����� 
	uint16		defWordLength;        // Ĭ������λ 5-8 
	uint16		defStopbits;          // Ĭ��ֹͣλ 1-2 
	uint16		defParity;            // Ĭ��У��λ
	uint16		HardwareFlowControl;  // Ӳ������ 
	//20130904
	UART_USE_TYPE		useType;	//UART��;����
} STM32F10x_SIO_CHAN_PARAM;	

#define IS_VALID_USE_TYPE(arg) ((arg == UART_USE_TYPE_NOMAL) || (arg == UART_USE_TYPE_GPS))



/* Device and Channel Structures */
typedef struct {
	OS_EVENT* 	uartLock;	                 		// devece mutex lock	
	void*				base;	                     		// register base address
	void*				directionGPIO;	              // direction contrl pin's GPIO
	uint32			rs485ContrlPin;               // rs485  direction contrl pin
	uint32			deviceID;                     // uart ID 
	uint8				deviceOpenFlag;	              // device flag opened
	uint8				devicType;		             		// 0= rs232 , 1= rs485
	int32   		baudRate;                     // baud rate 
	uint16				WordLength;        			 			// Ĭ������λ 5-8 
	uint16				Stopbits;          			 			// Ĭ��ֹͣλ 1-2 
	uint16				Parity;            			 			// Ĭ��У��λ
	uint16				HardwareFlowControl;  		 		// Ӳ������ 

	int32				sioMode;                     	// SIO mode
	int32   		options;                     	// SIO options
//	uint32 			read_status_mask;
	uint8				SendBuf[MAX_UART_BUF_LENTH]; // send    buf
	uint8				RcvBuf[MAX_UART_BUF_LENTH];  // receive buf
	uint32  		SendTimer;                  	 // ���ڷ��ͳ�ʱ������
	UART_SND_STA	SendStatus;//���ڷ��Ϳ�����״̬
	gpsuart_t Timegps;
	UART_USE_TYPE		useType;	//UART��;����
} STM32F10x_UART_CHAN;


extern uint32 UartHandler[];


// Ӳ����ʼ��
void sysSerialHwInit(void);
//�豸��
extern int32 Uart_Open(uint32 deviceId);/*device ID*/ 
extern STM32F10x_UART_CHAN *GetChanHandler(uint8 id);
//�豸�ر�
extern int32 Uart_Close(uint32 devDesc);/*device description*/	

//���豸�����ݣ����ض��������ݳ���
extern int32 Uart_Read(int32 devDesc,	/*device description*/
				uint8 *buf,	            /*save to buffer*/
				uint16 maxSize          // ����ȡ���� 
				);
//д������,���سɹ�д�볤��
extern int32 Uart_Write(int32 devDesc,	/*device description*/
				uint8 *buf,	/* buffer*/
				int32 size	/*data size writed*/
				);

//uartֱ��д������,���سɹ�д�볤��
extern void Uart_Send_data(int32 devDesc, u8 *s, uint16 size);


//���ڲ�������
extern int Uart_IOCtrl(int32 devDesc,	/*device description*/
			int32 cmd,	/*request cmd*/
			void* arg	/*some argument*/
			);

//���մ��ڻ�����
extern int32 Uart_ClearReadBuf(int32 devDesc);

//?????????			
extern uint16 Uart_GetBytesNumInBuff(int32 devDesc);
#ifdef  __cplusplus
}
#endif

#endif  /* _UART_H */


/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/



