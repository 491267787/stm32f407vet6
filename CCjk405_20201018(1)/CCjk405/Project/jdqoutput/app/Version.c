//version.c
#include "M_Global.h"
#include "wdg.h"
#include "TaskIsoCheck.h"
#include "BillingUnit.h"

//#if	( SUPPORT_ONLINE_CARD == ONLINE_CARD )
//	#warning  "Support Online Card!"
//#elif (SUPPORT_ONLINE_CARD == OFFLINE_CARD )
//	#warning "Support Offline Card!"
//#endif

//#if ( SCREEN_CHOICE == _43SCREEN  )
//	  #warning "Support 4.3 SCREEN!"
//#elif (SCREEN_CHOICE == _07SCREEN)
//	  #warning "Support 7 SCREEN!"
//#endif
	
//#if (SUPPORT_2DMBARCODE == _2DMBARCODE_SUPPORT)
//	   #warning "Support 2 DMBARCODE"!
//#else
//	   #warning "Don't Support 2 DMBARCODE!"
//#endif

#if (SUPPORT_GUN_START == GUNSTART_SUPPORT)
 #warning "Support gun start!"
#elif (SUPPORT_GUN_START == GUNSTART_DONTSUPPORT)
 #warning "Don't Support gun start!"
#elif (SUPPORT_GUN_START == GUNSTART_VIN_SUPPORT)
 #warning "Support che VIN start!"
#endif

#if ((WATCHDOG_ENABLE == WDG_ENABLE )&& CFG_IWDG_EN )
	#warning "Watchdog is Enable!"
#else
  #warning "Watchdog is Disabled!"
#endif

#if (PRODUCTS_LEVEL ==  DEBUG_VERSION )
	#warning "*******Products is Debug Level!******"
#elif(PRODUCTS_LEVEL ==  FACTORY_UPGRADE_VERSION )
  	#warning "Products is Upgrade Level!"
#else
	#warning "Products is Release Level!"
#endif

#if  (ISO_TYPE == WBISO_SINGLE )
  #warning "*******Τ����Ե�У���ɫ��******"
#elif (ISO_TYPE == ISO_SINGLE )
  #warning "*******������Ե�У���ɫ��*****"
#elif (ISO_TYPE == ISO_AUNICE )
  #warning "*******����˫·��Ե�У���ɫ��*****"
#endif

#if (BILL_PRICE_BASE ==  BILL_PRICE_YIDIAN )
 #warning "*******���ʾ���0.01!******"
#elif (BILL_PRICE_BASE ==  BILL_PRICE_HECI )
  #warning "19-03-29�����ʾ���0.0001!"
#else 
  #warning "��ȷ�����ʾ���!"
#endif



