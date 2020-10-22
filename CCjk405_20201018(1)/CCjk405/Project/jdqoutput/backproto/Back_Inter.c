/**************************************************************************/
/***************************����WIFI�ײ�ӿ�*******************************/
/**************************************************************************/
#include "main.h"
#include "uart.h"
#include "Back_Inter.h"
#include "TaskBackComm.h"
#include "wifi_RAK415.h"

#define 	WIFI_BAUD			115200 /* ��̨���ڲ����� */

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
/* ���ڽ������� */
int32 Uart_ReadData(uint8 *buf, uint16 maxSize)
{
	return Uart_Read(UartHandler[BACK_COM],buf,maxSize);
}


#define MAX_SEND_ONCE  80
/* ���ڷ������� */
int32 Uart_WriteData(uint8 *buf, uint16 len)
{
	uint16 wlen = 0;

	while(len){ //���治�����ֶη���
		if(len > MAX_SEND_ONCE){
			wlen += Uart_Write(UartHandler[BACK_COM], &buf[wlen], MAX_SEND_ONCE);
			len = len - MAX_SEND_ONCE;
			Delay10Ms(3);
		}
		else{
			wlen += Uart_Write(UartHandler[BACK_COM], &buf[wlen], len);
			len = 0;
		}
	}
	return wlen;
}

/* ����ӿڳ�ʼ�� */
void Init_NetInterFunc(NET_DATA_INTER *pNet,uint8 Type)
{
	OS_CPU_SR  cpu_sr;
	OS_ENTER_CRITICAL();  
	switch(Type)
	{
//		case CONN_WIFI:
//		{
//			pNet->Init = Init_WifiModule;
//			pNet->Reset = Reset_RAK415;
//			pNet->WriteData = Uart_WriteData;
//			pNet->ReadData = Uart_ReadData;
//			break;
//		}
		case CONN_4G:
		{
			pNet->Init = Init_4GModule;
			pNet->Reset = Reset_4G;
			pNet->WriteData = Uart_WriteData;
			pNet->ReadData = Uart_ReadData;
			break;
		}
		case CONN_RS485:
		{
			pNet->Init = Init_WifiModule;
			pNet->Reset = Reset_RAK415;
			pNet->WriteData = Uart_WriteData;
			pNet->ReadData = Uart_ReadData;
			break;
		}
		default:
			pNet->Init = Init_WifiModule;
			pNet->Reset = Reset_RAK415;
			pNet->WriteData = Uart_WriteData;
			pNet->ReadData = Uart_ReadData;
			break;
	}
	OS_EXIT_CRITICAL();
}


