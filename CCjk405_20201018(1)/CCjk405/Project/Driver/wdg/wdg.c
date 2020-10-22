
#include "wdg.h"

//�ⲿ���Ź���ʼ��
void OutWatchdogInit()
{
	GPIO_InitTypeDef  GPIO_InitStructure;
#if defined (STM32F4XX)
	  /* Enable the GPIO_LED Clock */
	  RCC_AHB1PeriphClockCmd(OUTWATCHDOG_GPIO_CLK, ENABLE);
	  	
	  /* Configure the GPIO_LED pin */
	  GPIO_InitStructure.GPIO_Pin = OUTWATCHDOG_GPIO_PIN;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	  GPIO_Init(OUTWATCHDOG_GPIO_PORT, &GPIO_InitStructure);
#endif
}

// ==================== ���ڿ��Ź� ================================================================================
// ���÷�Χ��Ҫ���Ź��ھ�ȷ��ʱ���������õ�Ӧ�ó���
// �ڴ��ڿ��Ź��ж���ι�� 
void sysWWdgInit(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

//patli 20191205  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  NVIC_InitStructure.NVIC_IRQChannel = WWDG_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE); // �������ڿ��Ź�����ʱ��
	WWDG_SetPrescaler(WWDG_Prescaler_8);	               // ����Ƶ�� = (PCLK1/4096)/8 = 36M/4096/8 = 1099HZ  
	WWDG_SetWindowValue(65);	                           //  Set Window value to 65
	WWDG_Enable(127);		                                 // ����Ź������ó�ֵΪ127�ݼ���63ʱ���Ź�������λ
	WWDG_ClearFlag();	                                   // �����ǰ�����жϱ�־
	WWDG_EnableIT();		                                 // ���ж� 

	OutWatchdogInit();
}

// ===================== ���ڿ��Ź� end ============================================================================

// ===================== �������Ź� ================================================================================
// ʹ�÷�Χ����������֮�⣬�ܹ���ȫ�������������Ҷ�ʱ�侫��Ҫ��ϵ͵ĳ���
// ��SysTick_Handler()��ι�� 

void sysIWdgInit(void)
{
	uint16 timeout = 0, tmp;
	uint8 i = 0xff;

	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);		 // ����IWDG_KR�Ĵ�����д��0x5555�����д���� 
	IWDG_SetPrescaler(IWDG_Prescaler_128);	           // ������ʱ�ӵ�Ԥ��Ƶ����32	IWDG counter clock=40KHz(LSI) / 32 = 1.25 KHz

	timeout = IWDG_TIMEOUT;
	if((IWDG_TIMEOUT < 1000) || (IWDG_TIMEOUT > 4000))
		timeout = 3000;                                // Ĭ�� 12s	
	tmp =  (uint16)(timeout*1.25);
	IWDG_SetReload(tmp);	                           // ���Ź���������װ��ֵ

	IWDG_ReloadCounter();			                       // ��װ����ֵ 
	IWDG_Enable();		                               // д��0xCCCC���������Ź�����
//	
//	RCC_LSICmd(ENABLE);//��LSI  

//	OutWatchdogInit();
}


//�ⲿӲ�����Ź�
void FeedOutWatchdog(void)
{
	OUTWATCHDOG_GPIO_PORT->ODR ^= OUTWATCHDOG_GPIO_PIN;//��򣬽�1��Ϊ0?0��Ϊ1������˸
}

//***���ڿ��Ź�ι��
void FeedWWdog(void)
{
#ifdef WATCH_DOG_ENABLE
	IWDG_ReloadCounter();
#endif
//	FeedOutWatchdog();
}

void Wdg_feeddog(void)
{
  IWDG_ReloadCounter();
}
