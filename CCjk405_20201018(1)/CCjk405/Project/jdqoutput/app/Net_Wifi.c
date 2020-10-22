/**************************************************************************/
/***************************����WIFI�ײ�ӿ�*******************************/
/**************************************************************************/
#include "main.h"
#include "uart.h"
#include "wifi_RAK415.h"
#include "NetLay.h"
//#include "Task_TcpClient.h"

#define  WIFI_BAUD			115200 /* ��̨���ڲ����� */
#define  MAX_SEND_ONCE  (MAX_UART_BUF_LENTH - 100)

//#define WIFI_RESET_L   Gpio_WritiLoutputIo(OUT9,Bit_RESET)
//#define WIFI_RESET_H	 Gpio_WritiLoutputIo(OUT9,Bit_SET)

/* ��λWIFIоƬ */
void Reset_RAK415(void)
{
//	WIFI_RESET_L;
	RAK415_Reset_Cmd(BACK_COM);
//	WIFI_RESET_H;
}

/* ��λ4GоƬ */
void Reset_4G(void)
{

}

/* ��ʼ����̨���� */
static void Init_WifiCom(void)
{
	uint32 tmp;
	
	/* ��ȡ���� */
	UartHandler[BACK_COM] = Uart_Open(BACK_COM);
	
	/* ������ */
 	tmp  = WIFI_BAUD;
	Uart_IOCtrl(UartHandler[BACK_COM], SIO_BAUD_SET, &tmp);	

	/* ��У�� */
	tmp  = USART_Parity_No;
	Uart_IOCtrl(UartHandler[BACK_COM], SIO_PARITY_BIT_SET, &tmp);

	/* ֹͣλ */
	tmp  = USART_StopBits_1;
	Uart_IOCtrl(UartHandler[BACK_COM], SIO_STOP_BIT_SET, &tmp);	
	
	/* 9������λ */
	tmp = USART_WordLength_8b;
	Uart_IOCtrl(UartHandler[BACK_COM], SIO_DATA_BIT_SET, &tmp);	
}

/* WIFIģ���ʼ�� */
void Init_WifiModule(ETH_PARAM_T *Para)
{
//	Reset_RAK415();
	Init_WifiCom();
	RAK415_ParaConfig(BACK_COM,Para);
}

/* 4Gģ���ʼ�� */
void Init_4GModule(ETH_PARAM_T *Para)
{
	Init_WifiCom();
}
/* 2Gģ���ʼ�� */
void Init_2GModule(ETH_PARAM_T *Para)
{
	Init_WifiCom();
	GSM2_ParaConfig(BACK_COM,Para);
}

/* ���ڽ������� */
int32 Uart_ReadData(uint8 *buf, uint16 maxSize)
{
	return Uart_Read(UartHandler[BACK_COM],buf,maxSize);
}

/* ���ڷ������� */
int32 Uart_WriteData(uint8 *buf, uint16 len)
{
	  int32 i32Len = 0;
	  i32Len = Uart_Write(UartHandler[BACK_COM],buf,len);
//	uint16 wlen = 0;

//	while(len){ //512 �����㹻�����÷ֶη��ͣ��ӳ��Ƿֶη��ͽ������ǳ���
////		if(len > MAX_SEND_ONCE){
////			wlen += Uart_Write(UartHandler[BACK_COM], &buf[wlen], MAX_SEND_ONCE);
////			len = len - MAX_SEND_ONCE;
////			Delay10Ms(3);
////		}
////		else
//			{
//			wlen += Uart_Write(UartHandler[BACK_COM], &buf[wlen], len);
//			len = 0;
//		}
//	}
	return i32Len;
}

extern void Init_Lwip(ETH_PARAM_T *Para);
extern void Reset_Lwip(void);

/* ����ӿڳ�ʼ�� */
void Init_NetInterFunc(NET_DATA_INTER *pNet, uint8 Type)
{
	OS_CPU_SR  cpu_sr;
	
	OS_ENTER_CRITICAL(); 
	
	switch (Type)
	{
		case CONN_WIFI:
		{
			pNet->Init = Init_WifiModule;
			pNet->Reset = Reset_RAK415;
			pNet->WriteData = Uart_WriteData;
			pNet->ReadData = Uart_ReadData;
			break;
		}
		case CONN_4G:
		{
			pNet->Init = Init_4GModule;
			pNet->Reset = Reset_4G;
			pNet->WriteData = Uart_WriteData;
			pNet->ReadData = Uart_ReadData;
			break;
		}
		case CONN_2G:
		{
			pNet->Init = Init_2GModule;
			pNet->Reset = Reset_4G;
			pNet->WriteData = Uart_WriteData;
			pNet->ReadData = Uart_ReadData;
			break;
		}
#ifdef AN_ETH_TASK		
		case CONN_ETH:
		{
			pNet->Init = tcp_client_init;
			pNet->Reset = Reset_Lwip;
			pNet->WriteData = Tcp_Write;//Lwip_WriteData;
			pNet->ReadData = Tcp_Read;//Lwip_ReadData;
			break;
		}
#endif		
		default:
			pNet->Init = Init_WifiModule;
			pNet->Reset = Reset_RAK415;
			pNet->WriteData = Uart_WriteData;
			pNet->ReadData = Uart_ReadData;
			break;
	}
	
	OS_EXIT_CRITICAL();
}
