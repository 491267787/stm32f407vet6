#ifndef _LWIP_COMM_H
#define _LWIP_COMM_H 
#include "typedefs.h"
#include "sys.h"
#include "stm32f4x7_eth.h"

//#include "lan8720.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//lwipͨ������ ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/8/15
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//*******************************************************************************
//�޸���Ϣ
//��
////////////////////////////////////////////////////////////////////////////////// 	   
 

#define 	LWIP_MAX_DHCP_TRIES		20   //DHCP������������Դ���
#define   	LWIP_DHCP	1

//lwip���ƽṹ��
typedef struct  
{
	u8 mac[6];      	//MAC��ַ
	u8 remoteip[4];		//Զ������IP��ַ 
	u16 remote_port;	//Զ�̶˿�
	u8 local_ip[4];     //����IP��ַ
	u16 local_port;		//���ض˿�
	u8 netmask[4]; 		//��������
	u8 gateway[4]; 		//Ĭ�����ص�IP��ַ
	
	vu8 dhcpstatus;	//dhcp״̬ 
					//0,δ��ȡDHCP��ַ;
					//1,����DHCP��ȡ״̬
					//2,�ɹ���ȡDHCP��ַ
					//0XFF,��ȡʧ��.
}__lwip_dev;
extern __lwip_dev lwipdev;	//lwip���ƽṹ��
 
s8 lwip_pkt_handle(void);
void lwip_comm_default_ip_set(__lwip_dev *lwipx);
u8 lwip_comm_mem_malloc(void);
void lwip_comm_mem_free(void);
u8 lwip_comm_init(uint8 flag);
void lwip_comm_dhcp_creat(void);
void lwip_comm_dhcp_delete(void);
void lwip_comm_destroy(void);
void lwip_comm_delete_next_timeout(void);

//netif up
//void an_netif_set_link_up();


//netif down
void an_netif_set_link_down();

#endif













