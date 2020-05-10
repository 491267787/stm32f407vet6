#include "sys.h"
#include "delay.h"
#include "usart.h"


#include "gpio.h"
#include "timer.h"

#include "MainData.h"
#include "RealTimeCheck.h"
//////////////////////////////////////////////////////////////////////////////////	 
//���ܣ�������Ƶ�Ԫ(ȫ�ֱ����Ķ��弰��ȡ�ӿڡ��������̽ӿ�)   
//���ߣ�zyf
//��������:2020/04/16
//�汾��V1.0								  
////////////////////////////////////////////////////////////////////////////////// 

PARAM_OPER_TYPE gRunParam;

PARAM_DEV_TYPE gDevParam;

CHARGE_TYPE gPtrRunData;



PARAM_OPER_TYPE *ChgData_GetRunParamPtr(void)
{
	 return &gRunParam;
}

PARAM_DEV_TYPE *ChgData_GetDevParamPtr(void)
{
	 return &gDevParam;
}

CHARGE_TYPE *ChgData_GetRunDataPtr(void)
{
	 return &gPtrRunData;
}


void MainCtrlUnit_Init(void)
{
	gPtrRunData.input = Check_GetInputDataptr();
	gPtrRunData.dev_status = Check_GetStatusDataptr();
}
