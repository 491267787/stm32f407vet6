/**************************************************************************/
/***************************����PHY�ײ�ӿ�*******************************/
///**************************************************************************/
//#include "main.h"
//#include "netconf.H"
//#include "Tcp_client.h"
//#include "stm32f4x7_eth.h"
//#include "stm32f4x7_eth_bsp.h"
//#include "TcpClient.h"
//#include "Task_TcpClient.h"
//#include "EthNet.h"
#include "NetLay.h"
//#include "Task_TcpClient.h"

//extern void TClient_ResetActive(void);
///*tcp client �������ݽӿ�*/
//extern int32 TClient_Write(uint8 *buf,uint16 len);
///*tcp client �������ݽӿ�*/
//extern int32 TClient_Read(uint8 *buf,uint16 len);

//OS_STK Stk_Task_TCP_Client[TASK_TCP_CLIENT_STK_SIZE];
	extern void lwiptcp_Init(void);

///* ��ʼ�� */
void Init_Lwip(ETH_PARAM_T *Para)
{	
	lwiptcp_Init();

	return;
}

/* ��λ */
void Reset_Lwip(void)
{
	return;
}

///* �������� */
//int32 Lwip_WriteData(uint8 *buf,uint16 len)
//{
////	return TClient_Write(buf,len);
//}

///* �������� */
//int32 Lwip_ReadData(uint8 *buf,uint16 maxSize)
//{
////	return TClient_Read(buf,maxSize);
//}





