#include "tcp_client_demo.h"
#include "dp83848.h"
#include "lwip_comm.h"

#include "sys.h"
#include "delay.h"
#include "usart.h"


#include "gpio.h"
#include "timer.h"
#include "Debug.h"


#include "lwip/api.h"

//////////////////////////////////////////////////////////////////////////////////	 
//���ܣ����Թ���api  
//���ߣ�
//��������:2020/05/09
//�汾��V1.0								  
////////////////////////////////////////////////////////////////////////////////// 

char DEBUG_Buff[50] = {0}; 

static uint32_t gDebugHandler;


#ifdef SCOMM_DEBUG
#define DEBUG_COM    COM1   //���Դ�ӡ��
#define DEBUG_COM_BAUD		        115200
void DebugScomm_init(void)
{
	int32_t tmp;
	UartHandler[DEBUG_COM] = Uart_Open(DEBUG_COM);
	gDebugHandler = UartHandler[DEBUG_COM];
	
 	tmp  = DEBUG_COM_BAUD;
	Uart_IOCtrl(UartHandler[DEBUG_COM], SIO_BAUD_SET, &tmp);	

	tmp  = USART_Parity_No;
	Uart_IOCtrl(UartHandler[DEBUG_COM], SIO_PARITY_BIT_SET, &tmp);

	tmp  = USART_StopBits_1;
	Uart_IOCtrl(UartHandler[DEBUG_COM], SIO_STOP_BIT_SET, &tmp);	
	
	tmp = USART_WordLength_8b;
	Uart_IOCtrl(UartHandler[DEBUG_COM], SIO_DATA_BIT_SET, &tmp);	
}
#endif



void DEBUG_Printf(char *s)
{
#ifdef SCOMM_DEBUG	
	Uart_Write(gDebugHandler, (uint8_t *)s, strlen(s));
#endif
	
}


#define DEBUG_TCP_CLIENT_RX_BUFSIZE	2000	//���ջ���������
char *debug_tcp_client_sendbuf="DEBUG NETCONN TCP Client send data\r\n";	//TCP�ͻ��˷������ݻ�����
u8 debug_tcp_client_recvbuf[DEBUG_TCP_CLIENT_RX_BUFSIZE];	//TCP�ͻ��˽������ݻ�����
u8 debug_tcp_client_flag;		//TCP�ͻ������ݷ��ͱ�־λ


#define DEBUG_REMOTE_PORT  8001
const uint8_t DEBUG_REMOTE_ADDR[4] = {192,168,0,102};
struct netconn *debugtcp_clientconn;					//TCP CLIENT�������ӽṹ��
static void debug_tcp_client_thread(void)
{
	OS_ERR err;
	//-----------------test lwip
//	while(lwip_comm_init()) 		//lwip��ʼ��
//	{
//		DEBUG_Printf("Lwip Init failed!\r\n"); 	//lwip��ʼ��ʧ��
//		OSTimeDlyHMSM(0,0,2,0,OS_OPT_TIME_HMSM_STRICT,&err);  
//	}
//	
//	while(tcp_client_init()) 									//��ʼ��tcp_client(����tcp_client�߳�)
//	{
//		DEBUG_Printf("TCP Client failed!!\r\n"); 		//tcp����ʧ��
//		OSTimeDlyHMSM(0,0,2,0,OS_OPT_TIME_HMSM_STRICT,&err); 
//	}
	
//#if LWIP_DHCP
//	lwip_comm_dhcp_creat(); //����DHCP����
//#endif
//-----------------	
	
	CPU_SR_ALLOC();
	
	u32 data_len = 0;
	struct pbuf *q;
	err_t recv_err;
	static ip_addr_t server_ipaddr,loca_ipaddr;
	static u16_t 		 server_port,loca_port;
	
	while(1)
	{
		if(1 == DP83848_GetPHYlinkStatus())
	  {
		  if(2 != lwipdev.dhcpstatus)	//DHCP�ɹ�
			{
				OSTimeDlyHMSM(0,0,2,0,OS_OPT_TIME_HMSM_STRICT,&err); 
			}
			else
			{
				break;
			}
		}
		OSTimeDlyHMSM(0,0,2,0,OS_OPT_TIME_HMSM_STRICT,&err); 
	}

	

//	LWIP_UNUSED_ARG(arg);
	server_port = DEBUG_REMOTE_PORT;
	IP4_ADDR(&server_ipaddr, DEBUG_REMOTE_ADDR[0],DEBUG_REMOTE_ADDR[1], DEBUG_REMOTE_ADDR[2],DEBUG_REMOTE_ADDR[3]);
	
	while (1) 
	{
		debugtcp_clientconn = netconn_new(NETCONN_TCP);  //����һ��TCP����
		err = netconn_connect(debugtcp_clientconn,&server_ipaddr,server_port);//���ӷ�����
		if(err != ERR_OK)  netconn_delete(debugtcp_clientconn); //����ֵ������ERR_OK,ɾ��tcp_clientconn����
		else if (err == ERR_OK)    //���������ӵ�����
		{ 
			struct netbuf *recvbuf;
			debugtcp_clientconn->recv_timeout = 10;
			netconn_getaddr(debugtcp_clientconn,&loca_ipaddr,&loca_port,1); //��ȡ����IP����IP��ַ�Ͷ˿ں�
			printf("�����Ϸ�����%d.%d.%d.%d,�����˿ں�Ϊ:%d\r\n",DEBUG_REMOTE_ADDR[0],DEBUG_REMOTE_ADDR[1], DEBUG_REMOTE_ADDR[2],DEBUG_REMOTE_ADDR[3],loca_port);
			while(1)
			{
				if((debug_tcp_client_flag & LWIP_SEND_DATA) == LWIP_SEND_DATA) //������Ҫ����
				{
					err = netconn_write(debugtcp_clientconn ,debug_tcp_client_sendbuf,strlen((char*)debug_tcp_client_sendbuf),NETCONN_COPY); //����tcp_server_sentbuf�е�����
					if(err != ERR_OK)
					{
						printf("����ʧ��\r\n");
					}
					debug_tcp_client_flag &= ~LWIP_SEND_DATA;
				}
					
				if((recv_err = netconn_recv(debugtcp_clientconn,&recvbuf)) == ERR_OK)  //���յ�����
				{	
					OS_CRITICAL_ENTER(); //���ж�
					memset(debug_tcp_client_recvbuf,0,DEBUG_TCP_CLIENT_RX_BUFSIZE);  //���ݽ��ջ���������
					for(q=recvbuf->p;q!=NULL;q=q->next)  //����������pbuf����
					{
						//�ж�Ҫ������TCP_CLIENT_RX_BUFSIZE�е������Ƿ����TCP_CLIENT_RX_BUFSIZE��ʣ��ռ䣬�������
						//�Ļ���ֻ����TCP_CLIENT_RX_BUFSIZE��ʣ�೤�ȵ����ݣ�����Ļ��Ϳ������е�����
						if(q->len > (DEBUG_TCP_CLIENT_RX_BUFSIZE-data_len)) memcpy(debug_tcp_client_recvbuf+data_len,q->payload,(DEBUG_TCP_CLIENT_RX_BUFSIZE-data_len));//��������
						else memcpy(debug_tcp_client_recvbuf+data_len,q->payload,q->len);
						data_len += q->len;  	
						if(data_len > DEBUG_TCP_CLIENT_RX_BUFSIZE) break; //����TCP�ͻ��˽�������,����	
					}
					OS_CRITICAL_EXIT();  //���ж�
					data_len=0;  //������ɺ�data_lenҪ���㡣					
					printf("%s\r\n",debug_tcp_client_recvbuf);
					netbuf_delete(recvbuf);
				}else if(recv_err == ERR_CLSD)  //�ر�����
				{
					netconn_close(debugtcp_clientconn);
					netconn_delete(debugtcp_clientconn);
					printf("������%d.%d.%d.%d�Ͽ�����\r\n",DEBUG_REMOTE_ADDR[0],DEBUG_REMOTE_ADDR[1], DEBUG_REMOTE_ADDR[2],DEBUG_REMOTE_ADDR[3]);
					break;
				}
			}
		}
	}

	
	
	
	
	
	
	
	
	
	
	
	
//	while(1)
//	{
//	
//	  if(1 == DP83848_GetPHYlinkStatus())
//	  {
//		  if(2 != lwipdev.dhcpstatus)	//DHCP�ɹ�
//			{
//				OSTimeDlyHMSM(0,0,2,0,OS_OPT_TIME_HMSM_STRICT,&err); 
//			}
//			else
//			{
//				
//			}
//	  }
//		OSTimeDlyHMSM(0,0,2,0,OS_OPT_TIME_HMSM_STRICT,&err);
//		
//	}
	
	
}



//������ƿ�
OS_TCB	DEBUG_TcpClientTaskTCB;
//�����ջ
CPU_STK DEBUG_TCPCLIENT_TASK_STK[DEBUG_TCPCLIENT_STK_SIZE];

//��������TCP�ͻ����߳�
//����ֵ:0 TCP�ͻ��˴����ɹ�
//		���� TCP�ͻ��˴���ʧ��
u8 DEBUG_TCP_Client_Init(void)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	
	OS_CRITICAL_ENTER();//�����ٽ���
	//����TCP�ͻ�������
	OSTaskCreate((OS_TCB 	* )&DEBUG_TcpClientTaskTCB,		
				 (CPU_CHAR	* )"debug_tcp_client task", 		
                 (OS_TASK_PTR )debug_tcp_client_thread, 			
                 (void		* )0,					
                 (OS_PRIO	  )DEBUG_TCPCLIENT_PRIO,     
                 (CPU_STK   * )&DEBUG_TCPCLIENT_TASK_STK[0],	
                 (CPU_STK_SIZE)DEBUG_TCPCLIENT_STK_SIZE/10,	
                 (CPU_STK_SIZE)DEBUG_TCPCLIENT_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);
	OS_CRITICAL_EXIT();	//�˳��ٽ���
	return err;
}


void DebugETH_init(void)
{
  OS_ERR err;
  while(DEBUG_TCP_Client_Init()) 									//��ʼ��tcp_client(����tcp_client�߳�)
	{
		DEBUG_Printf("DEBUG TCP Client failed!!\r\n"); 		//tcp����ʧ��
		OSTimeDlyHMSM(0,0,2,0,OS_OPT_TIME_HMSM_STRICT,&err); 
	}	
	
}




void DEBUG_Init(void)
{
	
#ifdef SCOMM_DEBUG
  DebugScomm_init();
#endif
	
#ifdef ETH_DEBUG
	DebugETH_init();
#endif

}

