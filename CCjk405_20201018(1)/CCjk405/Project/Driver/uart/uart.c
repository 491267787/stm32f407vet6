/****************************************Copyright (c)**************************************************
 Aunice
 
****************************************************************************************************/
#include "Uart.h"
//#include "Gps.h"

#if defined (STM32F10X_CL)
#include "stm32f10x_rcc.h"
#endif
						  
#include "..\queue\queue.h"

#if defined (STM32F4XX)
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#endif
#include "Adebug.h"

extern void GpsSetMsValue(int val);
extern void GpstimeFlash(void);



#define _IS_USER_FIFO_

uint8 PowerOnForUart = FALSE;	
        
uint32 UartHandler[] = {0,0,0,0,0,0};

USART_InitTypeDef USART_InitStructure;

static STM32F10x_SIO_CHAN_PARAM UARTParameter[] = {	     //ע��:����2-3 ��ԭ��ͼ/PCB  ����� 
		{(void *)USART1_BASE, (void *)GPIOA, GPIO_Pin_3,  RS232TYPE, 115200,   USART_WordLength_8b, USART_StopBits_1, USART_Parity_No, HFC_NONE,UART_USE_TYPE_NOMAL},	
		{(void *)USART2_BASE, (void *)GPIOA, GPIO_Pin_4,  RS232TYPE, 115200,     USART_WordLength_8b, USART_StopBits_1, USART_Parity_No, HFC_NONE,UART_USE_TYPE_NOMAL},
		{(void *)USART3_BASE, (void *)GPIOD, GPIO_Pin_10, RS485TYPE, 9600,     USART_WordLength_8b, USART_StopBits_1, USART_Parity_No, HFC_NONE,UART_USE_TYPE_NOMAL},	
		{(void *)UART4_BASE,  (void *)GPIOG, GPIO_Pin_4,  RS232TYPE, 115200,   USART_WordLength_8b, USART_StopBits_1, USART_Parity_No, HFC_NONE,UART_USE_TYPE_NOMAL},
		{(void *)UART5_BASE,  (void *)GPIOB, GPIO_Pin_15,  RS485TYPE, 115200,   USART_WordLength_8b, USART_StopBits_1, USART_Parity_No, HFC_NONE,UART_USE_TYPE_NOMAL},
		{(void *)USART6_BASE, (void *)GPIOC, GPIO_Pin_8,  RS485TYPE, 2400,     USART_WordLength_8b, USART_StopBits_1, USART_Parity_No, HFC_NONE,UART_USE_TYPE_NOMAL},
		};

STM32F10x_UART_CHAN UARTChan[N_UART_CHANNELS];
STM32F10x_UART_CHAN *GetChanHandler(uint8 id)
{
		if(id < 5)
			return  &UARTChan[id] ;
	return NULL;
}

// *************************************************
// Uart_EnIsr - desable a ISR .	
// RETURN: N
// *************************************************
static void Uart_EnableIsr(STM32F10x_UART_CHAN* pChan)
{	
	USART_Cmd((USART_TypeDef*)pChan->base, ENABLE);	
}

// ***********************************************************************
// �����豸�򿪺��� 
// ����:  deviceId	:����ID   ȡֵ��Χ�� 1 ~  N_UART_CHANNELS
// ����ֵ�� FALSE - ��ʧ��, ���������ھ�� 
// *************************************************************************
int32 Uart_Open(uint32 deviceId)
{
	uint8 err;

	if(deviceId <= N_UART_CHANNELS)
	{
		OSSemPend(UARTChan[deviceId].uartLock, 0, &err);   // ��ȡ�ź��� 
			
		if(UARTChan[deviceId].deviceOpenFlag == FALSE)	   // δ��״̬ 
		{						
			UARTChan[deviceId].deviceOpenFlag = TRUE;  	   // ���Ϊ�Ѵ�
			Uart_EnableIsr(&UARTChan[deviceId]);   
			OSSemPost(UARTChan[deviceId].uartLock);			
			return (int32)&UARTChan[deviceId];
		}else {
			OSSemPost(UARTChan[deviceId].uartLock);	           // �ͷ��ź��� 		
			return (int32)&UARTChan[deviceId];
		}		
	}	
	return FALSE;
}
// ***********************************************************************
// ���ڲ��������ò���  
// ����:  pChan - ���ھ����pbase - �Ĵ�����������ַ   
// ����ֵ�� FALSE - ��ʧ��, ���������ھ�� 
// *************************************************************************

BOOL Uart_BaudSet(STM32F10x_UART_CHAN* pChan,  USART_TypeDef* pbase)
{
	uint32 apbclock = 0, tmpreg = 0, integerdivider=0, fractionaldivider=0;
	RCC_ClocksTypeDef  RCC_ClocksStatus;
#if defined (STM32F10X_CL) 
	assert_param(IS_USART_BAUDRATE(bps));  	                    // �������� 
	RCC_GetClocksFreq(&RCC_ClocksStatus);
	if (pbase == (USART_TypeDef*)USART1_BASE)
	{
		apbclock = RCC_ClocksStatus.PCLK2_Frequency;
	}
	else
	{
		apbclock = RCC_ClocksStatus.PCLK1_Frequency;
	}
	integerdivider = ((0x19 * apbclock) / (0x04 * (pChan->baudRate)));		// Determine the integer part
	tmpreg = (integerdivider / 0x64) << 0x04;	
	fractionaldivider = integerdivider - (0x64 * (tmpreg >> 0x04));		    // Determine the fractional part 
	tmpreg |= ((((fractionaldivider * 0x10) + 0x32) / 0x64)) & ((uint8_t)0x0F);	
	pbase->BRR = (uint16_t)tmpreg;		                                    // Write to USART BRR
	return TRUE;
#endif

#if defined (STM32F4XX)
	/*---------------------------- USART BRR Configuration -----------------------*/
  /* Configure the USART Baud Rate */
  RCC_GetClocksFreq(&RCC_ClocksStatus);

  if ((pbase == USART1) || (pbase == USART6))
  {
    apbclock = RCC_ClocksStatus.PCLK2_Frequency;
  }
  else
  {
    apbclock = RCC_ClocksStatus.PCLK1_Frequency;
  }
  
  /* Determine the integer part */
  if ((pbase->CR1 & USART_CR1_OVER8) != 0)
  {
    /* Integer part computing in case Oversampling mode is 8 Samples */
    integerdivider = ((25 * apbclock) / (2 * (pChan->baudRate)));    
  }
  else /* if ((USARTx->CR1 & USART_CR1_OVER8) == 0) */
  {
    /* Integer part computing in case Oversampling mode is 16 Samples */
    integerdivider = ((25 * apbclock) / (4 * (pChan->baudRate)));    
  }
  tmpreg = (integerdivider / 100) << 4;

  /* Determine the fractional part */
  fractionaldivider = integerdivider - (100 * (tmpreg >> 4));

  /* Implement the fractional part in the register */
  if ((pbase->CR1 & USART_CR1_OVER8) != 0)
  {
    tmpreg |= ((((fractionaldivider * 8) + 50) / 100)) & ((uint8_t)0x07);
  }
  else /* if ((USARTx->CR1 & USART_CR1_OVER8) == 0) */
  {
    tmpreg |= ((((fractionaldivider * 16) + 50) / 100)) & ((uint8_t)0x0F);
  }
  
  /* Write to USART BRR register */
  pbase->BRR = (uint16_t)tmpreg;	

#endif
  return TRUE;
}	
// *************************************************
// �������� 
// ���ж� ����ʹ�� 
// pbase -- ���ڼĴ�������ַ 
// *************************************************/
static void  Uart_Start(USART_TypeDef* pbase)
{
	USART_ITConfig(USART1, USART_IT_TXE, ENABLE);	// ���ͻ����������Ѿ����� 
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);	// ���ջ������յ����� 

	// USARTʹ�� USART_CR1 bit14  		
	USART_Cmd(USART1, ENABLE);

}
// *************************************************
// UartOptionSet -set uart option .byte size,stop bit,parity 	
// PARA: pChan ���ڻ�����Ϣ���ݽṹ ��	pbase �Ĵ�������ʼ��ַ 
// RETURN: 
// ************************************************
static void UartOptionSet(STM32F10x_UART_CHAN *pChan, USART_TypeDef* pbase)
{
	uint32 tmpreg = pbase->CR1;
		
	if(pChan->WordLength == USART_WordLength_9b)				// byte size
		tmpreg |= (1<<12);                      // һ����ʼλ��9������λ��һ��ֹͣλ 	
	else
		tmpreg &= ~((uint32)(1<<12));				// һ����ʼλ��8������λ��n��ֹͣλ 
	if(pChan->Parity != USART_Parity_No)  // У����Ʊ�ʹ��
	{
		tmpreg |= (1<<10);	
		if(pChan->Parity == USART_Parity_Odd) // ��У��
		{
		 	tmpreg |= (1<<9);	
		}
		else
			tmpreg &= ~((uint32)(1<<9));		
	}
	else
		tmpreg &= ~((uint32)(1<<10));	
	pbase->CR1 = tmpreg;   

	tmpreg = pbase->CR2;
	tmpreg &= ~((uint32)(3<<12));	        // һ��ֹͣλ 
	if(pChan->Stopbits == USART_StopBits_0_5)          // 0.5��ֹͣλ 
		tmpreg |= (1<<12);
	else if(pChan->Stopbits == USART_StopBits_2)      // 2��ֹͣλ 
		tmpreg |= (2<<12);	
	else if(pChan->Stopbits == USART_StopBits_1_5)     // 1.5��ֹͣλ 
		tmpreg |= (3<<12);	
	pbase->CR2 = tmpreg;  	
}
// ***************************************************************************
// UartTypeSet -   �ܽ����ü���ʼ��   485ģʽʱ �շ����ƹܽ����ü����ʼ��		
// PARA:   pChan ���ڻ�����Ϣ���ݽṹ ��pbase �Ĵ�������ʼ��ַ 
// RETURN: �� 
/*
USART1  - TX:PA9 RX:PA10 TEN:PB7
USART2  - TX:PD5 RX:PD6 TEN:PB9
USART3  - TX:PD8 RX:PD9 TEN:PB14
UART4  - TX:PC10 RX:PC11 TEN:PB15
UART5  - TX:PC12 RX:PD2 TEN:PD7
*/
// ****************************************************************************

static void UartTypeSet(STM32F10x_UART_CHAN *pChan, USART_TypeDef* pbase)
{
	GPIO_InitTypeDef GPIO_InitStructure;

#if defined (STM32F10X_CL) 
	if(pbase == (USART_TypeDef*)USART1_BASE) // UART0,(PA10 USART1_RX,PA9 USART1_TX) 
	{		
		// ��������PA9��PA10����USB(CN2)���ã�������JP42 JP43 ��������ӳ�� 	USB���ܸ��� 
		// ���ø�����ӳ��͵���I/O���üĴ���AFIO_MAPR ��ʹ��USAR1�� IO��ӳ�书��		
		GPIO_PinRemapConfig(GPIO_Remap_USART1, DISABLE);

		/* Enable GPIO clock */
		RCC_AHB1PeriphClockCmd(COM_TX_PORT_CLK[COM] | COM_RX_PORT_CLK[COM], ENABLE);

		//enble clock
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
				
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;		         // GPIO�ܽ�����	  USART_RX
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;                // GPIO�ܽ�����	  USART_TX
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_Init(GPIOA, &GPIO_InitStructure);

		if(pChan->devicType == RS485TYPE)  // 485��ʽ ��Ҫ����485�շ����ƹܽ� 
		{
			GPIO_InitStructure.GPIO_Pin = pChan->rs485ContrlPin;                // GPIO�ܽ�����	  USART_TXEN
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
			GPIO_Init((GPIO_TypeDef*)pChan->directionGPIO, &GPIO_InitStructure); 
			GPIO_WriteBit(pChan->directionGPIO,pChan->rs485ContrlPin,Bit_SET);  //clr ctr pin
		}
	 }
	 else if (pbase == (USART_TypeDef*)USART2_BASE)
	 {
		// ������ ��ӳ�� �� PD3-PD7  (USART2_CTS USART2_RTS USART2_TX USART2_RX USART2_CK)
		// ���ø�����ӳ��͵���I/O���üĴ���AFIO_MAPR ʹ��USAR2�� IO��ӳ�书��	 ��Ӱ���������� ֻ��ռ��GPIO 	
		GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE);

		//enble clock
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
				
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;		         // GPIO�ܽ�����	  USART_RX
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_Init(GPIOD, &GPIO_InitStructure);
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;                // GPIO�ܽ�����	  USART_TX
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_Init(GPIOD, &GPIO_InitStructure);

		if(pChan->devicType == RS485TYPE)  // 485��ʽ ��Ҫ����485�շ����ƹܽ� 
		{
			GPIO_InitStructure.GPIO_Pin = pChan->rs485ContrlPin;                // GPIO�ܽ�����	  USART_TXEN
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
			GPIO_Init((GPIO_TypeDef*)pChan->directionGPIO, &GPIO_InitStructure); 
			GPIO_WriteBit(pChan->directionGPIO,pChan->rs485ContrlPin,Bit_SET);  //clr ctr pin
		}	 
	 }
	 else if (pbase == (USART_TypeDef*)USART3_BASE)
	 {
		// ����ӳ�䲻���У���Ϊ��ethnet���� 
		// ������ӳ�䲻���У���Ϊӳ��ռ��uart4�ܽ�
		// ȫ����ӳ�� ���ܿ��У� ����ethnetһ��������ӳ�� 	
		// ������ ��ȫ��ӳ�� �� PD8-12  (USART3_TX USART3_RX USART3_CK USART3_CTS USART3_RTS)
		// ���ø�����ӳ��͵���I/O���üĴ���AFIO_MAPR ʹ��USAR3�� IO������ӳ�书��	
		GPIO_PinRemapConfig(GPIO_FullRemap_USART3, ENABLE);
		
		//enble clock
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
				
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;		         // GPIO�ܽ�����	  USART_RX
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_Init(GPIOD, &GPIO_InitStructure);
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;                // GPIO�ܽ�����	  USART_TX
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_Init(GPIOD, &GPIO_InitStructure);

		if(pChan->devicType == RS485TYPE)  // 485��ʽ ��Ҫ����485�շ����ƹܽ� 
		{
			GPIO_InitStructure.GPIO_Pin = pChan->rs485ContrlPin;                // GPIO�ܽ�����	  USART_TXEN
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
			GPIO_Init((GPIO_TypeDef*)pChan->directionGPIO, &GPIO_InitStructure); 
			GPIO_WriteBit(pChan->directionGPIO,pChan->rs485ContrlPin,Bit_RESET);  //clr ctr pin
		}	 
	 }
	 else if (pbase == (USART_TypeDef*)UART4_BASE)
	 {
		// û����ӳ��Ĺ��� PC10-11 (UART4_TX UART4_RX)	
		//enble clock
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
							
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;		         // GPIO�ܽ�����	  USART_RX
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_Init(GPIOC, &GPIO_InitStructure);
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;                // GPIO�ܽ�����	  USART_TX
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_Init(GPIOC, &GPIO_InitStructure);

		if(pChan->devicType == RS485TYPE)  // 485��ʽ ��Ҫ����485�շ����ƹܽ� 
		{
			GPIO_InitStructure.GPIO_Pin = pChan->rs485ContrlPin;                // GPIO�ܽ�����	  USART_TXEN
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
			GPIO_Init((GPIO_TypeDef*)pChan->directionGPIO, &GPIO_InitStructure); 
			GPIO_WriteBit(pChan->directionGPIO,pChan->rs485ContrlPin,Bit_SET);  //clr ctr pin
		}	 
	 }
	 else if (pbase == (USART_TypeDef*)UART5_BASE)
	 {
		// û����ӳ��Ĺ��� PC12 PD2 (UART5_TX UART5_RX)
		//enble clock
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
								
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;		         // GPIO�ܽ�����	  USART_RX
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_Init(GPIOD, &GPIO_InitStructure);
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;                // GPIO�ܽ�����	  USART_TX
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_Init(GPIOC, &GPIO_InitStructure);

		if(pChan->devicType == RS485TYPE)                        // 485��ʽ ��Ҫ����485�շ����ƹܽ� 
		{
			GPIO_InitStructure.GPIO_Pin = pChan->rs485ContrlPin;                // GPIO�ܽ�����	  USART_TXEN
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
			GPIO_Init((GPIO_TypeDef*)pChan->directionGPIO, &GPIO_InitStructure); 
			GPIO_WriteBit(pChan->directionGPIO,pChan->rs485ContrlPin,Bit_SET);  //clr ctr pin
		}	 
	}
	
	if(pChan->devicType == IRDATYPE)
	{	//����ģʽ
		/* Configure the USARTy IrDA mode */
		USART_IrDAConfig(pbase,USART_IrDAMode_Normal);
		/* Enable the USARTy IrDA mode */
		USART_IrDACmd(pbase, ENABLE);  		
	}
	else
	{
		/* disable the USARTy IrDA mode */
		USART_IrDACmd(pbase, DISABLE);
	}    
#endif

#if defined (STM32F4XX)
	if(pbase == (USART_TypeDef*)USART1_BASE) // UART1,(PA10 USART1_RX,PA9 USART1_TX) 
	{		
		/* Enable UART clock */
    	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);	

		/* Enable GPIO clock */
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

		/* Connect PXx to USARTx_Tx*/
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
		
		/* Connect PXx to USARTx_Rx*/
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource10,GPIO_AF_USART1);
		
		/* Configure USART Tx as alternate function  */
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);		 // GPIO�ܽ�����	  USART_TX

		/* Configure USART Rx as alternate function  */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;		         // GPIO�ܽ�����	  USART_RX
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		
		if(pChan->devicType == RS485TYPE)  // 485��ʽ ��Ҫ����485�շ����ƹܽ� 
		{
			GPIO_InitStructure.GPIO_Pin = pChan->rs485ContrlPin;                // GPIO�ܽ�����	  USART_TXEN
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
			GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
			GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_Init((GPIO_TypeDef*)pChan->directionGPIO, &GPIO_InitStructure); 
			GPIO_WriteBit(pChan->directionGPIO,pChan->rs485ContrlPin,Bit_SET);  //clr ctr pin
		}
	 }
	 else if (pbase == (USART_TypeDef*)USART2_BASE)
	 {
		/* Enable GPIO clock */
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

		/* Connect PXx to USARTx_Tx*/
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
		
		/* Connect PXx to USARTx_Rx*/
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource3,GPIO_AF_USART2);
		
		/* Configure USART Tx as alternate function  */
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);		 // GPIO�ܽ�����	  USART_TX	   PD5

		/* Configure USART Rx as alternate function  */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;		         // GPIO�ܽ�����	  USART_RX	PD6
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		
		if(pChan->devicType == RS485TYPE)  // 485��ʽ ��Ҫ����485�շ����ƹܽ� 
		{
			GPIO_InitStructure.GPIO_Pin = pChan->rs485ContrlPin;                // GPIO�ܽ�����	  USART_TXEN
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
			GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
			GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_Init((GPIO_TypeDef*)pChan->directionGPIO, &GPIO_InitStructure); 
			GPIO_WriteBit(pChan->directionGPIO,pChan->rs485ContrlPin,Bit_SET);  //clr ctr pin
		}
	 }
	 else if (pbase == (USART_TypeDef*)USART3_BASE)
	 {
#if 0		 
		/* Enable GPIO clock */
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

		/* Connect PXx to USARTx_Tx*/
		GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_USART3);
		
		/* Connect PXx to USARTx_Rx*/
		GPIO_PinAFConfig(GPIOD, GPIO_PinSource9,GPIO_AF_USART3);
		
		/* Configure USART Tx as alternate function  */
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOD, &GPIO_InitStructure);		 // GPIO�ܽ�����	  USART_TX	   PB10

		/* Configure USART Rx as alternate function  */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;		         // GPIO�ܽ�����	  USART_RX	PB11
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_Init(GPIOD, &GPIO_InitStructure);
		
		if(pChan->devicType == RS485TYPE)  // 485��ʽ ��Ҫ����485�շ����ƹܽ� 
		{
			GPIO_InitStructure.GPIO_Pin = pChan->rs485ContrlPin;                // GPIO�ܽ�����	  USART_TXEN
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
			GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
			GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_Init((GPIO_TypeDef*)pChan->directionGPIO, &GPIO_InitStructure); 
			GPIO_WriteBit(pChan->directionGPIO,pChan->rs485ContrlPin,Bit_RESET);  //clr ctr pin
		}
#endif		
	 }
	 else if (pbase == (USART_TypeDef*)UART4_BASE)
	 {
#if 0	 
		/* Enable GPIO clock */
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

		/* Connect PXx to USARTx_Tx*/
		GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_UART4);
		
		/* Connect PXx to USARTx_Rx*/
		GPIO_PinAFConfig(GPIOC, GPIO_PinSource11,GPIO_AF_UART4);
		
		/* Configure USART Tx as alternate function  */
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOC, &GPIO_InitStructure);		 // GPIO�ܽ�����	  USART_TX	   PC10

		/* Configure USART Rx as alternate function  */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;		         // GPIO�ܽ�����	  USART_RX	PC11
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_Init(GPIOC, &GPIO_InitStructure);
		
		if(pChan->devicType == RS485TYPE)  // 485��ʽ ��Ҫ����485�շ����ƹܽ� 
		{
			GPIO_InitStructure.GPIO_Pin = pChan->rs485ContrlPin;                // GPIO�ܽ�����	  USART_TXEN
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
			GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
			GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_Init((GPIO_TypeDef*)pChan->directionGPIO, &GPIO_InitStructure); 
			GPIO_WriteBit(pChan->directionGPIO,pChan->rs485ContrlPin,Bit_SET);  //clr ctr pin
		}	 
#endif		
	 }
	 else if (pbase == (USART_TypeDef*)UART5_BASE)
	 {	 		
		/* Enable GPIO clock */
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

		/* Connect PXx to USARTx_Tx*/
		GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF_UART5);
		
		/* Connect PXx to USARTx_Rx*/
		GPIO_PinAFConfig(GPIOD, GPIO_PinSource2,GPIO_AF_UART5);
		
		/* Configure USART Tx as alternate function  */
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOC, &GPIO_InitStructure);		 // GPIO�ܽ�����	  USART_TX	   PC12

		/* Configure USART Rx as alternate function  */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;		         // GPIO�ܽ�����	  USART_RX	PD2
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_Init(GPIOD, &GPIO_InitStructure);
		
		if(pChan->devicType == RS485TYPE)  // 485��ʽ ��Ҫ����485�շ����ƹܽ� 
		{
			GPIO_InitStructure.GPIO_Pin = pChan->rs485ContrlPin;                // GPIO�ܽ�����	  USART_TXEN
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
			GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
			GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_Init((GPIO_TypeDef*)pChan->directionGPIO, &GPIO_InitStructure); 
			GPIO_WriteBit(pChan->directionGPIO,pChan->rs485ContrlPin,Bit_SET);  //clr ctr pin
		}	  
	}
	else if (pbase == (USART_TypeDef*)USART6_BASE)
	 {
#if 0			 
	 	/* Enable UART clock */
    	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);

		/* Enable GPIO clock */
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

		/* Connect PXx to USARTx_Tx*/
		GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_USART6);
		
		/* Connect PXx to USARTx_Rx*/
		GPIO_PinAFConfig(GPIOC, GPIO_PinSource7,GPIO_AF_USART6);
		
		/* Configure USART Tx as alternate function  */
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOC, &GPIO_InitStructure);		 // GPIO�ܽ�����	  USART_TX	   PC6

		/* Configure USART Rx as alternate function  */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;		         // GPIO�ܽ�����	  USART_RX	PC7
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_Init(GPIOC, &GPIO_InitStructure);
		
		if(pChan->devicType == RS485TYPE)  // 485��ʽ ��Ҫ����485�շ����ƹܽ� 
		{
			GPIO_InitStructure.GPIO_Pin = pChan->rs485ContrlPin;                // GPIO�ܽ�����	  USART_TXEN
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
			GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
			GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_Init((GPIO_TypeDef*)pChan->directionGPIO, &GPIO_InitStructure); 
			GPIO_WriteBit(pChan->directionGPIO,pChan->rs485ContrlPin,Bit_SET);  //clr ctr pin
		}
#endif		
	 }
	
	if(pChan->devicType == IRDATYPE)
	{	//����ģʽ
		/* Configure the USARTy IrDA mode */
		USART_IrDAConfig(pbase,USART_IrDAMode_Normal);
		/* Enable the USARTy IrDA mode */
		USART_IrDACmd(pbase, ENABLE);  		
	}
	else
	{
		/* disable the USARTy IrDA mode */
		USART_IrDACmd(pbase, DISABLE);
	}
	
#endif

}

//ģʽ�ж�
BOOL IsUartMode(uint8 mode)
{
	if((mode == RS232TYPE)||(mode == RS485TYPE)||(mode == IRDATYPE)||(mode == IR38KHZTYPE))
		return TRUE;

	return FALSE;
}


//���ڲ�������
BOOL Uart_ParamSet(STM32F10x_UART_CHAN* pChan)
{
	USART_InitTypeDef USART_InitStructure; 		
	USART_TypeDef* pbase = (USART_TypeDef*)pChan->base;	

	USART_InitStructure.USART_BaudRate = pChan->baudRate;	     // ���ڲ������� bps ����λ ֹͣλ У�鷽ʽ ���� 
	USART_InitStructure.USART_WordLength = pChan->WordLength;
	USART_InitStructure.USART_StopBits = pChan->Stopbits;
	USART_InitStructure.USART_Parity = pChan->Parity;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	
	USART_Init(pbase, &USART_InitStructure);				 // ���ڲ�������
	return TRUE;
}

// **************************************************************************
// Uart_IOCtrl -opreat  the UART 
// PARA: devDesc--device description, cmd--request cmd, arg--some argument.
// RETURN: FALSE - error,or TRUE - successful
//****************************************************************************
int Uart_IOCtrl(int32 devDesc, int32 cmd, void* arg)
{
#if OS_CRITICAL_METHOD == 3
	OS_CPU_SR  cpu_sr;
#endif  
	int ret = FALSE;	
	uint8 err;
	STM32F10x_UART_CHAN* pChan = (STM32F10x_UART_CHAN*)devDesc;	 // ������Ϣ���ݽṹ 
	USART_TypeDef* pbase = (USART_TypeDef*)pChan->base;	         // �Ĵ�������ַ���ݽṹ 

	if((devDesc == 0) || (pbase == 0))
	{
		switch(cmd)  //���������Ϊ���ڲ�֪���豸�������¿����豸
		{
			case SIO_GET_PORT_HANDLE:
				OS_ENTER_CRITICAL();
				if(*(uint32*)arg <= N_UART_CHANNELS)
				{		
					*(uint32*)arg = (int32)&UARTChan[*(uint32*)arg]; 
				}	
				else
				{
						*(uint32*)arg = (uint32)NULL;
				}
				OS_EXIT_CRITICAL();	
				ret = TRUE;
				break;
			default:
				break;
		}
		return ret;
	}
	OSSemPend(pChan->uartLock, 0, &err);
	switch(cmd)
	{
		case SIO_BAUD_SET:	                           // Set the baud rate 
		{
			OS_ENTER_CRITICAL();
			pChan->baudRate = *(int32*)arg;				// add by wxh 2010-11-20
			if(Uart_ParamSet(pChan) == TRUE)     // ���ò����� 
			{
				ret = TRUE;
			}
			OS_EXIT_CRITICAL();			
		}
		break;
		case SIO_START:	                               // UART Start work
			OS_ENTER_CRITICAL();
			Uart_Start(pbase);
			OS_EXIT_CRITICAL();
			ret = TRUE;
			break;
		case SIO_HW_OPTS_SET:	                       // Set wordbits
			OS_ENTER_CRITICAL();
			UartOptionSet(pChan, pbase);	
			OS_EXIT_CRITICAL();
			ret = TRUE;
			break;
		case SIO_PARITY_BIT_SET:	                       // Parity bit set
			OS_ENTER_CRITICAL();
			if(IS_USART_PARITY(*(int32*)arg))
			{
				pChan->Parity = *(int32*)arg;
				Uart_ParamSet(pChan);
			}	
			OS_EXIT_CRITICAL();
			ret = TRUE;
			break;
		case SIO_STOP_BIT_SET:	                       // STOP bit set
			OS_ENTER_CRITICAL();
			if(IS_USART_STOPBITS(*(int32*)arg))
			{
				pChan->Stopbits = *(int32*)arg;		 //modify by wxh 2010-11-20	   pChan->Parity = *(int32*)arg;
				Uart_ParamSet(pChan);
			}	
			OS_EXIT_CRITICAL();
			ret = TRUE;
			break;
		case SIO_DATA_BIT_SET:	                       // DATA bit set
			OS_ENTER_CRITICAL();
			if(IS_USART_WORD_LENGTH(*(int32*)arg))
			{
				pChan->WordLength = *(int32*)arg;	//modify by wxh 2010-11-20		pChan->Parity = *(int32*)arg;
				Uart_ParamSet(pChan);
			}	
			OS_EXIT_CRITICAL();
			ret = TRUE;
			break;
		case SIO_WORK_TYPE_SET:	                      // type set(rs232 or rs485)
			OS_ENTER_CRITICAL();
			if(IsUartMode(*(int*)arg))
			{
				pChan->devicType = *(int*)arg;
				UartTypeSet(pChan, pbase);
				ret = TRUE;
			}
			OS_EXIT_CRITICAL();				
			break;
		case SIO_SEND_STATUS_GET:	                      // ��÷�����״̬
			OS_ENTER_CRITICAL();
			*(int*)arg = pChan->SendStatus;	  //type info UART_SND_STA
			OS_EXIT_CRITICAL();				
			break;
		case SIO_SEND_BUF_FULL_STATUS_GET: /*��÷��ͻ�����״̬*/
			OS_ENTER_CRITICAL();
			if(arg != NULL)
			{
			    if(IsQueueFull(pChan->SendBuf) == FALSE)
				{//����
					*(uint32*)arg = UART_SEND_BUF_NO_FULL;			
				}else{ //��
					*(uint32*)arg = UART_SEND_BUF_FULL;
				}
				ret = TRUE;
			}
			OS_EXIT_CRITICAL();
			break;	
		case SIO_USE_TYPE_SET:	                      // ����ʹ������
			OS_ENTER_CRITICAL();
			if(IS_VALID_USE_TYPE(*(int*)arg))
			{
				pChan->useType = (UART_USE_TYPE)(*(int*)arg);	  
			}
			OS_EXIT_CRITICAL();				
			break;
		case SIO_USE_TYPE_GET:	                      // ���ʹ������
			OS_ENTER_CRITICAL();
			*(int*)arg = pChan->useType;	  
			OS_EXIT_CRITICAL();				
			break;
		case SIO_GPS_SYN_TIME_GET:	                      // ���GPSͬ��ʱ��ʱ��
			OS_ENTER_CRITICAL();
			*(uint32*)arg = pChan->Timegps.RevGpsSynTime;	  
			OS_EXIT_CRITICAL();				
			break;
		default:
			break;
	}
	OSSemPost(pChan->uartLock);
	return ret;
}

// *************************************************
// Uart_EnIsr - desable a ISR .	
// RETURN: N
// *************************************************
static void Uart_DisableIsr(STM32F10x_UART_CHAN* pChan)
{	
	USART_Cmd((USART_TypeDef*)pChan->base, DISABLE);	
}

// *************************************************
// Uart_Close
// RETURN: FALSE - error,or TRUE - successful
// *************************************************
int32 Uart_Close(uint32 devDesc)		//device description
{
	int8 i;
	uint8 err;
	for(i = 0;i < N_UART_CHANNELS; i++)
	{
		if(devDesc == (uint32)&UARTChan[i])
		{
			OSSemPend(UARTChan[i].uartLock, 0, &err); 		
			UARTChan[i].deviceOpenFlag = FALSE;
			Uart_DisableIsr(&UARTChan[i]);
			OSSemPost(UARTChan[i].uartLock);			
			return TRUE;
		}
	}
	return FALSE;
}
// ********************************************************************************
// Uart_Read -read data from the UART .
// note:
//	the function will be pend when no data 
// PARA: devDesc - device description struct ,	buf	- save to buffer
// RETURN: byte numbers readed
// *********************************************************************************
int32 Uart_Read(int32 devDesc,uint8 *buf, uint16 maxSize)
{
	uint16 ret = 0;
	STM32F10x_UART_CHAN* pChan = (STM32F10x_UART_CHAN*)devDesc;
	if(devDesc == 0)
	{
		return 0;
	}
	for(ret = 0; ((ret < MAX_UART_BUF_LENTH) && (ret < maxSize)); ret++)	           // ȡ���� 
	{
	   if(QueueRead(buf, pChan->RcvBuf) == QUEUE_OK)           // ��ȡ�ɹ� 
			buf++;	   		
	   else
		break;
	}
	return ret;
}

/********************************************************************************
** Uart_ClearReadBuf -clear read buf  .
** note:
** the function will be pend when no data 
** PARA: devDesc - device description struct ,	buf	- save to buffer
** RETURN: byte numbers readed
*********************************************************************************/
int32 Uart_ClearReadBuf(int32 devDesc)
{
	uint16 cnt = 0;
	int32 ret;
	uint8 buf[50] ;
	cnt = 0;
	do{
		ret = Uart_Read(devDesc,buf,50);
		if( cnt++ > 100 ) {  /*old is 10 20190403 */
			 break;
		}
	}while(ret > 0);
	
	return ret;
}

/********************************************************************************
** Uart_GetBytesNumInBuff -write data to the UART .
** PARA: devDesc -- device description, buf size--buffer and  data size writed
** RETURN: numbers writed 
*********************************************************************************/
uint16 Uart_GetBytesNumInBuff(int32 devDesc)
{
	STM32F10x_UART_CHAN* pChan = (STM32F10x_UART_CHAN*)devDesc;
	if(devDesc == 0)
	{
		return 0;
	}
	 return QueueNData(pChan->RcvBuf);
}

// ***************************************************************************
// Uart_Write -write data to the UART .
// PARA: devDesc -- device description, buf size--buffer and  data size writed
// RETURN: numbers writed 
// ****************************************************************************
int32 Uart_Write(int32 devDesc,	uint8 *buf,	int32 size)
{
	int delayMax = 200;
	uint8 err;
	int len = size;
	int32 temp;
	STM32F10x_UART_CHAN* pChan = (STM32F10x_UART_CHAN*)devDesc;
	USART_TypeDef* pbase = (USART_TypeDef*)pChan->base;
	if(devDesc == 0)
	{		
		return 0;
	}
	if(pChan->useType == UART_USE_TYPE_GPS)
	{
		return 0;
	}
	OSSemPend(pChan->uartLock, 0, &err);
	if (len > 0)
	{	
		do{	
			while((err = QueueWrite((void *)pChan->SendBuf, buf)) == QUEUE_OK)    // ������� 
			{
				len --;
				buf++;
				if(len <= 0)
					break;			
			}
			USART_ITConfig(pbase, USART_IT_TXE, ENABLE);	
			USART_ITConfig(pbase, USART_IT_TC, ENABLE);	
			if(pChan->devicType == IR38KHZTYPE)
			{	//38K�����ͺ���ģʽ��Ϊ��˫��ģʽ
				USART_ITConfig(pbase, USART_IT_RXNE, DISABLE); 
			}
			if(err==QUEUE_FULL)
			{
				OSTimeDly(2);
			}				 				
		}while((--delayMax <= 0) && (len>=0));		
	}
	OSSemPost(pChan->uartLock);
	delayMax = 200;
	while(--delayMax)
	{
		OSTimeDly(2);
		if((len = QueueNData(pChan->SendBuf)) == 0)
			break;
	}
	temp = (int32)(size - len);
	return temp;
}

void Uart_Send_data(int32 devDesc, u8 *s, uint16 len)
{	
	uint8 err;	
	STM32F10x_UART_CHAN* pChan = (STM32F10x_UART_CHAN*)UartHandler[devDesc];
	
	//���󻥳���
//	OSMutexPend(d_comm_smp,0,&err); 
	
	OSSemPend(pChan->uartLock, 500, &err);
		
	for(int i =0; i < len; i++)	
	{ 		
		
		if(pChan->devicType == RS485TYPE ) {
			GPIO_WriteBit(pChan->directionGPIO,pChan->rs485ContrlPin,Bit_SET);	//set ctr pin
		}
		
		USART_SendData(pChan->base,s[i]);
		
		while(USART_GetFlagStatus(pChan->base,USART_FLAG_TC )==RESET){			
			if(pChan->devicType == RS485TYPE )
				GPIO_WriteBit(pChan->directionGPIO,pChan->rs485ContrlPin,Bit_RESET);  //clr ctr pin
		};
	}

	OSSemPost(pChan->uartLock);
	//�ͷŻ�����
//	OSMutexPost(d_comm_smp);
	
}

//===================================================================
// ����ʱ��ʹ�� 
//
//===================================================================
void UsartClockOperation(STM32F10x_UART_CHAN *pChan, uint8 cmd)
{
	uint8 i;
	for(i = 0;i < N_UART_CHANNELS; i++)
	{
		if(pChan == &UARTChan[i])  
		{
			if(i == 0)
			{
				if(cmd != DISABLE)	  // USART1ʱ��ʹ��
					RCC->APB2ENR |= (1<<14);	
				else
					RCC->APB2ENR &= ~((uint32)(1<<14));				
			}
			else	// usart 2-5
			{
				if(cmd != DISABLE)	  // USART1ʱ��ʹ��
					RCC->APB1ENR |= (1<<(16+i));	
				else
					RCC->APB1ENR &= ~((uint32)(1<<(16+i)));				
					
			}
		} 	
	}
}

/*************************************************
LPCUartDevInit - initialize  a  UART .
RETURN: N
*************************************************/
#ifdef SCOMM_DEBUG
extern	uint8 debug_comm;	// BACK_COM CARD_COM;
#endif

static void UartDevInit(STM32F10x_UART_CHAN* pChan)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	USART_InitTypeDef USART_InitStructure;
		
	USART_TypeDef* pbase = (USART_TypeDef*)pChan->base;

	UsartClockOperation(pChan, ENABLE);                          // �������� RCCʱ������ 

	USART_InitStructure.USART_BaudRate = pChan->baudRate;	     // ���ڲ������� bps ����λ ֹͣλ У�鷽ʽ ���� 
	USART_InitStructure.USART_WordLength = pChan->WordLength;
	USART_InitStructure.USART_StopBits = pChan->Stopbits;
	USART_InitStructure.USART_Parity = pChan->Parity;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	UartTypeSet(pChan, pbase);                                   // �ܽ�����

	if(pbase == (USART_TypeDef*)USART1_BASE)                     // UART0,(PA10 USART1_RX,PA9 USART1_TX) ; BACK_COM    COM1
	{		
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); 	 // �������� USART����ʱ�� 
//		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);	  	     // ���� ����NVIC  Ӧ���ж��븴λ���ƼĴ���   ����Ӧ�����ȼ����з���
		NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;	     // �жϺ� 37  
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	// �������ȼ� 0 
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	     // ��Ӧ���ȼ� 0 
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	         // ʹ�ܴ���1�ж� 
		NVIC_Init(&NVIC_InitStructure);	                         // �����ж����ȼ���ʹ���ж� 						
		
		USART_Init(USART1, &USART_InitStructure);				 // ���ڲ������� 
		USART_ITConfig(USART1, USART_IT_TXE, ENABLE);			 // ������ж�����-���ͻ����������Ѿ�����
		USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);	         // ������ж�����-���ջ������յ����� 
				  		
		USART_Cmd(USART1, ENABLE);	                             // USARTʹ�� USART_CR1 bit13
	}
	else if(pbase == (USART_TypeDef*)USART2_BASE)				 // METER_COM   COM2 
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); 	 // �������� USART����ʱ�� 

//		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);	  	     // ���� ����NVIC  Ӧ���ж��븴λ���ƼĴ���   ����Ӧ�����ȼ����з���
		NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;	     // �жϺ� 38  
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;// �������ȼ� 1 
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;	     // ��Ӧ���ȼ� 0 
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	         // ʹ�ܴ����ж� 
		NVIC_Init(&NVIC_InitStructure);	                         // �����ж����ȼ���ʹ���ж� 		
		 
		USART_Init(USART2, &USART_InitStructure);				 // ���ڲ�������
		USART_ITConfig(USART2, USART_IT_TXE, ENABLE); 	         // ������ж�����-���ͻ����������Ѿ�����
		USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);	         // ������ж�����-���ջ������յ����� 		
		USART_Cmd(USART2, ENABLE);		                         // USARTʹ��		
	}	
#if 1	
	else if(pbase == (USART_TypeDef*)USART3_BASE)				// ISO_COM     COM3 or MOD_COM     COM3
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE); 	 // �������� USART����ʱ�� 

//		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);	  	     // ���� ����NVIC  Ӧ���ж��븴λ���ƼĴ���   ����Ӧ�����ȼ����з���
		NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;	     // �жϺ� 39  
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;// �������ȼ� 1 
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;	     // ��Ӧ���ȼ� 2 
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	         // ʹ�ܴ����ж� 
		NVIC_Init(&NVIC_InitStructure);	                         // �����ж����ȼ���ʹ���ж� 		
		
		USART_Init(USART3, &USART_InitStructure);				 // ���ڲ������� 
		USART_ITConfig(USART3, USART_IT_TXE, ENABLE);	         // ������ж�����-���ͻ����������Ѿ�����
		USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);	         // ������ж�����-���ջ������յ����� 
				
		USART_Cmd(USART3, ENABLE);								 // USARTʹ��
	}	
#endif	
	else if(pbase == (USART_TypeDef*)UART4_BASE)				// SCREEN_COM  COM4
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE); 	 // �������� UART����ʱ�� 

//		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);	  	     // ���� ����NVIC  Ӧ���ж��븴λ���ƼĴ���   ����Ӧ�����ȼ����з���
		NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;	     // �жϺ� 52  
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;// �������ȼ� 1 
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;	     // ��Ӧ���ȼ� 3
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	         // ʹ�ܴ����ж� 
		NVIC_Init(&NVIC_InitStructure);	                         // �����ж����ȼ���ʹ���ж� 		

		 
		USART_Init(UART4, &USART_InitStructure);				 // ���ڲ�������

		USART_ITConfig(UART4, USART_IT_TXE, ENABLE);	         // ������ж�����-���ͻ����������Ѿ�����
		USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);	         // ������ж�����-���ջ������յ����� 
		
 		USART_ClearFlag(pChan->base,USART_FLAG_TXE);
		
		USART_Cmd(UART4, ENABLE);						         // USARTʹ��
	}	

	else if(pbase == (USART_TypeDef*)UART5_BASE)				// CARD_COM    COM5
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE); 	 // �������� UART����ʱ�� 

//		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);	  	     // ���� ����NVIC  Ӧ���ж��븴λ���ƼĴ���   ����Ӧ�����ȼ����з���
		NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;	     // �жϺ� 53  
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;// �������ȼ� 1 
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;	     // ��Ӧ���ȼ� 4 
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	         // ʹ�ܴ����ж� 
		NVIC_Init(&NVIC_InitStructure);	                         // �����ж����ȼ���ʹ���ж� 		
		 
		USART_Init(UART5, &USART_InitStructure);				 // ���ڲ�������
		USART_ITConfig(UART5, USART_IT_TXE, ENABLE);			 // ������ж�����-���ͻ����������Ѿ�����
		USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);	         // ������ж�����-���ջ������յ����� 
 		
		USART_Cmd(UART5, ENABLE);						         // USARTʹ��
	}
#if 1	
#if defined (STM32F4XX)
	else if(pbase == (USART_TypeDef*)USART6_BASE)				 // ACMETER_COM COM6
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE); 	 // �������� USART����ʱ�� 
//		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);	  	     // ���� ����NVIC  Ӧ���ж��븴λ���ƼĴ���   ����Ӧ�����ȼ����з���
		NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;	     // �жϺ� 37  
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;// �������ȼ� 1 
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 5;	     // ��Ӧ���ȼ� 1 
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	         // ʹ�ܴ���1�ж� 
		NVIC_Init(&NVIC_InitStructure);	                         // �����ж����ȼ���ʹ���ж� 						
		
		USART_Init(USART6, &USART_InitStructure);				 // ���ڲ������� 
 		 
		USART_ITConfig(USART6, USART_IT_TXE, ENABLE);	         // ������ж�����-���ͻ����������Ѿ�����
		USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);	         // ������ж�����-���ջ������յ����� 
				  		
		USART_Cmd(USART6, ENABLE);	                             // USARTʹ�� USART_CR1 bit13
	}
#endif
#endif

	UartOptionSet(pChan, pbase); //add option	
}

// ******************************************************************************
// sysSerialHwInit - initialize  Serial Devece struct and Hardware at first.
// RETURN: NONE
// ******************************************************************************
void sysSerialHwInit(void)
{
	uint8 i;
	if(PowerOnForUart == FALSE)
	{
		PowerOnForUart = TRUE;
		for(i = 0;i < N_UART_CHANNELS; i++)		 
		{
			UARTChan[i].uartLock = OSSemCreate(1);	                     // �����ź���  ��ʼ��Ϊ�Ѵ� 
			UARTChan[i].base = UARTParameter[i].baseAddr;                // �Ĵ�������ַ 
			UARTChan[i].directionGPIO =  UARTParameter[i].directionGPIO; // �������GPIO 
			UARTChan[i].rs485ContrlPin = UARTParameter[i].rs485ContrlPin;// ������ƽ���� 
//patli 20200225			UARTChan[i].deviceID = i+1;                                  // ���� ID
			UARTChan[i].deviceID = i;  
			UARTChan[i].deviceOpenFlag = FALSE;	                         // �򿪱�־��ʼ��Ϊ δ�� 
			UARTChan[i].devicType = UARTParameter[i].devicType;	         // �豸���� 1 rs232 0 rs485

			UARTChan[i].baudRate = UARTParameter[i].defBuardRate;	     // ������ 
			UARTChan[i].WordLength = UARTParameter[i].defWordLength;	 // Ĭ������λ 5-8 
			UARTChan[i].Stopbits = UARTParameter[i].defStopbits;	     // Ĭ��ֹͣλ 1-2  
			UARTChan[i].Parity = UARTParameter[i].defParity;	         // Ĭ��У��λ
			UARTChan[i].SendStatus =  UART_SEND_IDLE;
			UARTChan[i].useType =  UARTParameter[i].useType;	         // ʹ���豸����	 
			memset(&UARTChan[i].Timegps,0,sizeof(gpsuart_t)); 
			QueueCreate(UARTChan[i].RcvBuf, MAX_UART_BUF_LENTH,1,0,0);     // �������ݶ��� 
			QueueCreate(UARTChan[i].SendBuf, MAX_UART_BUF_LENTH,1,0,0);    // �������ݶ��� 		
			UartDevInit(&UARTChan[i]);
		}
	}
}



/********************************************************************************
// Function Name  : GetDevChan()
// Description    : This function get device object.
// Input          : dev address
// Output         : None
// Return         : device struct
*********************************************************************************/
STM32F10x_UART_CHAN *GetDevChan(USART_TypeDef* uart)
{
	uint8 i;
	for(i = 0;i < N_UART_CHANNELS; i++)	
	{
		if(uart == UARTChan[i].base)
			return &UARTChan[i];
	}	
	return NULL;
}
#ifdef STM32F4XX
 #define TRANS_TIMES 10
#endif
// *******************************************************************************
// Function Name  : USART1_IRQHandler
// Description    : This function handles USART1 global interrupt request.
// Input          : None
// Output         : None
// Return         : None
// *******************************************************************************
//extern SERIALPORT serial;
void  UartDealInIRQ(STM32F10x_UART_CHAN* pChan)
{
	uint8 tmpdata,rx_data;

	if (USART_GetFlagStatus(pChan->base, USART_FLAG_RXNE) != RESET)  // ���ջ������ǿ� 
	{
		rx_data = (uint8)USART_ReceiveData(pChan->base);
		QueueWriteInInt(pChan->RcvBuf, &rx_data); 
		// дһ���ֽڵ�����  �˴��ݲ�������� ������ݲ��ٽ��� 				
		setbit(uart_status, pChan->deviceID+8);  //recv status set
	}
	if (USART_GetFlagStatus(pChan->base, USART_FLAG_TXE) != RESET)  // ���ջ������ǿ�  
	{   
		USART_ClearFlag(pChan->base, USART_FLAG_TXE);
		if (QueueReadInInt(&tmpdata, pChan->SendBuf) == QUEUE_OK) // ������Ҫ���� 
		{
			if (pChan->devicType == RS485TYPE)
			{
				GPIO_WriteBit(pChan->directionGPIO,pChan->rs485ContrlPin,Bit_SET);  //set ctr pin
			}
			USART_SendData(pChan->base, tmpdata);
			pChan->SendStatus = UART_SEND_BUSY;		
		}
		else
		{		
      USART_ClearITPendingBit(pChan->base, USART_IT_TXE);			
			USART_ITConfig(pChan->base, USART_IT_TXE, DISABLE);
		}		
		setbit(uart_status, pChan->deviceID);  //write status set
	}
  if (USART_GetFlagStatus(pChan->base, USART_FLAG_TC) != RESET)  
	{
		USART_ClearFlag(pChan->base, USART_FLAG_TC);
		if (pChan->devicType == RS485TYPE)
		{
			GPIO_WriteBit(pChan->directionGPIO,pChan->rs485ContrlPin,Bit_RESET);  //clr ctr pin
			for(int i = 0; i< TRANS_TIMES; i++ );
		}
		USART_ITConfig(pChan->base, USART_IT_TC, DISABLE);
  }
	
  if (USART_GetFlagStatus(pChan->base, USART_FLAG_ORE) != RESET)
	{        
		rx_data = USART_ReceiveData(pChan->base);
		USART_ClearFlag(pChan->base, USART_FLAG_ORE);
	}
	
	if (USART_GetFlagStatus(pChan->base, USART_FLAG_NE) != RESET)
	{        
		USART_ClearFlag(pChan->base, USART_FLAG_NE);
	}
	
	if (USART_GetFlagStatus(pChan->base, USART_FLAG_FE) != RESET)
	{        
		USART_ClearFlag(pChan->base, USART_FLAG_FE);
	}
	
	if (USART_GetFlagStatus(pChan->base, USART_FLAG_PE) != RESET)
	{        
		USART_ClearFlag(pChan->base, USART_FLAG_PE);
	}
}

// *******************************************************************************
// Function Name  : USART1_IRQHandler
// Description    : This function handles USART1 global interrupt request.
// Input          : None
// Output         : None
// Return         : None
// *******************************************************************************
void USART1_IRQHandler(void)
{    
//	uint8 tmpdata,rx_data;
	uint32 regsr, regcr1;
	OS_CPU_SR  cpu_sr;
	STM32F10x_UART_CHAN* pChan = &UARTChan[COM1];	
//	STM32F10x_UART_CHAN* pChan = GetDevChan(USART1);	
	OS_ENTER_CRITICAL();                                       // Tell uC/OS-II that we are starting an ISR
	OSIntNesting++;
	OS_EXIT_CRITICAL();

	regsr = USART1->SR;
	regcr1 = USART1->CR1;
	if(regsr == regcr1)
	{
	   regsr = 0;
	}
	if(pChan != NULL)
	{
		UartDealInIRQ(pChan);
	} 	
	OSIntExit();                                                // Tell uC/OS-II that we are leaving the ISR 
}
//*******************************************************************************
// Function Name  : USART2_IRQHandler
// Description    : This function handles USART2 global interrupt request.
// Input          : None
// Output         : None
// Return         : None
//*******************************************************************************
void USART2_IRQHandler(void)
{    
//	uint8 tmpdata,rx_data;
	OS_CPU_SR  cpu_sr;
		STM32F10x_UART_CHAN* pChan = &UARTChan[COM2];	
//	STM32F10x_UART_CHAN* pChan = GetDevChan(USART2);
		
	OS_ENTER_CRITICAL();                                       // Tell uC/OS-II that we are starting an ISR
	OSIntNesting++;
	OS_EXIT_CRITICAL();

	if(pChan != NULL)
	{
		UartDealInIRQ(pChan);
	} 	
	OSIntExit();                                                // Tell uC/OS-II that we are leaving the ISR 
}

#if 1
// *******************************************************************************
// Function Name  : USART3_IRQHandler
// Description    : This function handles USART1 global interrupt request.
// Input          : None
// Output         : None
// Return         : None
//*******************************************************************************
void USART3_IRQHandler(void)
{    
//	uint8 tmpdata, rx_data;
	OS_CPU_SR  cpu_sr;
		STM32F10x_UART_CHAN* pChan = &UARTChan[COM3];	
//	STM32F10x_UART_CHAN* pChan = GetDevChan(USART3);
		
	OS_ENTER_CRITICAL();                                       // Tell uC/OS-II that we are starting an ISR
	OSIntNesting++;
	OS_EXIT_CRITICAL();

	if(pChan != NULL)
	{
		UartDealInIRQ(pChan);
	} 	
	OSIntExit();                                                // Tell uC/OS-II that we are leaving the ISR 
}
#endif

// *******************************************************************************
// Function Name  : USART4_IRQHandler
// Description    : This function handles USART1 global interrupt request.
// Input          : None
// Output         : None
// Return         : None
//*******************************************************************************
void UART4_IRQHandler(void)
{    
//	uint8 tmpdata, rx_data;
	OS_CPU_SR  cpu_sr;
			STM32F10x_UART_CHAN* pChan = &UARTChan[COM4];	
//	STM32F10x_UART_CHAN* pChan = GetDevChan(UART4);
		
	OS_ENTER_CRITICAL();                                       // Tell uC/OS-II that we are starting an ISR
	OSIntNesting++;
	OS_EXIT_CRITICAL();

	if(pChan != NULL)
	{
		UartDealInIRQ(pChan);
	} 	
	OSIntExit();                                                // Tell uC/OS-II that we are leaving the ISR 
}

#if 1
// *******************************************************************************
// Function Name  : USART5_IRQHandler
// Description    : This function handles USART1 global interrupt request.
// Input          : None
// Output         : None
// Return         : None
//*******************************************************************************
void UART5_IRQHandler(void)
{    
//	uint8 tmpdata, rx_data;
	OS_CPU_SR  cpu_sr;
	STM32F10x_UART_CHAN* pChan = &UARTChan[COM5];	
//	STM32F10x_UART_CHAN* pChan = GetDevChan(UART5);
		
	OS_ENTER_CRITICAL();                                       // Tell uC/OS-II that we are starting an ISR
	OSIntNesting++;
	OS_EXIT_CRITICAL();

	if(pChan != NULL)
	{
		UartDealInIRQ(pChan);
	} 	
	OSIntExit();                                                // Tell uC/OS-II that we are leaving the ISR 
}

#if defined (STM32F4XX)
// *******************************************************************************
// Function Name  : USART5_IRQHandler
// Description    : This function handles USART1 global interrupt request.
// Input          : None
// Output         : None
// Return         : None
//*******************************************************************************
void USART6_IRQHandler(void)
{    
//	uint8 tmpdata, rx_data;
	OS_CPU_SR  cpu_sr;
					STM32F10x_UART_CHAN* pChan = &UARTChan[COM6];	
//	STM32F10x_UART_CHAN* pChan = GetDevChan(USART6);
		
	OS_ENTER_CRITICAL();                                       // Tell uC/OS-II that we are starting an ISR
	OSIntNesting++;
	OS_EXIT_CRITICAL();

	if(pChan != NULL)
	{
		UartDealInIRQ(pChan);
	} 	
	OSIntExit();                                                // Tell uC/OS-II that we are leaving the ISR 
}
#endif
#endif
/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/

