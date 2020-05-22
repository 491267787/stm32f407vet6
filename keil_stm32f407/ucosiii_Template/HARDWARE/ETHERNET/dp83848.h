#ifndef __DP83848_H
#define __DP83848_H
#include "sys.h"
#include "stm32f4x7_eth.h"
//////////////////////////////////////////////////////////////////////////////////	 
//DP83848 ��������	   							  
////////////////////////////////////////////////////////////////////////////////// 

/* Ethernet Flags for EthStatus variable */ 
#define ETH_LINK_FLAG           0x10 /* Ethernet Link Flag */

#define DP83848_PHY_ADDRESS  	1//0x00				//DP83848 PHYоƬ��ַ.
//#define DP83848_RST 		   	PDout(3) 			//LAN8720��λ����	 

extern ETH_DMADESCTypeDef *DMARxDscrTab;			//��̫��DMA�������������ݽṹ��ָ��
extern ETH_DMADESCTypeDef *DMATxDscrTab;			//��̫��DMA�������������ݽṹ��ָ�� 
extern uint8_t *Rx_Buff; 							//��̫���ײ���������buffersָ�� 
extern uint8_t *Tx_Buff; 							//��̫���ײ���������buffersָ��
extern ETH_DMADESCTypeDef  *DMATxDescToSet;			//DMA����������׷��ָ��
extern ETH_DMADESCTypeDef  *DMARxDescToGet; 		//DMA����������׷��ָ�� 
extern ETH_DMA_Rx_Frame_infos *DMA_RX_FRAME_infos;	//DMA�����յ���֡��Ϣָ��
 

u8 DP83848_Init(void);
u8 DP83848_Get_Speed(void);
u8 DP83848_GetPHYlinkStatus(void);
u8 ETH_MACDMA_Config(void);
FrameTypeDef ETH_Rx_Packet(void);
u8 ETH_Tx_Packet(u16 FrameLength);
u32 ETH_GetCurrentTxBuffer(void);
u8 ETH_Mem_Malloc(void);
void ETH_Mem_Free(void);


#endif 

