#ifndef __WIFI_RAK415_H
#define __WIFI_RAK415_H

#ifdef  __cplusplus
  extern "C" {
#endif
 #include "typedefs.h"
#include "chgdata.h"   
//#include "os_cpu.h"

/************** Socket�˿����ò��� ********************/
//typedef struct
//{
//    uint8  RemoteIP[4];             /* ������IP��ַ */
//    uint16 RemoteSocket;            /* �������˿� */
//    uint16 LocalSocket;             /* �����˿� */
//    
//}SOCKET_PARA_STR;

///************** STAģʽ�����ò��� ********************/
//typedef struct
//{
//    int8   Ssid[32];                /* ָ��·��AP��������, ASCII��洢���������ַ���������־ */
//    int8   Password[32];            /* ָ��·��AP��������, ASCII��洢���������ַ���������־ */
//    uint8  SecuEn;                  /* ָ��·��AP�Ƿ���ܣ�0=�����ܣ�1=���� */
//    uint8  DhcpEn;                  /* ��ȡIP��ַ��ʽ��0=��̬���ã�1=��̬��ȡ  */
//    uint8  LocalIP[4];              /* ����IP��ַ */
//    uint8  NetMask[4];              /* �������� */
//    uint8  NetGateAddr[4];          /* ���ص�ַ */
//    uint8  DnsSever1[4];            /* DNS��������ַ */
//    uint8  DnsSever2[4];            /* ����DNS��������ַ */  
//}STA_PARA_STR;

///************** WIFI�������ò��� ********************/
//typedef struct
//{
//    SOCKET_PARA_STR   SocketPara;   /* Socket���ò��� */
//    STA_PARA_STR      StaModePara;  /* STAģʽ���ò��� */  
//    
//}WIFI_NET_CONFIG_PARA;

//extern WIFI_NET_CONFIG_PARA  WifiNetConfigPara;        /* WIFI�������ò��� */

//ErrorStatus RAK415_OpenAssistCmdMode(uint8 ComNo);       /* ����WiFiģ�鸨������ģʽ */
ErrorStatus RAK415_SetPara(uint8 ComNo,ETH_PARAM_T *Para);         /* ����WiFiģ����� */
ErrorStatus RAK415_Reset_Cmd(uint8 ComNo);                    /* ���λWiFiģ�� */
ErrorStatus Check_RAK415_TCPStatus(uint8 ComNo);              /* ��ѯWiFiģ��TCP����״̬ */
ErrorStatus  RAK415_ParaConfig(uint8 ComNo,ETH_PARAM_T *Para);     /* ����WiFiģ�� */
ErrorStatus  GSM2_ParaConfig(uint8 ComNo,ETH_PARAM_T *Para);

ErrorStatus InEasyTxRx(uint8 ComNo);                           /* ����͸��ģʽ */

#ifdef __cplusplus
  }
#endif


#endif
