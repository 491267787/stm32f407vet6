#include "usbh_usr.h" 
//#include "led.h"
#include "../../FATFS/src/ff.h" 
#include "../../FATFS/src/diskio.h" 
#include "usart.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//USBH-USR ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/7/22
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//*******************************************************************************
//�޸���Ϣ
//��
////////////////////////////////////////////////////////////////////////////////// 	   


static USBH_HOST  USB_Host;
static USB_OTG_CORE_HANDLE  USB_OTG_Core;


static u8 AppState;
//extern USB_OTG_CORE_HANDLE  USB_OTG_Core;



USBH_HOST *USBH_USR_GetUsbHost(void)
{
	return &USB_Host;
}

USB_OTG_CORE_HANDLE *USBH_USR_GetUsbOtgCore(void)
{
	return &USB_OTG_Core;
}



//USB OTG �жϷ�����
//��������USB�ж�
void OTG_FS_IRQHandler(void)
{
  	USBH_OTG_ISR_Handler(&USB_OTG_Core);
} 
//USB HOST �û��ص�����.
USBH_Usr_cb_TypeDef USR_Callbacks=
{
	USBH_USR_Init,
	USBH_USR_DeInit,
	USBH_USR_DeviceAttached,
	USBH_USR_ResetDevice,
	USBH_USR_DeviceDisconnected,
	USBH_USR_OverCurrentDetected,
	USBH_USR_DeviceSpeedDetected,
	USBH_USR_Device_DescAvailable,
	USBH_USR_DeviceAddressAssigned,
	USBH_USR_Configuration_DescAvailable,
	USBH_USR_Manufacturer_String,
	USBH_USR_Product_String,
	USBH_USR_SerialNum_String,
	USBH_USR_EnumerationDone,
	USBH_USR_UserInput,
	USBH_USR_MSC_Application,
	USBH_USR_DeviceNotSupported,
	USBH_USR_UnrecoveredError
};
/////////////////////////////////////////////////////////////////////////////////
//����Ϊ���ص�����ʵ��.

extern u8 exfuns_init(u8 disknum);
extern FATFS *exfuns_GetfsArea(u8 fsnum);
extern void DEBUG_Printf(char *s);
//USB HOST ��ʼ�� 
void USBH_USR_Init(void)
{
	FRESULT result;
	printf("USB OTG HS MSC Host\r\n");
	printf("> USB Host library started.\r\n");
	printf("  USB Host Library v2.1.0\r\n\r\n");
	
	if(1 == exfuns_init(USB_DISK))							//Ϊfatfs��ر��������ڴ�		
	{
		DEBUG_Printf("fatfs USB_Disk malloc false!");
		while(1);
	}
	result = f_mount(exfuns_GetfsArea(USB_DISK),"2:",1); 				//����U��
	if (result != FR_OK)
	{
		printf("�����ļ�ϵͳʧ�� (%d)\r\n", result);
		return;
	}
	//��ʼ��USB����
  USBH_Init(&USB_OTG_Core,USB_OTG_HS_CORE_ID,&USB_Host,&USBH_MSC_cb,&USR_Callbacks); 
	
}
//��⵽U�̲���
void USBH_USR_DeviceAttached(void)//U�̲���
{
//	LED1=1;
	printf("��⵽USB�豸����!\r\n");
}
//��⵽U�̰γ�
void USBH_USR_DeviceDisconnected (void)//U���Ƴ�
{
//	LED1=0;
	printf("USB�豸�γ�!\r\n");
}  
//��λ�ӻ�
void USBH_USR_ResetDevice(void)
{
	printf("��λ�豸...\r\n");
}
//��⵽�ӻ��ٶ�
//DeviceSpeed:�ӻ��ٶ�(0,1,2 / ����)
void USBH_USR_DeviceSpeedDetected(uint8_t DeviceSpeed)
{
	if(DeviceSpeed==HPRT0_PRTSPD_HIGH_SPEED)
	{
		printf("����(HS)USB�豸!\r\n");
 	}  
	else if(DeviceSpeed==HPRT0_PRTSPD_FULL_SPEED)
	{
		printf("ȫ��(FS)USB�豸!\r\n"); 
	}
	else if(DeviceSpeed==HPRT0_PRTSPD_LOW_SPEED)
	{
		printf("����(LS)USB�豸!\r\n");  
	}
	else
	{
		printf("�豸����!\r\n");  
	}
}
//��⵽�ӻ���������
//DeviceDesc:�豸������ָ��
void USBH_USR_Device_DescAvailable(void *DeviceDesc)
{ 
	USBH_DevDesc_TypeDef *hs;
	hs=DeviceDesc;   
	printf("VID: %04Xh\r\n" , (uint32_t)(*hs).idVendor); 
	printf("PID: %04Xh\r\n" , (uint32_t)(*hs).idProduct); 
}
//�ӻ���ַ����ɹ�
void USBH_USR_DeviceAddressAssigned(void)
{
	printf("�ӻ���ַ����ɹ�!\r\n");   
}
//��������������Ч
void USBH_USR_Configuration_DescAvailable(USBH_CfgDesc_TypeDef * cfgDesc,
                                          USBH_InterfaceDesc_TypeDef *itfDesc,
                                          USBH_EpDesc_TypeDef *epDesc)
{
	USBH_InterfaceDesc_TypeDef *id; 
	id = itfDesc;   
	if((*id).bInterfaceClass==0x08)
	{
		printf("���ƶ��洢���豸!\r\n"); 
	}else if((*id).bInterfaceClass==0x03)
	{
		printf("HID �豸!\r\n"); 
	}    
}
//��ȡ���豸Manufacturer String
void USBH_USR_Manufacturer_String(void *ManufacturerString)
{
	printf("Manufacturer: %s\r\n",(char *)ManufacturerString);
}
//��ȡ���豸Product String 
void USBH_USR_Product_String(void *ProductString)
{
	printf("Product: %s\r\n",(char *)ProductString);  
}
//��ȡ���豸SerialNum String 
void USBH_USR_SerialNum_String(void *SerialNumString)
{
	printf("Serial Number: %s\r\n",(char *)SerialNumString);    
} 
//�豸USBö�����
void USBH_USR_EnumerationDone(void)
{ 
	printf("�豸ö�����!\r\n\r\n");    
} 
//�޷�ʶ���USB�豸
void USBH_USR_DeviceNotSupported(void)
{
	printf("�޷�ʶ���USB�豸!\r\n\r\n");    
}  
//�ȴ��û����밴��,ִ����һ������
USBH_USR_Status USBH_USR_UserInput(void)
{ 
	printf("�����û�ȷ�ϲ���!\r\n");
	return USBH_USR_RESP_OK;
} 
//USB�ӿڵ�������
void USBH_USR_OverCurrentDetected (void)
{
	printf("�˿ڵ�������!!!\r\n");
} 


extern u8 exf_getfree(u8 *drv,u32 *total,u32 *free);
//�û�����������
//����ֵ:0,����
//       1,������
u8 USH_User_App(void)
{ 
	OS_ERR err;
	u32 total,free;
	u8 res=0;
//	Show_Str(30,140,200,16,"�豸���ӳɹ�!.",16,0);	 
	res=exf_getfree("2:",&total,&free);
	if(res==0)
	{
//		POINT_COLOR=BLUE;//��������Ϊ��ɫ	   
//		LCD_ShowString(30,160,200,16,16,"FATFS OK!");	
//		LCD_ShowString(30,180,200,16,16,"U Disk Total Size:     MB");	 
//		LCD_ShowString(30,200,200,16,16,"U Disk  Free Size:     MB"); 	    
//		LCD_ShowNum(174,180,total>>10,5,16); //��ʾU�������� MB
//		LCD_ShowNum(174,200,free>>10,5,16);	
	} 
 
	while(HCD_IsDeviceConnected(&USB_OTG_Core))//�豸���ӳɹ�
	{	
//		LED1=!LED1;
#if SYSTEM_SUPPORT_OS
    OSTimeDlyHMSM(0,0,0,200,OS_OPT_TIME_HMSM_STRICT,&err); 
#else
    delay_ms(200);
#endif		
	}
//	POINT_COLOR=RED;//��������Ϊ��ɫ	   
//	Show_Str(30,140,200,16,"�豸������...",16,0);
//	LCD_Fill(30,160,239,220,WHITE);
	return res;
} 


//USB HOST MSC���û�Ӧ�ó���
int USBH_USR_MSC_Application(void)
{
	u8 res=0;
  switch(AppState)
  {
    case USH_USR_FS_INIT://��ʼ���ļ�ϵͳ 
			printf("��ʼִ���û�����!!!\r\n");
			AppState=USH_USR_FS_TEST;
      break;
    case USH_USR_FS_TEST:	//ִ��USB OTG ����������
			res=USH_User_App(); //�û�������
     	res=0;
			if(res)
			{
				AppState=USH_USR_FS_INIT;
			}
      break;
    default:
			break;
  } 
	return res;
}
//�û�Ҫ�����³�ʼ���豸
void USBH_USR_DeInit(void)
{
  	AppState=USH_USR_FS_INIT;
}
//�޷��ָ��Ĵ���!!  
void USBH_USR_UnrecoveredError (void)
{
	printf("�޷��ָ��Ĵ���!!!\r\n\r\n");	
}
////////////////////////////////////////////////////////////////////////////////////////
//�û����庯��,ʵ��fatfs diskio�Ľӿں��� 
//extern USBH_HOST              USB_Host;

//��ȡU��״̬
//����ֵ:0,U��δ����
//      1,����
u8 USBH_UDISK_Status(void)
{
	return HCD_IsDeviceConnected(&USB_OTG_Core);//����U��״̬
}

//��U��
//buf:�����ݻ�����
//sector:������ַ
//cnt:��������	
//����ֵ:����״̬;0,����;����,�������;		 
u8 USBH_UDISK_Read(u8* buf,u32 sector,u32 cnt)
{
	u8 res=1;
	if(HCD_IsDeviceConnected(&USB_OTG_Core)&&AppState==USH_USR_FS_TEST)//���ӻ�����,����APP����״̬
	{  		    
		do
		{
			res=USBH_MSC_Read10(&USB_OTG_Core,buf,sector,512*cnt);
			USBH_MSC_HandleBOTXfer(&USB_OTG_Core ,&USB_Host);		      
			if(!HCD_IsDeviceConnected(&USB_OTG_Core))
			{
				res=1;//��д����
				break;
			};   
		}while(res==USBH_MSC_BUSY);
	}else res=1;		  
	if(res==USBH_MSC_OK)res=0;	
	return res;
}

//дU��
//buf:д���ݻ�����
//sector:������ַ
//cnt:��������	
//����ֵ:����״̬;0,����;����,�������;		 
u8 USBH_UDISK_Write(u8* buf,u32 sector,u32 cnt)
{
	u8 res=1;
	if(HCD_IsDeviceConnected(&USB_OTG_Core)&&AppState==USH_USR_FS_TEST)//���ӻ�����,����APP����״̬
	{  		    
		do
		{
			res=USBH_MSC_Write10(&USB_OTG_Core,buf,sector,512*cnt); 
			USBH_MSC_HandleBOTXfer(&USB_OTG_Core ,&USB_Host);		      
			if(!HCD_IsDeviceConnected(&USB_OTG_Core))
			{
				res=1;//��д����
				break;
			};   
		}while(res==USBH_MSC_BUSY);
	}else res=1;		  
	if(res==USBH_MSC_OK)res=0;	
	return res;
}



//USBHost������
void USBHost_task(void *p_arg)
{	
	OS_ERR err;
	p_arg = p_arg;
	
	
	USBH_USR_Init();
//	printf("USBHost_task start running!\n");	
	while(1)
	{
		USBH_Process(&USB_OTG_Core, &USB_Host);
		OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ100ms
	}
	
}







































