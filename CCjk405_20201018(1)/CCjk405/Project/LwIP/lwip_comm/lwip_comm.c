#include "lwip_comm.h" 
//#include "netif/etharp.h"
#include "lwip/dhcp.h"
#include "ethernetif.h" 
//#include "lwip/timers.h"
//#include "lwip/tcp_impl.h"
#include "lwip/ip4_frag.h"
#include "lwip/tcpip.h" 
//#include "malloc.h"
#include "delay.h" 
#include <stdio.h>
#include "ucos_ii.h" 
//#include "lwipopts.h" 
#include "chgdata.h"

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
   
  
__lwip_dev lwipdev;						//lwip���ƽṹ�� 
struct netif lwip_netif;				//����һ��ȫ�ֵ�����ӿ�

extern u32 memp_get_memorysize(void);	//��memp.c���涨��
extern u8_t *memp_memory;				//��memp.c���涨��.
extern u8_t *ram_heap;					//��mem.c���涨��.

//netif up
void an_netif_set_link_up()
{
	netif_set_link_up(&lwip_netif);
}


//netif down
void an_netif_set_link_down()
{
	netif_set_link_down(&lwip_netif);

}


/////////////////////////////////////////////////////////////////////////////////
//lwip����������(�ں������DHCP����)

//lwip�ں������ջ(���ȼ��Ͷ�ջ��С��lwipopts.h������) 

//lwip DHCP����
//�����������ȼ�
//#define LWIP_DHCP_TASK_PRIO       		7
//���������ջ��С
//efine LWIP_DHCP_STK_SIZE  		    156  // patli 20200115 128
//�����ջ�������ڴ����ķ�ʽ��������	
static OS_STK  LWIP_DHCP_TASK_STK[LWIP_DHCP_STK_SIZE];	
//������
void lwip_dhcp_task(void *pdata); 


//������̫���жϵ���
err_t lwip_pkt_handle(void)
{
	return ethernetif_input(&lwip_netif);
}



//lwip Ĭ��IP����
//lwipx:lwip���ƽṹ��ָ��
void lwip_comm_default_ip_set(__lwip_dev *lwipx)
{
	u32 sn0;
	sn0=*(vu32*)(0x1FFF7A10);//��ȡSTM32��ΨһID��ǰ24λ��ΪMAC��ַ�����ֽ�
	//Ĭ��Զ��IPΪ:192.168.1.115
	lwipx->remoteip[0]=192;	
	lwipx->remoteip[1]=168;
	lwipx->remoteip[2]=1;
	lwipx->remoteip[3]=102;
	//MAC��ַ����(�����ֽڹ̶�Ϊ:2.0.0,�����ֽ���STM32ΨһID)
	lwipx->mac[0]=2;//�����ֽ�(IEEE��֮Ϊ��֯ΨһID,OUI)��ַ�̶�Ϊ:2.0.0
	lwipx->mac[1]=0;
	lwipx->mac[2]=0;
	lwipx->mac[3]=(sn0>>16)&0XFF;//�����ֽ���STM32��ΨһID
	lwipx->mac[4]=(sn0>>8)&0XFFF;;
	lwipx->mac[5]=sn0&0XFF; 
	//Ĭ�ϱ���IPΪ:192.168.1.30
	lwipx->local_ip[0]=192;	
	lwipx->local_ip[1]=168;
	lwipx->local_ip[2]=1;
	lwipx->local_ip[3]=30;
	//Ĭ����������:255.255.255.0
	lwipx->netmask[0]=255;	
	lwipx->netmask[1]=255;
	lwipx->netmask[2]=255;
	lwipx->netmask[3]=0;
	//Ĭ������:192.168.1.1
	lwipx->gateway[0]=192;	
	lwipx->gateway[1]=168;
	lwipx->gateway[2]=1;
	lwipx->gateway[3]=1;	
	lwipx->dhcpstatus=0;//û��DHCP	
} 

//LWIP��ʼ��(LWIP������ʱ��ʹ��)
//����ֵ:0,�ɹ�
//      1,�ڴ����
//      2,LAN8720��ʼ��ʧ��
//      3,�������ʧ��.
u8 lwip_comm_init(uint8 flag)
{
	OS_CPU_SR cpu_sr;
	struct netif *Netif_Init_Flag;		//����netif_add()����ʱ�ķ���ֵ,�����ж������ʼ���Ƿ�ɹ�
	ip_addr_t ipaddr;  			//ip��ַ
	ip_addr_t netmask; 			//��������
	ip_addr_t gw;      			//Ĭ������ 
	
	if(LAN8720_Init())
	{
		
//		printf("lwip_comm_init,LAN8720_Init fail�\r\n");	
		return 2;			//��ʼ��LAN8720ʧ��
	}
	

	tcpip_init(NULL,NULL);				//��ʼ��tcp ip�ں�,�ú�������ᴴ��tcpip_thread�ں�����
	lwip_comm_default_ip_set(&lwipdev);	//����Ĭ��IP����Ϣ
	
	/*��ȡMac��ַ�����ڽ�����ʾ*/
	PARAM_COMM_TYPE *ptrNetPara = ChgData_GetCommParaPtr();  
	ptrNetPara->netpara.MacAdress[0] = lwipdev.mac[0];
	ptrNetPara->netpara.MacAdress[1] = lwipdev.mac[1];
	ptrNetPara->netpara.MacAdress[2] = lwipdev.mac[2];
	ptrNetPara->netpara.MacAdress[3] = lwipdev.mac[3];
	ptrNetPara->netpara.MacAdress[4] = lwipdev.mac[4];
	ptrNetPara->netpara.MacAdress[5] = lwipdev.mac[5];
	
#if LWIP_DHCP		//ʹ�ö�̬IP
  if(1 == ptrNetPara->netpara.StaPara.DhcpEn){
		ipaddr.addr = 0;
		netmask.addr = 0;
		gw.addr = 0;
	}
	else{
		memcpy(lwipdev.local_ip,ptrNetPara->netpara.LocalIpAddr,sizeof(lwipdev.local_ip));
		memcpy(lwipdev.netmask,ptrNetPara->netpara.NetMask,sizeof(lwipdev.netmask));
		memcpy(lwipdev.gateway,ptrNetPara->netpara.NetGate,sizeof(lwipdev.gateway));
		IP4_ADDR(&ipaddr,lwipdev.local_ip[0],lwipdev.local_ip[1],lwipdev.local_ip[2],lwipdev.local_ip[3]);
	  IP4_ADDR(&netmask,lwipdev.netmask[0],lwipdev.netmask[1] ,lwipdev.netmask[2],lwipdev.netmask[3]);
	  IP4_ADDR(&gw,lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3]);
	}
#else				//ʹ�þ�̬IP
	
	IP4_ADDR(&ipaddr,lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);
	IP4_ADDR(&netmask,lwipdev.netmask[0],lwipdev.netmask[1] ,lwipdev.netmask[2],lwipdev.netmask[3]);
	IP4_ADDR(&gw,lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3]);
//	printf("����en��MAC��ַΪ:................%d.%d.%d.%d.%d.%d\r\n",lwipdev.mac[0],lwipdev.mac[1],lwipdev.mac[2],lwipdev.mac[3],lwipdev.mac[4],lwipdev.mac[5]);
//	printf("��̬IP��ַ........................%d.%d.%d.%d\r\n",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);
//	printf("��������..........................%d.%d.%d.%d\r\n",lwipdev.netmask[0],lwipdev.netmask[1],lwipdev.netmask[2],lwipdev.netmask[3]);
//	printf("Ĭ������..........................%d.%d.%d.%d\r\n",lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3]);
#endif
	OS_ENTER_CRITICAL();  //�����ٽ���
	Netif_Init_Flag=netif_add(&lwip_netif,&ipaddr,&netmask,&gw,NULL,&ethernetif_init,&tcpip_input);//�������б������һ������
	OS_EXIT_CRITICAL();  //�˳��ٽ���
	if(Netif_Init_Flag==NULL)return 3;//�������ʧ�� 
	else//������ӳɹ���,����netifΪĬ��ֵ,���Ҵ�netif����
	{
		netif_set_default(&lwip_netif); //����netifΪĬ������
		netif_set_up(&lwip_netif);		//��netif����
	}
	return 0;//����OK.
}   
//���ʹ����DHCP
#if LWIP_DHCP
//����DHCP����
void lwip_comm_dhcp_creat(void)
{
	OS_CPU_SR cpu_sr;
	OS_ENTER_CRITICAL();  //�����ٽ���
#if(OS_TASK_STAT_STK_CHK_EN)
	OSTaskCreateExt(lwip_dhcp_task,(void*)0,(OS_STK*)&LWIP_DHCP_TASK_STK[LWIP_DHCP_STK_SIZE-1],LWIP_DHCP_TASK_PRIO, LWIP_DHCP_TASK_PRIO, LWIP_DHCP_TASK_STK, LWIP_DHCP_STK_SIZE, NULL, OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);//����DHCP���� 

#else	
	OSTaskCreate(lwip_dhcp_task,(void*)0,(OS_STK*)&LWIP_DHCP_TASK_STK[LWIP_DHCP_STK_SIZE-1],LWIP_DHCP_TASK_PRIO);//����DHCP���� 
#endif
	OS_EXIT_CRITICAL();  //�˳��ٽ���
}
//ɾ��DHCP����
void lwip_comm_dhcp_delete(void)
{
//patli 20191227	dhcp_stop(&lwip_netif); 		//�ر�DHCP
	OSTaskDel(LWIP_DHCP_TASK_PRIO);	//ɾ��DHCP����
}

void lwipHandleMsg(MSG_STRUCT *msg)
{
	
	switch(msg->MsgType)
	{
		case MSG_RESET_ETH:  /*�豸�����ݸ�TCP CLIENT, ������̨ */
		{	
//			printf("MSG_RESET_ETH,�����������硣����\r\n");   
#if 1
			lwipdev.dhcpstatus = 0;
				
	//		dhcp_stop(&lwip_netif); 		//�ر�DHCP		
			
	//		dhcp_cleanup(&lwip_netif);
	//		netif_remove(&lwip_netif);
			
	//		Delay10Ms(100);

#if 0			
			OSTaskDel(TCPIP_THREAD_PRIO); //ɾ��DHCP����

			ETH_Mem_Free();

			lwip_comm_mem_free1();
			
	//		mymem_init(SRAMIN); 
			Tcp_DataInit();
			
			while(lwip_comm_init(1)) 	
			{
				Delay10Ms(200);
			}	
#endif			
			
		//	printf("MSG_RESET_ETH,dhcp_start������\r\n");   
			
	//		dhcp_start(&lwip_netif);//����DHCP 
#else
	       	SoftReset();
#endif
			
			break;
		}
		default:
			break;
	}
}


//DHCP��������
void lwip_dhcp_task(void *pdata)
{
	PARAM_COMM_TYPE *ptrNetPara = ChgData_GetCommParaPtr();  
	MSG_STRUCT msg;
	u32 ip=0,netmask=0,gw=0, disp_num = 0;
	dhcp_start(&lwip_netif);//����DHCP 
	lwipdev.dhcpstatus=0;	//����DHCP
	
//	printf("���ڲ���DHCP������,���Ե�...........\r\n");  	
		
	Message_QueueCreat(LWIP_DHCP_TASK_PRIO); 

	while(1)
	{ 
		TaskRunTimePrint("lwip_dhcp_task Begin", OSPrioCur);
	
		if (RcvMsgFromQueue(&msg) == TRUE)
		{
			lwipHandleMsg(&msg);		 //������Ϣ
		}

		
//		printf("���ڻ�ȡ��ַ...\r\n");
		ip=lwip_netif.ip_addr.addr;		//��ȡ��IP��ַ
		netmask=lwip_netif.netmask.addr;//��ȡ��������
		gw=lwip_netif.gw.addr;			//��ȡĬ������
		
		disp_num++;
		
		if(ip!=0)   					//����ȷ��ȡ��IP��ַ��ʱ��
		{
			if(disp_num > 10)
			{
//	 			printf("����en��MAC��ַΪ:................%d.%d.%d.%d.%d.%d\r\n",lwipdev.mac[0],lwipdev.mac[1],lwipdev.mac[2],lwipdev.mac[3],lwipdev.mac[4],lwipdev.mac[5]);
//				printf("ͨ��DHCP��ȡ��IP��ַ..............%d.%d.%d.%d\r\n",ip&0xff,(ip>>8)&0xff,(ip>>16)&0xff,(ip>>24)&0xff);
//				printf("ͨ��DHCP��ȡ����������............%d.%d.%d.%d\r\n",netmask&0xff,(netmask>>8)&0xff,(netmask>>16)&0xff,(netmask>>24)&0xff);
//				printf("ͨ��DHCP��ȡ����Ĭ������..........%d.%d.%d.%d\r\n",gw&0xff,(gw>>8)&0xff,(gw>>16)&0xff,(gw>>24)&0xff);
			}

			if(lwipdev.dhcpstatus != 2)
			{
				struct dhcp * dhcp = netif_dhcp_data(&lwip_netif);
				
				//������ͨ��DHCP��ȡ����IP��ַ
				lwipdev.local_ip[3]=(uint8_t)(ip>>24); 
				lwipdev.local_ip[2]=(uint8_t)(ip>>16);
				lwipdev.local_ip[1]=(uint8_t)(ip>>8);
				lwipdev.local_ip[0]=(uint8_t)(ip);

				//����ͨ��DHCP��ȡ�������������ַ
				lwipdev.netmask[3]=(uint8_t)(netmask>>24);
				lwipdev.netmask[2]=(uint8_t)(netmask>>16);
				lwipdev.netmask[1]=(uint8_t)(netmask>>8);
				lwipdev.netmask[0]=(uint8_t)(netmask);

				//������ͨ��DHCP��ȡ����Ĭ������
				lwipdev.gateway[3]=(uint8_t)(gw>>24);
				lwipdev.gateway[2]=(uint8_t)(gw>>16);
				lwipdev.gateway[1]=(uint8_t)(gw>>8);
				lwipdev.gateway[0]=(uint8_t)(gw);

				memcpy(ptrNetPara->netpara.LocalIpAddr,lwipdev.local_ip,4);
				memcpy(ptrNetPara->netpara.NetGate,lwipdev.gateway,4);
				memcpy(ptrNetPara->netpara.NetMask,lwipdev.netmask,4);					
				
			
				netif_set_addr(&lwip_netif, &(dhcp->offered_ip_addr),&(dhcp->offered_sn_mask), &(dhcp->offered_gw_addr));
				netif_set_default(&lwip_netif);
				netif_set_up(&lwip_netif);
				
				lwipdev.dhcpstatus = 2;
			}
			
			
			Delay10Ms(50); //��ʱ250ms
//patli 20191228			break;
		}
	else{
			
			struct dhcp * dhcp = netif_dhcp_data(&lwip_netif);
			lwipdev.dhcpstatus=1;//1,����DHCP��ȡ״̬.
//			printf("DHCP�������DHCP��ȡ״̬!\r\n");

			if(dhcp->tries > LWIP_MAX_DHCP_TRIES)
			{
				lwipdev.dhcpstatus=0XFF;//DHCPʧ��.
//				printf("DHCP����ʱ,ʹ�þ�̬IP��ַ!\r\n");
//				printf("����en��MAC��ַΪ:................%d.%d.%d.%d.%d.%d\r\n",lwipdev.mac[0],lwipdev.mac[1],lwipdev.mac[2],lwipdev.mac[3],lwipdev.mac[4],lwipdev.mac[5]);
//				printf("��̬IP��ַ........................%d.%d.%d.%d\r\n",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);
//				printf("��������..........................%d.%d.%d.%d\r\n",lwipdev.netmask[0],lwipdev.netmask[1],lwipdev.netmask[2],lwipdev.netmask[3]);
//				printf("Ĭ������..........................%d.%d.%d.%d\r\n",lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3]);
			
			}
		}

		 Delay10Ms(LWIP_DHCP_TASK_DELAY); //��ʱ2.5s
			
			//��ӡͳ����Ϣ
			
		if(disp_num > 10)
		{
			stats_display();
			disp_num = 0;
		}

		
		TaskRunTimePrint("lwip_dhcp_task end", OSPrioCur);
	}
	
//patli 20200102	lwip_comm_dhcp_delete();	//ɾ��DHCP���� 
}
#endif 





























