/****************************************************************************************
Filename: Adebug.h
Author: patli
Datetime: 20190930
Version: v0.01
****************************************************************************************/

#ifndef ADEBUG_H
#define ADEBUG_H
#include "typedefs.h"
#include "M_Global.h"


#define TCP_CONNECT_ON  1
#define TCP_CONNECT_OFF  0

#define LOG_ON  1
#define LOG_OFF  0

#define DISABLE_ON  1
#define DISABLE_OFF  0

#define RESTART_ON  1
#define RESTART_OFF  0


#if 0
#define AUNICE_DEBUG			//����������LOG����
//#define NET_DEBUG					//��̫�����LOG����
#define SCOMM_DEBUG				//����ͨ�����LOG����
#endif


#ifdef AUNICE_DEBUG

#if (PRODUCTS_LEVEL == DEBUG_VERSION ||PRODUCTS_LEVEL == RELEASE_VERSION) 				  //���԰汾

//#define MODULE_DEBUG           //��Դģ��LOG�������
#define BMS_DEBUG              //BMS LOG�������
#define CONTROL_DEBUG						//����������LOG�������
//#define SCREEN_DEBUG         		//��Ļ��ʾlog�������
//#define BACK_COMM_DEBUG					//��̨log�������

#define NET_CONTROL_LOG				//��̨����ģ����־

#define WRITE_CACHE					//д����

#else

#define FTP_DEBUG					//ftp log�������

#endif


#endif

#ifdef NET_DEBUG

#define SOH 0x75   	//0x01
#define STX 0x72	//0x02
#define ETX 0x68	//0x03
#define EOT 0x6A	//0x04

#define SEND_DIR 	0   //DEV SEND TO NETDEBUG
#define RECV_DIR 	1	//NET SEND TO DEV

typedef struct NetDebugProtoHandle
{
	uint8   cmd;                            /*����*/
	void  (*func)(uint8 dir);   /*����*/
}NetDebugProtoHandle_t;

typedef enum
{
	DEV_LOGIN_REQ 		= 0x00,  /*DEV LOGIN REQ*/
	NETDEBUG_LOGIN_RES	= 0x01,  /*NETDEBUG  LOGIN RES FOR DEV*/
	DEV_TO_NETDEBUG_HB_REQ 	= 0x10,  /*DEV TO NETDEBUG ����REQ*/
	NETDEBUG_TO_DEV_HB_RES 	= 0x11,	 /*NETDEBUG  TO  DEV����RES*/
}NetDebugProtoCmd;

typedef struct NetDebugProtoFrame
{
	uint8   soh;                            /*֡ͷ 0x01 */
	uint8   stx;                            /*֡ͷ 0x02 */
	uint16  frame_len;						// frame len(cmd + data)
	uint8 	cmd;
	uint8   data[128];						//data
	uint8 	etx;                            //���Ľ���0x03
	uint32  crc;							//У���
	uint8   eot;							//�������0x04
}NetDebugProtoFrame_t;




#endif

typedef  struct MOD_LOG_BIT
{
	 union {
		uint32 dword;
		 struct {
			uint32 mod:1;
			uint32 bms:1;
			uint32 con:1;
			uint32 scr:1;
			uint32 bac:1;
			uint32 tcp:1;
			uint32 ftp:1;
			uint32 usb:1;
			uint32 chk:1;
			uint32 rem:23;   //������

		}bits;
	}mod_log;
}MOD_LOG_BIT_t;


//���ڵ���log ������
typedef enum{
	MOD_MOD,
	BMS_MOD,
	CON_MOD,
	SCR_MOD,
	BAC_MOD,
	TCP_MOD,
	FTP_MOD,
	USB_MOD,
	CHK_MOD,
	UNK_MOD
}MOD_DEBUG;



#define USER_PROGRAM_ON        	0x01     //�����������������������û�������� 
#define USER_PROGRAM_OFF        0x00 	 //�û������������

#define setbit(x,y) x|=(1<<y)
#define clrbit(x,y) x&=~(1<<y)
#define reversebit(x,y) x^=(1<<y)
#define getbit(x,y) ((x)>>(y)&1)


extern uint32 DevRunSec;     //�豸���ж�����

extern uint16 task_run_status;  //���������б�־
extern uint16 uart_status;  //all uart send and recv status,  high byte is recv status, low byte is send status
extern uint8  can_status;   //all can send and recv status, high 4 bits is recv, low 4bits is send status
extern uint8  gun_status;   //all gun status, high 4bits is B gun 4bit--cc1 5bit---locked, low 4bits is A gun 0bit---cc1 1bit-----locked;
extern uint8  msg_full;	//message queue is full, 0---no full; 1---full

extern void ClearDebugStatus(void);

#ifdef SCOMM_DEBUG   //����ͨ�ŵ��Խӿ�
extern void ScommDebugPrintStr(char* buf);

extern void ScommDebugPrintStrWithPx(char* buf, int32 param);

extern void ScommDebugPrintStrWithPi(char* buf, int32 param);

extern void ScommDebugPrintStrWithPf(char* buf, float param);

extern void ScommDebugPrintStrWithLen(char* buf, uint16 len);
extern void Print_data(int32 devDesc, u8 *s, uint16 len);

#endif

#ifdef NET_DEBUG
extern void Debug_Get_Mac();

extern void Net_Debug_Write(uint8 *buf, uint16 len, uint8 isbug);
extern uint8 IsNetDebugConnect();
extern void SendMsgToNetDebug(uint8 Type,uint8 Len,void *Data);
extern void SendMsgToNetDebugByPtr(uint8 Type,uint8 Len,void *Data);

extern uint8 net_debug_lwiptcp_Init(void);

//extern uint8 TcpDebugHandleMsg(MSG_STRUCT *msg);

extern uint8 net_debug_try_connect();   //retrurn 1---success; 0-----fail

extern void net_debug_thread_mian_proc();   
extern void net_debug_close();

#endif

extern void TaskRunTimePrint(char* buf, int32 param);

extern int8 adebug_init(void);

//��ģ��ID��int32�����ĵ��Խӿ�
extern void DebugInfoWithPx(uint8 mod, uint8 *buf, int32 param);

//��ģ��ID��int32�����ĵ��Խӿ�
extern void DebugInfoWithPi(uint8 mod, uint8 *buf, int32 param);

//��ģ��ID��float�����ĵ��Խӿ�
extern void DebugInfoWithPf(uint8 mod, uint8 *buf, float param);

extern void DebugInfoWithPbuf(uint8 mod, uint8 *buf, uint8 *data, uint16 data_len, uint8 mod_sec);

//��ģ��ID�ĵ��Խӿ�
extern void DebugInfo(uint8 mod, uint8 *buf);

//��Դģ��LOG���Խӿ�
extern void DebugInfoByMod(uint8 *buf);

//�복��BMSͨ��LOG���Խӿ�
extern void DebugInfoByBms(uint8 *buf);

//��������LOG���Խӿ�
extern void DebugInfoByCon(uint8 *buf);

//��Ļlog���Խӿ�
extern void DebugInfoByScr(uint8 *buf);

//��̨log���Խӿ�
extern void DebugInfoByBack(uint8 *buf);

//ftp log���Խӿ�
extern void DebugInfoByFtp(uint8 *buf);

extern void DebugInfoByChk(uint8 *buf);

#define DEBUG_MAX_BUF_LEN 512   //640

#define WRITE_CACHE_BUF_LEN  DEBUG_MAX_BUF_LEN  // 400    //DEBUG_MAX_BUF_LEN/2   //640


//#define DEBUG_MAX_INFO_LEN 512

#endif                                                  
