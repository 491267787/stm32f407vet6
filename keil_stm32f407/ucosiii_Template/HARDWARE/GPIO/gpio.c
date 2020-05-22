#include "gpio.h"
//////////////////////////////////////////////////////////////////////////////////	 
//���ܣ����ư�����gpio���ܵĳ�ʼ��   
//���ߣ�zyf
//��������:2020/04/16
//�汾��V1.0								  
////////////////////////////////////////////////////////////////////////////////// 


const GPIO_INFO_T conLED_gpio[] = 
{
	{LED1_GPIO_PORT, LED1_GPIO_CLK, LED1_GPIO_PIN, GPIO_Speed_50MHz, GPIO_PuPd_NOPULL, GPIO_OType_PP}, /* CPU_RUN */
  {LED2_GPIO_PORT, LED2_GPIO_CLK, LED2_GPIO_PIN, GPIO_Speed_50MHz, GPIO_PuPd_NOPULL, GPIO_OType_PP}, /* BMS1_RUN */
  {LED3_GPIO_PORT, LED3_GPIO_CLK, LED3_GPIO_PIN, GPIO_Speed_50MHz, GPIO_PuPd_NOPULL, GPIO_OType_PP}, /* BMS2_RUN */
};

const GPIO_INFO_T conInput_gpio[] = 
{
	{INPUT0_GPIO_PORT,  INPUT0_GPIO_CLK,  INPUT0_GPIO_PIN,  GPIO_Speed_50MHz, GPIO_PuPd_NOPULL, GPIO_OType_OD},  /* ����1״̬ */
	{INPUT1_GPIO_PORT,  INPUT1_GPIO_CLK,  INPUT1_GPIO_PIN,  GPIO_Speed_50MHz, GPIO_PuPd_NOPULL, GPIO_OType_OD},  /* ����2״̬ */
	{INPUT2_GPIO_PORT,  INPUT2_GPIO_CLK,  INPUT2_GPIO_PIN,  GPIO_Speed_50MHz, GPIO_PuPd_NOPULL, GPIO_OType_OD},  /* ����3״̬ */
};


#define LED_IO_NUM        (sizeof(conLED_gpio) / sizeof(GPIO_INFO_T))
#define INPUT_NUM			    (sizeof(conInput_gpio) / sizeof(GPIO_INFO_T))  


/****************************************************************************************
** ��������: 
** ��������: led����
** �䡡��: led
** �䡡��: 
** ȫ�ֱ���:
** ����:   
******************************************************************************************/
void LED_Toggle(Led_TypeDef Led)
{
	static uint8_t fg[LED_IO_NUM] = {0};
	fg[Led] = ~fg[Led];
	GPIO_WriteBit(conLED_gpio[Led].ioport, conLED_gpio[Led].iopin, (BitAction)(fg[Led] & 0x01));
}

/****************************************************************************************
** ��������: 
** ��������: led����
** �䡡��: led
** �䡡��: 
** ȫ�ֱ���:
** ����:   
******************************************************************************************/
void LED_On(Led_TypeDef Led)
{
	GPIO_WriteBit(conLED_gpio[Led].ioport, conLED_gpio[Led].iopin, Bit_SET);
}

/****************************************************************************************
** ��������: 
** ��������: led����
** �䡡��: led
** �䡡��: 
** ȫ�ֱ���:
** ����:   
******************************************************************************************/
void LED_Off(Led_TypeDef Led)
{
	GPIO_WriteBit(conLED_gpio[Led].ioport, conLED_gpio[Led].iopin, Bit_RESET);
}

/****************************************************************************************
** ��������: 
** ��������: 
** �䡡��: 
** �䡡��: 
** ȫ�ֱ���:
** ����:   
******************************************************************************************/
static void LEDIOInit(Led_TypeDef Led)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
  
	RCC_APB1PeriphClockCmd(conLED_gpio[Led].iobusclk, ENABLE);

	GPIO_InitStructure.GPIO_Pin = conLED_gpio[Led].iopin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = conLED_gpio[Led].iospeed;
	GPIO_InitStructure.GPIO_OType = conLED_gpio[Led].ioOType;
	GPIO_InitStructure.GPIO_PuPd = conLED_gpio[Led].ioPuPd;

	GPIO_Init(conLED_gpio[Led].ioport, &GPIO_InitStructure);
	
}

/****************************************************************************************
** ��������: Led_all_Init(void)
** ��������: ��ʼ������led��gpoi
** �䡡��: 
** �䡡��: 
** ȫ�ֱ���:
** ����:   
******************************************************************************************/
static void Led_all_Init(void)
{
	uint8_t i;
	
	for( i = 0 ; i < LED_IO_NUM; i++ ) 
	{
		LEDIOInit((Led_TypeDef)i);
		GPIO_SetBits(conLED_gpio[i].ioport,conLED_gpio[i].iopin);   //���øߣ�����
	}	
}



/****************************************************************************************
** ��������: uint8 ReadInputDataBit(void)
** ��������: ��ȡ1· Input IO
** �䡡��: ���������
** �䡡��: ��Ӧͨ��IOֵ
** ȫ�ֱ���:
** ����:   ������Ŷ�ȡ��Ӧ������״̬
******************************************************************************************/
uint8_t ReadInputDataBit(Input_TypeDef inputn)
{
	return GPIO_ReadInputDataBit(conInput_gpio[inputn].ioport, conInput_gpio[inputn].iopin);
}


///****************************************************************************************
//** ��������: WriteLoutputDataBit(Loutput_TypeDef no,BitAction val)
//** ��������: 1·�����
//** �䡡��: no:���������
//			val:Bit_RESET = 0,
//  				Bit_SET	 = 1
//** �䡡��: ��
//** ȫ�ֱ���:
//** ����:   д��Ӧ���ֵ
//******************************************************************************************/
//void WriteLoutputDataBit(Loutput_TypeDef no, BitAction val)
//{
//	GPIO_WriteBit(conLoutput_gpio[no].ioport, conLoutput_gpio[no].iopin, val);
//}

/****************************************************************************************
** ��������: ����gpio��ʼ��
** ��������: 
** �䡡��:  				
** �䡡��: ��
** ȫ�ֱ���:
** ����:   
******************************************************************************************/
static void InputIoInit(Input_TypeDef inputn)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
  
	RCC_APB1PeriphClockCmd(conInput_gpio[inputn].iobusclk, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = conInput_gpio[inputn].iopin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = conInput_gpio[inputn].iospeed;
	GPIO_InitStructure.GPIO_OType = conInput_gpio[inputn].ioOType;
	GPIO_InitStructure.GPIO_PuPd = conInput_gpio[inputn].ioPuPd;
	
	GPIO_Init(conInput_gpio[inputn].ioport, &GPIO_InitStructure);
}

/*****************************************************************************************
** ��������:  Input_IO_Init(void)
** ��������: ���뿪������ʼ��
** �䡡��: ��
** �䡡��: ��
** ȫ�ֱ���:
** ����:  ��ʼ������
******************************************************************************************/
static void Input_IO_Init(void)
{
	uint8_t i = 0;
	
	for (i = 0; i < INPUT_NUM; i++)
	{
		InputIoInit((Input_TypeDef)i);
		GPIO_ResetBits(conInput_gpio[i].ioport, conInput_gpio[i].iopin);
	}	
}
/****************************************************************************************
** ��������: InPut_OutPut_Init(void)
** ��������: ��ʼ��gpio
** �䡡��: 
** �䡡��: ��
** ȫ�ֱ���:
** ����:   
******************************************************************************************/
void InPut_OutPut_Init(void)
{
	Led_all_Init();
	Input_IO_Init();
//	Loutput_IO_Init();
//	Joutput_IO_Init();
}


