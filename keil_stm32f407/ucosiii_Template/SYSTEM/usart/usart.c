#include "sys.h"
#include "usart.h"	
#include "queue.h"	
//////////////////////////////////////////////////////////////////////////////////	 
//���ܣ����ư����д��ڹ��ܵĳ�ʼ��   
//���ߣ�
//��������:2020/04/29
//�汾��V1.0								  
////////////////////////////////////////////////////////////////////////////////// 


//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos ʹ��	  
#endif


STM32F10x_UART_CHAN UARTChan[N_UART_CHANNELS];



#if 1
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{ 	
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
	USART1->DR = (u8) ch;      
	return ch;
}
#endif
 
#if 0   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA=0;       //����״̬���	

//��ʼ��IO ����1 
//bound:������
void uart_init(u32 bound){
   //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //ʹ��GPIOAʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//ʹ��USART1ʱ��
 
	//����1��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); //GPIOA9����ΪUSART1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); //GPIOA10����ΪUSART1
	
	//USART1�˿�����
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //GPIOA9��GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOA,&GPIO_InitStructure); //��ʼ��PA9��PA10

   //USART1 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
  USART_Init(USART1, &USART_InitStructure); //��ʼ������1
	
  USART_Cmd(USART1, ENABLE);  //ʹ�ܴ���1 
	
	//USART_ClearFlag(USART1, USART_FLAG_TC);
	
#if EN_USART1_RX	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//��������ж�

	//Usart1 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//����1�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ�����

#endif

 /* for can't send first byte when hardware resetting*/
	USART_GetFlagStatus(USART1, USART_FLAG_TC);
}


void USART1_IRQHandler(void)                	//����1�жϷ������
{
	u8 Res;
#if SYSTEM_SUPPORT_OS 		//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntEnter();    
#endif
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		Res =USART_ReceiveData(USART1);//(USART1->DR);	//��ȡ���յ�������
		
		if((USART_RX_STA&0x8000)==0)//����δ���
		{
			if(USART_RX_STA&0x4000)//���յ���0x0d
			{
				if(Res!=0x0a)USART_RX_STA=0;//���մ���,���¿�ʼ
				else USART_RX_STA|=0x8000;	//��������� 
			}
			else //��û�յ�0X0D
			{	
				if(Res==0x0d)USART_RX_STA|=0x4000;
				else
				{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
					USART_RX_STA++;
					if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
				}		 
			}
		}   		 
  } 
#if SYSTEM_SUPPORT_OS 	//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntExit();  											 
#endif
} 
#endif	






uint32_t gUartRcvBuff0[512];
uint32_t gUartSndBuff0[1024/4];


uint32_t gUartRcvBuff[5][512/4];
uint32_t gUartSndBuff[5][400/4];

#define  MAX_BACK_RCVBUF_LENGTH  (512*4)
#define  MAX_BACK_SNDBUF_LENGTH  1024

#define 	MAX_UART_RCVBUF_LENTH    512    //   �����շ���������С
#define   MAX_UART_SNDBUF_LENTH    400



#define _IS_USER_FIFO_

uint8_t PowerOnForUart = FALSE;	
        
uint32_t UartHandler[] = {0,0,0,0,0,0};

USART_InitTypeDef USART_InitStructure;

STM32F10x_UART_CHAN UARTChan[N_UART_CHANNELS];

static char *UARTChanSemName[] = {"UARTChanSem0", "UARTChanSem1"};   //�����ź��������֣����ݴ��������

static STM32F10x_SIO_CHAN_PARAM UARTParameter[] = {	     //ע��:����2-3 ��ԭ��ͼ/PCB  ����� 
		{(void *)USART1_BASE, (void *)GPIOA, GPIO_Pin_3,  RS232TYPE, 115200,   USART_WordLength_8b, USART_StopBits_1, USART_Parity_No, HFC_NONE,UART_USE_TYPE_NOMAL},	
		{(void *)USART2_BASE, (void *)GPIOD, GPIO_Pin_7,  RS485TYPE, 2400,     USART_WordLength_8b, USART_StopBits_1, USART_Parity_No, HFC_NONE,UART_USE_TYPE_NOMAL},
//		{(void *)USART3_BASE, (void *)GPIOD, GPIO_Pin_10, RS485TYPE, 9600,     USART_WordLength_8b, USART_StopBits_1, USART_Parity_No, HFC_NONE,UART_USE_TYPE_NOMAL},	
//		{(void *)UART4_BASE,  (void *)GPIOG, GPIO_Pin_4,  RS232TYPE, 115200,   USART_WordLength_8b, USART_StopBits_1, USART_Parity_No, HFC_NONE,UART_USE_TYPE_NOMAL},
//		{(void *)UART5_BASE,  (void *)GPIOG, GPIO_Pin_5,  RS232TYPE, 115200,   USART_WordLength_8b, USART_StopBits_1, USART_Parity_No, HFC_NONE,UART_USE_TYPE_NOMAL},
//		{(void *)USART6_BASE, (void *)GPIOC, GPIO_Pin_8,  RS485TYPE, 2400,     USART_WordLength_8b, USART_StopBits_1, USART_Parity_No, HFC_NONE,UART_USE_TYPE_NOMAL},
		};


STM32F10x_UART_CHAN *GetChanHandler(uint8_t id)
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
int32_t Uart_Open(uint32_t deviceId)
{
  CPU_TS *p_ts = NULL;
  OS_ERR err;
	if(deviceId <= N_UART_CHANNELS)
	{
		OSSemPend(UARTChan[deviceId].uartLock, 0, OS_OPT_PEND_BLOCKING, p_ts, &err);   // ��ȡ�ź���   
			
		if(UARTChan[deviceId].deviceOpenFlag == FALSE)	   // δ��״̬ 
		{						
			UARTChan[deviceId].deviceOpenFlag = TRUE;  	   // ���Ϊ�Ѵ�
			Uart_EnableIsr(&UARTChan[deviceId]);   
			OSSemPost(UARTChan[deviceId].uartLock, OS_OPT_POST_1, &err);		
			return (int32_t)&UARTChan[deviceId];
		}else {
			OSSemPost(UARTChan[deviceId].uartLock, OS_OPT_POST_1, &err);	           // �ͷ��ź��� 		
			return (int32_t)&UARTChan[deviceId];
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
	uint32_t apbclock = 0, tmpreg = 0, integerdivider=0, fractionaldivider=0;
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
	tmpreg |= ((((fractionaldivider * 0x10) + 0x32) / 0x64)) & ((uint8_t_t)0x0F);	
	pbase->BRR = (uint16_t_t)tmpreg;		                                    // Write to USART BRR
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
	uint32_t tmpreg = pbase->CR1;
		
	if(pChan->WordLength == USART_WordLength_9b)				// byte size
		tmpreg |= (1<<12);                      // һ����ʼλ��9������λ��һ��ֹͣλ 	
	else
		tmpreg &= ~((uint32_t)(1<<12));				// һ����ʼλ��8������λ��n��ֹͣλ 
	if(pChan->Parity != USART_Parity_No)  // У����Ʊ�ʹ��
	{
		tmpreg |= (1<<10);	
		if(pChan->Parity == USART_Parity_Odd) // ��У��
		{
		 	tmpreg |= (1<<9);	
		}
		else
			tmpreg &= ~((uint32_t)(1<<9));		
	}
	else
		tmpreg &= ~((uint32_t)(1<<10));	
	pbase->CR1 = tmpreg;   

	tmpreg = pbase->CR2;
	tmpreg &= ~((uint32_t)(3<<12));	        // һ��ֹͣλ 
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
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

		/* Connect PXx to USARTx_Tx*/
		GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_USART2);
		
		/* Connect PXx to USARTx_Rx*/
		GPIO_PinAFConfig(GPIOD, GPIO_PinSource6,GPIO_AF_USART2);
		
		/* Configure USART Tx as alternate function  */
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOD, &GPIO_InitStructure);		 // GPIO�ܽ�����	  USART_TX	   PD5

		/* Configure USART Rx as alternate function  */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;		         // GPIO�ܽ�����	  USART_RX	PD6
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
#if 0	 
	 else if (pbase == (USART_TypeDef*)USART3_BASE)
	 {
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
	 }
	 else if (pbase == (USART_TypeDef*)UART4_BASE)
	 { 			
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
	 }
#endif
	
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
BOOL IsUartMode(uint8_t mode)
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
int Uart_IOCtrl(int32_t devDesc, int32_t cmd, void* arg)
{
	CPU_SR_ALLOC();
#if OS_CRITICAL_METHOD == 3
	OS_CPU_SR  cpu_sr;
#endif  
	int ret = FALSE;	
	OS_ERR err;
	CPU_TS *p_ts = NULL;
	STM32F10x_UART_CHAN* pChan = (STM32F10x_UART_CHAN*)devDesc;	 // ������Ϣ���ݽṹ 
	USART_TypeDef* pbase = (USART_TypeDef*)pChan->base;	         // �Ĵ�������ַ���ݽṹ 

	if((devDesc == 0) || (pbase == 0))
	{
		switch(cmd)  //���������Ϊ���ڲ�֪���豸�������¿����豸
		{
			case SIO_GET_PORT_HANDLE:
				OS_CRITICAL_ENTER();	//�����ٽ���
				if(*(uint32_t*)arg <= N_UART_CHANNELS)
				{		
					*(uint32_t*)arg = (int32_t)&UARTChan[*(uint32_t*)arg]; 
				}	
				else
				{
						*(uint32_t*)arg = NULL;
				}
				OS_CRITICAL_EXIT();	//�˳��ٽ���
				ret = TRUE;
				break;
			default:
				break;
		}
		return ret;
	}
	OSSemPend(pChan->uartLock, 0, OS_OPT_PEND_BLOCKING, p_ts, &err);
	switch(cmd)
	{
		case SIO_BAUD_SET:	                           // Set the baud rate 
		{
			OS_CRITICAL_ENTER();	//�����ٽ���
			pChan->baudRate = *(int32_t*)arg;				
			if(Uart_ParamSet(pChan) == TRUE)     // ���ò����� 
			{
				ret = TRUE;
			}
			OS_CRITICAL_EXIT();	//�˳��ٽ���		
		}
		break;
		case SIO_START:	                               // UART Start work
			OS_CRITICAL_ENTER();	//�����ٽ���
			Uart_Start(pbase);
			OS_CRITICAL_EXIT();	//�˳��ٽ���
			ret = TRUE;
			break;
		case SIO_HW_OPTS_SET:	                       // Set wordbits
			OS_CRITICAL_ENTER();	//�����ٽ���
			UartOptionSet(pChan, pbase);	
			OS_CRITICAL_EXIT();	//�˳��ٽ���
			ret = TRUE;
			break;
		case SIO_PARITY_BIT_SET:	                       // Parity bit set
			OS_CRITICAL_ENTER();	//�����ٽ���
			if(IS_USART_PARITY(*(int32_t*)arg))
			{
				pChan->Parity = *(int32_t*)arg;
				Uart_ParamSet(pChan);
			}	
			OS_CRITICAL_EXIT();	//�˳��ٽ���
			ret = TRUE;
			break;
		case SIO_STOP_BIT_SET:	                       // STOP bit set
			OS_CRITICAL_ENTER();	//�����ٽ���
			if(IS_USART_STOPBITS(*(int32_t*)arg))
			{
				pChan->Stopbits = *(int32_t*)arg;		 //	   pChan->Parity = *(int32*)arg;
				Uart_ParamSet(pChan);
			}	
			OS_CRITICAL_EXIT();	//�˳��ٽ���
			ret = TRUE;
			break;
		case SIO_DATA_BIT_SET:	                       // DATA bit set
			OS_CRITICAL_ENTER();	//�����ٽ���
			if(IS_USART_WORD_LENGTH(*(int32_t*)arg))
			{
				pChan->WordLength = *(int32_t*)arg;	//		pChan->Parity = *(int32*)arg;
				Uart_ParamSet(pChan);
			}	
			OS_CRITICAL_EXIT();	//�˳��ٽ���
			ret = TRUE;
			break;
		case SIO_WORK_TYPE_SET:	                      // type set(rs232 or rs485)
			OS_CRITICAL_ENTER();	//�����ٽ���
			if(IsUartMode(*(int*)arg))
			{
				pChan->devicType = *(int*)arg;
				UartTypeSet(pChan, pbase);
				ret = TRUE;
			}
			OS_CRITICAL_EXIT();	//�˳��ٽ���		
			break;
		case SIO_SEND_STATUS_GET:	                      // ��÷�����״̬
			OS_CRITICAL_ENTER();	//�����ٽ���
			*(int*)arg = pChan->SendStatus;	  //type info UART_SND_STA
			OS_CRITICAL_EXIT();	//�˳��ٽ���			
			break;
		case SIO_SEND_BUF_FULL_STATUS_GET: /*��÷��ͻ�����״̬*/
			OS_CRITICAL_ENTER();	//�����ٽ���
			if(arg != NULL)
			{
			    if(IsQueueFull(pChan->SendBuf) == FALSE)
				{//����
					*(uint32_t*)arg = UART_SEND_BUF_NO_FULL;			
				}else{ //��
					*(uint32_t*)arg = UART_SEND_BUF_FULL;
				}
				ret = TRUE;
			}
			OS_CRITICAL_EXIT();	//�˳��ٽ���
			break;	
		case SIO_USE_TYPE_SET:	                      // ����ʹ������
			OS_CRITICAL_ENTER();	//�����ٽ���
			if(IS_VALID_USE_TYPE(*(int*)arg))
			{
				pChan->useType = (UART_USE_TYPE)(*(int*)arg);	  
			}
			OS_CRITICAL_EXIT();	//�˳��ٽ���			
			break;
		case SIO_USE_TYPE_GET:	                      // ���ʹ������
			OS_CRITICAL_ENTER();	//�����ٽ���
			*(int*)arg = pChan->useType;	  
			OS_CRITICAL_EXIT();	//�˳��ٽ���			
			break;
		case SIO_GPS_SYN_TIME_GET:	                      // ���GPSͬ��ʱ��ʱ��
			OS_CRITICAL_ENTER();	//�����ٽ���
			*(uint32_t*)arg = pChan->Timegps.RevGpsSynTime;	  
			OS_CRITICAL_EXIT();	//�˳��ٽ���			
			break;
		default:
			break;
	}
	OSSemPost(pChan->uartLock, OS_OPT_POST_1, &err);
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
int32_t Uart_Close(uint32_t devDesc)		//device description
{
	int8_t i;
	OS_ERR err;
	CPU_TS *p_ts = NULL;
	for(i = 0;i < N_UART_CHANNELS; i++)
	{
		if(devDesc == (uint32_t)&UARTChan[i])
		{
			OSSemPend(UARTChan[i].uartLock, 0, OS_OPT_PEND_BLOCKING, p_ts, &err);	
			UARTChan[i].deviceOpenFlag = FALSE;
			Uart_DisableIsr(&UARTChan[i]);
			OSSemPost(UARTChan[i].uartLock, OS_OPT_POST_1, &err);			
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
int32_t Uart_Read(int32_t devDesc,uint8_t *buf, uint16_t maxSize)
{
	uint16_t ret = 0;
	uint16_t length;
	STM32F10x_UART_CHAN* pChan = (STM32F10x_UART_CHAN*)devDesc;
	if(devDesc == 0)
	{
		return 0;
	}
	if(devDesc == (uint32_t)&UARTChan[BACK_COM]) {
		length = MAX_BACK_RCVBUF_LENGTH;
	}else {
		length = MAX_UART_RCVBUF_LENTH;
	}
	for(ret = 0; ((ret < length) && (ret < maxSize)); ret++)	           // ȡ���� 
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
int32_t Uart_ClearReadBuf(int32_t devDesc)
{
	uint16_t cnt = 0;
	int32_t ret;
	uint8_t buf[50] ;
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
uint16_t Uart_GetBytesNumInBuff(int32_t devDesc)
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
int32_t Uart_Write(int32_t devDesc,	uint8_t *buf,	int32_t size)
{
	int delayMax = 200;
	OS_ERR err;
	CPU_TS *p_ts = NULL;
	int len = size;
	int32_t temp;
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
	OSSemPend(pChan->uartLock, 0, OS_OPT_PEND_BLOCKING, p_ts, &err);	
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
				OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ10ms
			}				 				
		}while((--delayMax <= 0) && (len>=0));		
	}
	OSSemPost(pChan->uartLock, OS_OPT_POST_1, &err);
	delayMax = 200;
	while(--delayMax)
	{
		OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ10ms
		if((len = QueueNData(pChan->SendBuf)) == 0)
			break;
	}
	temp = (int32_t)(size - len);
	return temp;
}
//===================================================================
// ����ʱ��ʹ�� 
//
//===================================================================
void UsartClockOperation(STM32F10x_UART_CHAN *pChan, uint8_t cmd)
{
	uint8_t i;
	for(i = 0;i < N_UART_CHANNELS; i++)
	{
		if(pChan == &UARTChan[i])  
		{
			if(i == 0)
			{
				if(cmd != DISABLE)	  // USART1ʱ��ʹ��
					RCC->APB2ENR |= (1<<14);	
				else
					RCC->APB2ENR &= ~((uint32_t)(1<<14));				
			}
			else	// usart 2-5
			{
				if(cmd != DISABLE)	  // USART1ʱ��ʹ��
					RCC->APB1ENR |= (1<<(16+i));	
				else
					RCC->APB1ENR &= ~((uint32_t)(1<<(16+i)));				
					
			}
		} 	
	}
}

/*************************************************
LPCUartDevInit - initialize  a  UART .
RETURN: N
*************************************************/
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

	if(pbase == (USART_TypeDef*)USART1_BASE)                     // UART0,(PA10 USART1_RX,PA9 USART1_TX) 
	{		
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); 	 // �������� USART����ʱ�� 
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);	  	     // ���� ����NVIC  Ӧ���ж��븴λ���ƼĴ���   ����Ӧ�����ȼ����з���
		NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;	     // �жϺ� 37  
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	     // �����ȼ� 0 
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	         // ʹ�ܴ���1�ж� 
		NVIC_Init(&NVIC_InitStructure);	                         // �����ж����ȼ���ʹ���ж� 						
		
		USART_Init(USART1, &USART_InitStructure);				 // ���ڲ������� 
 		 
		USART_ITConfig(USART1, USART_IT_TXE, ENABLE);	         // ������ж�����-���ͻ����������Ѿ�����
		USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);	         // ������ж�����-���ջ������յ����� 
				  		
		USART_Cmd(USART1, ENABLE);	                             // USARTʹ�� USART_CR1 bit13
	}
	else if(pbase == (USART_TypeDef*)USART2_BASE)
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); 	 // �������� USART����ʱ�� 

		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);	  	     // ���� ����NVIC  Ӧ���ж��븴λ���ƼĴ���   ����Ӧ�����ȼ����з���
		NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;	     // �жϺ� 38  
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;	     // �����ȼ� 1 
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	         // ʹ�ܴ����ж� 
		NVIC_Init(&NVIC_InitStructure);	                         // �����ж����ȼ���ʹ���ж� 		
		 
		USART_Init(USART2, &USART_InitStructure);				 // ���ڲ�������
		USART_ITConfig(USART2, USART_IT_TXE, ENABLE); 	         // ������ж�����-���ͻ����������Ѿ�����
		USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);	         // ������ж�����-���ջ������յ����� 		
		USART_Cmd(USART2, ENABLE);		                         // USARTʹ��		
	}
#if 0	
	else if(pbase == (USART_TypeDef*)USART3_BASE)
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE); 	 // �������� USART����ʱ�� 

		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);	  	     // ���� ����NVIC  Ӧ���ж��븴λ���ƼĴ���   ����Ӧ�����ȼ����з���
		NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;	     // �жϺ� 39  
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;	     // �����ȼ� 
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	         // ʹ�ܴ����ж� 
		NVIC_Init(&NVIC_InitStructure);	                         // �����ж����ȼ���ʹ���ж� 		
		
		USART_Init(USART3, &USART_InitStructure);				 // ���ڲ������� 
		 
		USART_ITConfig(USART3, USART_IT_TXE, ENABLE);	         // ������ж�����-���ͻ����������Ѿ�����
		USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);	         // ������ж�����-���ջ������յ����� 
				
		USART_Cmd(USART3, ENABLE);								 // USARTʹ��
	}	
	else if(pbase == (USART_TypeDef*)UART4_BASE)
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE); 	 // �������� UART����ʱ�� 

		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);	  	     // ���� ����NVIC  Ӧ���ж��븴λ���ƼĴ���   ����Ӧ�����ȼ����з���
		NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;	     // �жϺ� 52  
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;	     // �����ȼ� 
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	         // ʹ�ܴ����ж� 
		NVIC_Init(&NVIC_InitStructure);	                         // �����ж����ȼ���ʹ���ж� 		

		 
		USART_Init(UART4, &USART_InitStructure);				 // ���ڲ�������

		USART_ITConfig(UART4, USART_IT_TXE, ENABLE);	         // ������ж�����-���ͻ����������Ѿ�����
		USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);	         // ������ж�����-���ջ������յ����� 
		
 		USART_ClearFlag(pChan->base,USART_FLAG_TXE);
		
		USART_Cmd(UART4, ENABLE);						         // USARTʹ��
	}	
	else if(pbase == (USART_TypeDef*)UART5_BASE)
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE); 	 // �������� UART����ʱ�� 

		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);	  	     // ���� ����NVIC  Ӧ���ж��븴λ���ƼĴ���   ����Ӧ�����ȼ����з���
		NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;	     // �жϺ� 53  
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;	     // �����ȼ� 
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	         // ʹ�ܴ����ж� 
		NVIC_Init(&NVIC_InitStructure);	                         // �����ж����ȼ���ʹ���ж� 		
		 
		USART_Init(UART5, &USART_InitStructure);				 // ���ڲ�������

		USART_ITConfig(UART5, USART_IT_TXE, ENABLE);	         // ������ж�����-���ͻ����������Ѿ�����
		USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);	         // ������ж�����-���ջ������յ����� 
 		
		USART_Cmd(UART5, ENABLE);						         // USARTʹ��
	}
#if defined (STM32F4XX)
	else if(pbase == (USART_TypeDef*)USART6_BASE)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE); 	 // �������� USART����ʱ�� 
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);	  	     // ���� ����NVIC  Ӧ���ж��븴λ���ƼĴ���   ����Ӧ�����ȼ����з���
		NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;	     // �жϺ� 37  
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	     // �����ȼ� 0 
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





/********************************************************************************
// Function Name  : GetDevChan()
// Description    : This function get device object.
// Input          : dev address
// Output         : None
// Return         : device struct
*********************************************************************************/
STM32F10x_UART_CHAN *GetDevChan(USART_TypeDef* uart)
{
	uint8_t i;
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
	uint8_t tmpdata,rx_data;

	if (USART_GetFlagStatus(pChan->base, USART_FLAG_RXNE) != RESET)  // ���ջ������ǿ� 
	{
		rx_data = (uint8_t)USART_ReceiveData(pChan->base);
		QueueWriteInInt(pChan->RcvBuf, &rx_data); 
		// дһ���ֽڵ�����  �˴��ݲ�������� ������ݲ��ٽ��� 				
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
//	uint8_t tmpdata,rx_data;
	CPU_SR_ALLOC();
	uint32_t regsr, regcr1;
//	OS_CPU_SR  cpu_sr;
	STM32F10x_UART_CHAN* pChan = GetDevChan(USART1);	
	OS_CRITICAL_ENTER();	//�����ٽ���                                      // Tell uC/OS-II that we are starting an ISR
	OSIntNestingCtr++;
	OS_CRITICAL_EXIT();	//�˳��ٽ���

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
//	uint8_t tmpdata,rx_data;
	CPU_SR_ALLOC();
	STM32F10x_UART_CHAN* pChan = GetDevChan(USART2);
		
	OS_CRITICAL_ENTER();	//�����ٽ���                                      // Tell uC/OS-II that we are starting an ISR
	OSIntNestingCtr++;
	OS_CRITICAL_EXIT();	//�˳��ٽ���

	if(pChan != NULL)
	{
		UartDealInIRQ(pChan);
	} 	
	OSIntExit();                                                // Tell uC/OS-II that we are leaving the ISR 
}
// *******************************************************************************
// Function Name  : USART3_IRQHandler
// Description    : This function handles USART1 global interrupt request.
// Input          : None
// Output         : None
// Return         : None
//*******************************************************************************
void USART3_IRQHandler(void)
{    
//	uint8_t tmpdata, rx_data;
	CPU_SR_ALLOC();
	STM32F10x_UART_CHAN* pChan = GetDevChan(USART3);
		
	OS_CRITICAL_ENTER();	//�����ٽ���                                      // Tell uC/OS-II that we are starting an ISR
	OSIntNestingCtr++;
	OS_CRITICAL_EXIT();	//�˳��ٽ���

	if(pChan != NULL)
	{
		UartDealInIRQ(pChan);
	} 	
	OSIntExit();                                                // Tell uC/OS-II that we are leaving the ISR 
}

// *******************************************************************************
// Function Name  : USART4_IRQHandler
// Description    : This function handles USART1 global interrupt request.
// Input          : None
// Output         : None
// Return         : None
//*******************************************************************************
void UART4_IRQHandler(void)
{    
//	uint8_t tmpdata, rx_data;
	CPU_SR_ALLOC();
	STM32F10x_UART_CHAN* pChan = GetDevChan(UART4);
		
	OS_CRITICAL_ENTER();	//�����ٽ���                                      // Tell uC/OS-II that we are starting an ISR
	OSIntNestingCtr++;
	OS_CRITICAL_EXIT();	//�˳��ٽ���

	if(pChan != NULL)
	{
		UartDealInIRQ(pChan);
	} 	
	OSIntExit();                                                // Tell uC/OS-II that we are leaving the ISR 
}

// *******************************************************************************
// Function Name  : USART5_IRQHandler
// Description    : This function handles USART1 global interrupt request.
// Input          : None
// Output         : None
// Return         : None
//*******************************************************************************
void UART5_IRQHandler(void)
{    
//	uint8_t tmpdata, rx_data;
	CPU_SR_ALLOC();
	STM32F10x_UART_CHAN* pChan = GetDevChan(UART5);
		
	OS_CRITICAL_ENTER();	//�����ٽ���                                      // Tell uC/OS-II that we are starting an ISR
	OSIntNestingCtr++;
	OS_CRITICAL_EXIT();	//�˳��ٽ���

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
//	uint8_t tmpdata, rx_data;
	CPU_SR_ALLOC();
	STM32F10x_UART_CHAN* pChan = GetDevChan(USART6);
		
	OS_CRITICAL_ENTER();	//�����ٽ���                                      // Tell uC/OS-II that we are starting an ISR
	OSIntNestingCtr++;
	OS_CRITICAL_EXIT();	//�˳��ٽ���

	if(pChan != NULL)
	{
		UartDealInIRQ(pChan);
	} 	
	OSIntExit();                                                // Tell uC/OS-II that we are leaving the ISR 
}
#endif




// ******************************************************************************
// sysSerialHwInit - initialize  Serial Devece struct and Hardware at first.
// RETURN: NONE
// ******************************************************************************
void sys_SerialHwInit(void)
{
	uint8_t i;
	OS_ERR err;
	if(PowerOnForUart == FALSE)
	{
		PowerOnForUart = TRUE;
		for(i = 0;i < N_UART_CHANNELS; i++)		 
		{
			OSSemCreate(UARTChan[i].uartLock, UARTChanSemName[i], 1, &err);	      // �����ź���  ��ʼ��Ϊ�Ѵ�  
			UARTChan[i].base = UARTParameter[i].baseAddr;                // �Ĵ�������ַ 
			UARTChan[i].directionGPIO =  UARTParameter[i].directionGPIO; // �������GPIO 
			UARTChan[i].rs485ContrlPin = UARTParameter[i].rs485ContrlPin;// ������ƽ���� 
			UARTChan[i].deviceID = i+1;                                  // ���� ID
			UARTChan[i].deviceOpenFlag = FALSE;	                         // �򿪱�־��ʼ��Ϊ δ�� 
			UARTChan[i].devicType = UARTParameter[i].devicType;	         // �豸���� 0 rs232 1 rs485

			UARTChan[i].baudRate = UARTParameter[i].defBuardRate;	     // ������ 
			UARTChan[i].WordLength = UARTParameter[i].defWordLength;	 // Ĭ������λ 5-8 
			UARTChan[i].Stopbits = UARTParameter[i].defStopbits;	     // Ĭ��ֹͣλ 1-2  
			UARTChan[i].Parity = UARTParameter[i].defParity;	         // Ĭ��У��λ
			UARTChan[i].SendStatus =  UART_SEND_IDLE;
			UARTChan[i].useType =  UARTParameter[i].useType;	         // ʹ���豸����	 
			memset(&UARTChan[i].Timegps,0,sizeof(gpsuart_t)); 
			if( i == BACK_COM ) {
				UARTChan[BACK_COM].RcvBuf =  (uint8_t *)&gUartRcvBuff0[0];
				UARTChan[BACK_COM].SendBuf = (uint8_t*)&gUartSndBuff0[0];
				QueueCreate(UARTChan[i].RcvBuf, MAX_BACK_RCVBUF_LENGTH,1,0,0);     // �������ݶ��� 
				QueueCreate(UARTChan[i].SendBuf, MAX_BACK_SNDBUF_LENGTH,1,0,0);    // �������ݶ��� 		
			}else {
				UARTChan[i].RcvBuf  = (uint8_t *)&gUartRcvBuff[i-1][0];
				UARTChan[i].SendBuf = (uint8_t*)&gUartSndBuff[i-2][0];
				QueueCreate(UARTChan[i].RcvBuf, MAX_UART_RCVBUF_LENTH,1,0,0);     // �������ݶ��� 
				QueueCreate(UARTChan[i].SendBuf, MAX_UART_SNDBUF_LENTH,1,0,0);    // �������ݶ��� 	
			}
			UartDevInit(&UARTChan[i]);
		}
	}
}

/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/




