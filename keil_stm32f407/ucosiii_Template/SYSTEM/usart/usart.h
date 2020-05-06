#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "stm32f4xx_conf.h"
#include "sys.h" 
#include "os.h" 



//#define USART_REC_LEN  			200  	//�����������ֽ��� 200
//#define EN_USART1_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����
//	  	
//extern u8  USART_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
//extern u16 USART_RX_STA;         		//����״̬���	
////����봮���жϽ��գ��벻Ҫע�����º궨��
//void uart_init(u32 bound);


#define STM32F4XX  1 
#define UART_SUPORT_GPS_EN	1   //����֧��GPS

#define DEF_PORT_UART 0

#define COM1        0
#define COM2        1
#define COM3        2
#define COM4        3
#define COM5        4
#define COM6				5

	
#define SCREEN_COM  COM4
#define CARD_COM    COM2
#define METER_COM   COM5  
#define ISO_COM     COM3


#define ACMETER_COM COM6
#define MOD_COM     COM3
#define BACK_COM    COM1

enum HFControl							 
{
	HFC_NONE,
	HFC_CTRL,
};

#define N_UART_CHANNELS	2 	        // rs232/485 ����

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
#define SIO_START	0x2008 /*channel start*/
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
#define CS5		0x0	/* 5 bits */
#define CS6		0x4	/* 6 bits */
#define CS7		0x8	/* 7 bits */
#define CS8		0xc	/* 8 bits */

#define HUPCL		0x10	/* hang up on last close */
#define STOPB		0x20	/* send two stop bits (else one) */

#define PARENB		0x1c0	/* parity detection setect */
#define PARODD		0x40	/* odd parity*/
#define PAREVEN		0x80	/* even parity  */
#define PARMASK		0xc0	/*Forced ��1�� stick parity*/
#define PARSPACE	0x100	/* Forced ��0�� stick parity  */



//#define 	MAX_UART_RCVBUF_LENTH    512    //   �����շ���������С
//#define   MAX_UART_SNDBUF_LENTH    512

#define   MAX_BACKRCV_LENGTH    700
#define   MAX_BACKSND_LENGTH    2048
 
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
	uint8_t startflag;
	uint8_t head1;
	uint8_t head2;
	uint8_t count;
	uint32_t RevGpsSynTime;//�����յ�GPSͬ����ʱ��
	uint8_t gpsbuf[50];
}gpsuart_t;	

typedef struct {
	void*		baseAddr;	          	// register base address
	void*		directionGPIO;	      // direction contrl pin's GPIO
	uint32_t	rs485ContrlPin;       // rs485  direction contrl pin
	uint8_t		devicType;		      	// 0= rs232 , 1= rs485*/
	uint32_t	defBuardRate;         // Ĭ�ϲ����� 
	uint16_t		defWordLength;        // Ĭ������λ 5-8 
	uint16_t		defStopbits;          // Ĭ��ֹͣλ 1-2 
	uint16_t		defParity;            // Ĭ��У��λ
	uint16_t		HardwareFlowControl;  // Ӳ������ 
	//20130904
	UART_USE_TYPE		useType;	//UART��;����
} STM32F10x_SIO_CHAN_PARAM;	

#define IS_VALID_USE_TYPE(arg) ((arg == UART_USE_TYPE_NOMAL) || (arg == UART_USE_TYPE_GPS))



/* Device and Channel Structures */
typedef struct {
	OS_SEM*  uartLock;//OS_EVENT* 	uartLock;	                 		// devece mutex lock	
	void*				base;	                     		// register base address
	void*				directionGPIO;	              // direction contrl pin's GPIO
	uint32_t			rs485ContrlPin;               // rs485  direction contrl pin
	uint32_t			deviceID;                     // uart ID 
	uint8_t				deviceOpenFlag;	              // device flag opened
	uint8_t				devicType;		             		// 0= rs232 , 1= rs485
	int32_t   		baudRate;                     // baud rate 
	uint16_t				WordLength;        			 			// Ĭ������λ 5-8 
	uint16_t				Stopbits;          			 			// Ĭ��ֹͣλ 1-2 
	uint16_t				Parity;            			 			// Ĭ��У��λ
	uint16_t				HardwareFlowControl;  		 		// Ӳ������ 

	int32_t				sioMode;                     	// SIO mode
	int32_t   		options;                     	// SIO options
//	uint32_t 			read_status_mask;
	uint8_t				*SendBuf;  //[MAX_UART_SNDBUF_LENTH]; // send    buf
	uint8_t				*RcvBuf;    //[MAX_UART_RCVBUF_LENTH];  // receive buf
	uint32_t  		SendTimer;                  	 // ���ڷ��ͳ�ʱ������
	UART_SND_STA	SendStatus;//���ڷ��Ϳ�����״̬
	gpsuart_t Timegps;
	UART_USE_TYPE		useType;	//UART��;����
} STM32F10x_UART_CHAN;


extern uint32_t UartHandler[];


// Ӳ����ʼ��
void sys_SerialHwInit(void);
//�豸��
extern int32_t Uart_Open(uint32_t deviceId);/*device ID*/ 
extern STM32F10x_UART_CHAN *GetChanHandler(uint8_t id);
//�豸�ر�
extern int32_t Uart_Close(uint32_t devDesc);/*device description*/	

//���豸�����ݣ����ض��������ݳ���
extern int32_t Uart_Read(int32_t devDesc,	/*device description*/
				uint8_t *buf,	            /*save to buffer*/
				uint16_t maxSize          // ����ȡ���� 
				);
//д������,���سɹ�д�볤��
extern int32_t Uart_Write(int32_t devDesc,	/*device description*/
				uint8_t *buf,	/* buffer*/
				int32_t size	/*data size writed*/
				);

//���ڲ�������
extern int Uart_IOCtrl(int32_t devDesc,	/*device description*/
			int32_t cmd,	/*request cmd*/
			void* arg	/*some argument*/
			);

//���մ��ڻ�����
extern int32_t Uart_ClearReadBuf(int32_t devDesc);

//?????????			
extern uint16_t Uart_GetBytesNumInBuff(int32_t devDesc);
#ifdef  __cplusplus
}
#endif

#endif  /* _UART_H */


