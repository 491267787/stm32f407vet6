#ifndef _DEBUG_H
#define _DEBUG_H

#include "sys.h"
#include "string.h"
#include "M_Global.h"

//////////////////////////////////////////////////////////////////////////////////	 
//���ܣ�   
//���ߣ�
//��������:2020/04/25
//�汾��V1.0									  
//////////////////////////////////////////////////////////////////////////////////

extern u8 debug_tcp_client_flag;
extern char DEBUG_Buff[50];
void DEBUG_Printf(char *s);
void DEBUG_Init(void);
#endif

