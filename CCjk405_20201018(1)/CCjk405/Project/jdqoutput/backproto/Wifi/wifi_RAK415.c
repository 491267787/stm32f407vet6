#include <stdio.h>
#include <string.h>
//#include "stm32f2xx.h"
#include "uart.h"
#include "common.h"
#include "ChgData.h"
#include "wifi_RAK415.h"


//WIFI_NET_CONFIG_PARA  WifiNetConfigPara;        /* WIFI�������ò��� */

/*****************************************************************
*�������ƣ�static uint16 BackLinkRead(uint8 *pbuf, uint16 MaxSize)
*����������uint8 *pbuf�����ջ��棬uint16 MaxSize�������ո���
*��������ֵ���������ݳ���
*����������WiFiģ�鴮����·���
*****************************************************************/
static uint16 BackLinkRead(uint8 ComNo,uint8 *pbuf, uint16 MaxSize)
{
	uint8  RcvDataFlag = 0;
    uint16 RcvLen = 0, TempLen = 0;
	uint32 Timers = 0;
	
 	Timers = GetSystemTick();
    do
    {
        TempLen = Uart_Read(UartHandler[ComNo], pbuf+RcvLen, MaxSize);
        if (TempLen > 0)
        {
            RcvDataFlag = 1;
            RcvLen += TempLen;
            if (RcvLen > MaxSize)
                break;
            Timers = GetSystemTick();
        }
        else if (RcvDataFlag)
        {
            if (GetSystemTick() - Timers > 2)
                break;
        }
        Delay5Ms(4);
    }while(GetSystemTick() - Timers < 100);

    return RcvLen;
}

/*****************************************************************
*�������ƣ�static ErrorStatus OpenAssistCmdMode(void)
*������������
*��������ֵ��SUCCESS or ERROR
*��������������WiFiģ��RAK415�ĸ�������ģʽ
*****************************************************************/
const uint8 conAsk[] = "+++";
const uint8 conReplyBuf[] = "U";
ErrorStatus RAK415_OpenAssistCmdMode(uint8 ComNo)
{
    uint8  AskBuf[4];
     uint8  ReplyBuf[2];
    uint8  RxBuf[20];
    uint8  i;
		
	  memcpy(AskBuf,conAsk,4);
	  memcpy(ReplyBuf,conReplyBuf,2);
    RAK415_Reset_Cmd(ComNo);
    for (i = 0; i < 3; i++)
    {
			 Uart_ClearReadBuf(UartHandler[ComNo]);
				
        Uart_Write(UartHandler[ComNo], AskBuf, 3);      /* ����+++�����ź� */
				Delay10Ms(30);
        if ( BackLinkRead(ComNo,RxBuf, 10) != 1 || RxBuf[0] != 'U')
            continue;
            
        Uart_Write(UartHandler[ComNo], ReplyBuf, 1);    /* ����ȷ���ź� */
				Delay10Ms(1);
        BackLinkRead(ComNo,RxBuf, sizeof(RxBuf));
        if (RxBuf[0] != 'O' || RxBuf[1] != 'K')
            continue;
        return SUCCESS;
    }
    return ERROR;
}
/*****************************************************************
*�������ƣ�static ErrorStatus RAK415_WritePara(uint8 *buf, uint16 len)
*����������uint8 *buf, uint16 len
*��������ֵ��SUCCESS or ERROR
*����������WiFiģ��RAK415����д
*****************************************************************/
extern int32 Uart_WriteData(uint8 *buf, uint16 len);
static ErrorStatus RAK415_WritePara(uint8 ComNo,uint8 *buf, uint16 len)
{
    uint8 i, RxBuf[10] = { 0 };
 //   uint16 wlen = 0;

    for (i = 0; i < 3; i++)                                 /* ������ɹ���ѭ��д���� */
    {
//			  while(len){ //���治�����ֶη���
//          if(len>60){
//						wlen += Uart_Write(UartHandler[ComNo], &buf[wlen], 60);
//						len = len - 60;
//					}
//					else{
//						Uart_Write(UartHandler[ComNo], &buf[wlen], len);
//						len = 0;
//					}
//				}
			  Uart_WriteData(buf, len);
				Delay10Ms(1);
        BackLinkRead(ComNo,RxBuf, sizeof(RxBuf));
        if (RxBuf[0] == 'O' && RxBuf[1] == 'K')             /* д�����ɹ� */
            break;
        else if (i == 2)
            return ERROR;
    }
    return SUCCESS;
}
/*****************************************************************
*�������ƣ�ErrorStatus RAK415_SetPara(ETH_PARAM_T *Para)
*����������WIFI_NET_CONFIG_PARA *Para
*��������ֵ��SUCCESS or ERROR
*��������������WiFiģ��RAK415�Ĳ���
*****************************************************************/
ErrorStatus RAK415_SetPara(uint8 ComNo,ETH_PARAM_T *Para)
{
    uint8 TxBuf[200] = {0};
    uint16 TxLen;
    
    /* д�̶����� */
    strcat((char *)TxBuf, "wifi_mode=STA&socket_multi_en=0&socketA_type=tcp&socketA_tcp_timeout=0");//91-21 =70
    TxLen = sprintf((char *)TxBuf, "at+write_config=%d,wifi_mode=STA&socket_multi_en=0&socketA_type=tcp&socketA_tcp_timeout=0\r\n", strlen((char *)TxBuf));    
    if (RAK415_WritePara(ComNo,TxBuf, TxLen) == ERROR)
        return ERROR;
        
    /* socket�˿����ò��� */
    TxLen = sprintf((char *)TxBuf, "socketA_destip=%d.%d.%d.%d&socketA_destport=%d&socketA_localport=%d",\
                              Para->RemoteIpAddr[0], Para->RemoteIpAddr[1], Para->RemoteIpAddr[2], Para->RemoteIpAddr[3],\
                              Para->RemotePort, Para->StaPara.LocalSocket);
    TxLen = sprintf((char *)TxBuf, "at+write_config=%d,socketA_destip=%d.%d.%d.%d&socketA_destport=%d&socketA_localport=%d\r\n", TxLen,\
                              Para->RemoteIpAddr[0], Para->RemoteIpAddr[1], Para->RemoteIpAddr[2], Para->RemoteIpAddr[3],\
                              Para->RemotePort, Para->StaPara.LocalSocket);
    if (RAK415_WritePara(ComNo,TxBuf, TxLen) == ERROR)
        return ERROR;
        
    /* д�û���������*/
    memset(TxBuf, 0, sizeof(TxBuf));
    strcat((char *)TxBuf, "sta_ssid=");
    strcat((char *)TxBuf, (const char *)Para->StaPara.Ssid);
    strcat((char *)TxBuf, "&");
    strcat((char *)TxBuf, "sta_psk=");
    strcat((char *)TxBuf, (const char *)Para->StaPara.Password);
    sprintf((char *)TxBuf, "at+write_config=%d,sta_ssid=", strlen((char *)TxBuf));
    strcat((char *)TxBuf, (const char *)Para->StaPara.Ssid);
    strcat((char *)TxBuf, "&");
    strcat((char *)TxBuf, "sta_psk=");
    strcat((char *)TxBuf, (const char *)Para->StaPara.Password);
    strcat((char *)TxBuf, "\r\n");
    TxLen = strlen((char *)TxBuf);
    if (RAK415_WritePara(ComNo,TxBuf, TxLen) == ERROR)
        return ERROR;
    
    /* ���ܷ�ʽ��IP��ַ��ȡ��ʽ */
    TxLen = sprintf((char *)TxBuf, "sta_secu_en=%d&sta_dhcp_en=%d", Para->StaPara.SecuEn, Para->StaPara.DhcpEn);
    TxLen = sprintf((char *)TxBuf, "at+write_config=%d,sta_secu_en=%d&sta_dhcp_en=%d\r\n", TxLen, Para->StaPara.SecuEn, Para->StaPara.DhcpEn);
    if (RAK415_WritePara(ComNo,TxBuf, TxLen) == ERROR)
        return ERROR;
    
    if (Para->StaPara.DhcpEn == 0)  /* IP��ַ��̬���� */
    {
        TxLen = sprintf((char *)TxBuf, "sta_ipaddr=%d.%d.%d.%d&sta_netmask=%d.%d.%d.%d&sta_gateway=%d.%d.%d.%d&sta_dnssever1=%d.%d.%d.%d&sta_dnssever2=%d.%d.%d.%d",\
                             Para->LocalIpAddr[0], Para->LocalIpAddr[1], Para->LocalIpAddr[2], Para->LocalIpAddr[3],\
                             Para->NetMask[0], Para->NetMask[1], Para->NetMask[2], Para->NetMask[3],\
                             Para->NetGate[0], Para->NetGate[1], Para->NetGate[2], Para->NetGate[3],\
                             Para->StaPara.DnsSever1[0], Para->StaPara.DnsSever1[1], Para->StaPara.DnsSever1[2], Para->StaPara.DnsSever1[3],\
                             Para->StaPara.DnsSever2[0], Para->StaPara.DnsSever2[1], Para->StaPara.DnsSever2[2], Para->StaPara.DnsSever2[3]);
        
        TxLen = sprintf((char *)TxBuf, "at+write_config=%d,sta_ipaddr=%d.%d.%d.%d&sta_netmask=%d.%d.%d.%d&sta_gateway=%d.%d.%d.%d&sta_dnssever1=%d.%d.%d.%d&sta_dnssever2=%d.%d.%d.%d\r\n", TxLen,\
                             Para->LocalIpAddr[0], Para->LocalIpAddr[1], Para->LocalIpAddr[2], Para->LocalIpAddr[3],\
                             Para->NetMask[0], Para->NetMask[1], Para->NetMask[2], Para->NetMask[3],\
                             Para->NetGate[0], Para->NetGate[1], Para->NetGate[2], Para->NetGate[3],\
                             Para->StaPara.DnsSever1[0], Para->StaPara.DnsSever1[1], Para->StaPara.DnsSever1[2], Para->StaPara.DnsSever1[3],\
                             Para->StaPara.DnsSever2[0], Para->StaPara.DnsSever2[1], Para->StaPara.DnsSever2[2], Para->StaPara.DnsSever2[3]);
        if (RAK415_WritePara(ComNo,TxBuf, TxLen) == ERROR)
            return ERROR;
    }
    return SUCCESS;
}
/*****************************************************************
*�������ƣ�ErrorStatus RAK415_Reset_Cmd(void)
*������������
*��������ֵ��SUCCESS or ERROR
*������������λWiFiģ��RAK415
*****************************************************************/
ErrorStatus RAK415_Reset_Cmd(uint8 ComNo)
{
    uint8 TxBuf[] = {"at+reset\r\n"};
    
    if (RAK415_WritePara(ComNo,TxBuf, strlen((char *)TxBuf)) == ERROR)
            return ERROR;
    Delay10Ms(100);
    return SUCCESS;
}
/*****************************************************************
*��������:�sstatic ErrorStatus RAK415_TCPStatusCmd(void)
*������������
*��������ֵ��SUCCESS or ERROR
*������������ѯWiFiģ��RAK415 TCP״̬����
*****************************************************************/
static ErrorStatus RAK415_TCPStatusCmd(uint8 ComNo)
{
    uint8 TxBuf[] = {"at+tcp_status=0\r\n"};
    uint8 RxBuf[10] = {0};
    uint8 i, j;
    
    for (i = 0; i < 3; i++)
    {
        j = 20;
        while(BackLinkRead(ComNo,RxBuf, sizeof(RxBuf)) > 0)                  /* ���մ��� */
        {
            j--;
            if (j == 0)
                return ERROR;
        }
        Uart_Write(UartHandler[ComNo], TxBuf, strlen((char *)TxBuf));
				Delay10Ms(1);
        BackLinkRead(ComNo,RxBuf, sizeof(RxBuf));
        if (RxBuf[0] == 'O' && RxBuf[1] == 'K')
        {
            if (RxBuf[2] == 0)
                return ERROR;
            else if (RxBuf[2] == 1)
                return SUCCESS;
        }
        memset(RxBuf, 0, sizeof(RxBuf));
    }
    return ERROR;
}
/*****************************************************************
*�������ƣ�ErrorStatus InEasyTxRx(void)
*������������
*��������ֵ��SUCCESS or ERROR
*���������C����͸��״̬ 
*****************************************************************/
ErrorStatus InEasyTxRx(uint8 ComNo)
{
    uint8 TxBuf[] = {"at+easy_txrx\r\n"};
    
    return (RAK415_WritePara(ComNo,TxBuf, strlen((char *)TxBuf)));
}
/*****************************************************************
*�������ƣ�ErrorStatus Check_RAK415_TCPStatus(void))
*������������
*��������ֵ��SUCCESS or ERROR
*���������C��ѯWiFiģ��RAK415 TCP����״̬
*****************************************************************/
ErrorStatus Check_RAK415_TCPStatus(uint8 ComNo)
{
    ErrorStatus Status;
    
    if (RAK415_OpenAssistCmdMode(ComNo) == ERROR)
        return ERROR;
    Status = RAK415_TCPStatusCmd(ComNo);  
    if (InEasyTxRx(ComNo) == ERROR)
        return ERROR;
    return Status;
}
/*****************************************************************
*�������ƣ�ErrorStatus  RAK415_ParaConfig(ETH_PARAM_T *Para)
*����������WIFI_NET_CONFIG_PARA *Para
*��������ֵ��SUCCESS or ERROR
*��������������WiFiģ��RAK415
*****************************************************************/
ErrorStatus  RAK415_ParaConfig(uint8 ComNo,ETH_PARAM_T *Para)
{
	
    if (RAK415_OpenAssistCmdMode(ComNo) == ERROR)  /* ������������ģʽ */
        return ERROR;
    if (RAK415_SetPara(ComNo,Para) == ERROR)        /* ���ò��� */
        return ERROR;
    if (RAK415_Reset_Cmd(ComNo) == ERROR)          /* ��λWiFiģ�� */
        return ERROR;
    return SUCCESS;
}


/*****************************************************************
*�������ƣ�2Gģ���ʼ��
*****************************************************************/
ErrorStatus GSM2_OpenAssistCmdMode(uint8 ComNo)
{
    uint8  AskBuf[] = {"+++\r\n"};
    uint8  RxBuf[100];
    uint8  i, j;
    
    for (i = 0; i < 2; i++)
    {
        j = 20;
        Uart_ClearReadBuf(UartHandler[ComNo]);
        Uart_Write(UartHandler[ComNo], AskBuf, 7);      /* ����+++�����ź� */
				OSTimeDly(31);
        BackLinkRead(ComNo,RxBuf, sizeof(RxBuf));
        if (RxBuf[0] != 'o' || RxBuf[1] != 'k'){
            continue;
				}
        return SUCCESS;
    }
    return ERROR;
}
static ErrorStatus GSM2_WritePara(uint8 ComNo,uint8 *buf, uint16 len)
{
	  uint8 rtn = ERROR;
    uint8 i, RxBuf[10] = { 0 };
    
    for (i = 0; i < 3; i++)                                 /* ������ɹ���ѭ��д���� */
    {
			  Uart_ClearReadBuf(UartHandler[ComNo]);
        Uart_Write(UartHandler[ComNo], buf, len);
			  OSTimeDly(20);
        BackLinkRead(ComNo,RxBuf, sizeof(RxBuf));
        if (RxBuf[0] == 'o' && RxBuf[1] == 'k') {            /* д�����ɹ� */
					  rtn = SUCCESS;
            break;
				}

    }
    return rtn;
}
ErrorStatus GSM2_SetPara(uint8 ComNo,ETH_PARAM_T *Para)
{
    uint8 TxBuf[200] = {0};
    uint16 TxLen;
          
    /* socket�˿����ò��� */
    TxLen = sprintf((char *)TxBuf, "socketA_destip=%d.%d.%d.%d&socketA_destport=%d",\
                              Para->RemoteIpAddr[0], Para->RemoteIpAddr[1], Para->RemoteIpAddr[2], Para->RemoteIpAddr[3],\
                              Para->RemotePort);
    TxLen = sprintf((char *)TxBuf, "at+write_config=%d,socketA_destip=%d.%d.%d.%d&socketA_destport=%d\r\n", TxLen,\
                              Para->RemoteIpAddr[0], Para->RemoteIpAddr[1], Para->RemoteIpAddr[2], Para->RemoteIpAddr[3],\
                              Para->RemotePort);
    if (GSM2_WritePara(ComNo,TxBuf, TxLen) == ERROR)
        return ERROR;
        
    return SUCCESS;
}
ErrorStatus  GSM2_ParaConfig(uint8 ComNo,ETH_PARAM_T *Para)
{
    if (GSM2_OpenAssistCmdMode(ComNo) == ERROR)  /* ������������ģʽ */
        return ERROR;
    if (GSM2_SetPara(ComNo,Para) == ERROR)        /* ���ò��� */
        return ERROR;
    return SUCCESS;
}



