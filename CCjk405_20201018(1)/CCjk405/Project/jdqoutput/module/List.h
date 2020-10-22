/**
  ******************************************************************************
  * @file    List.h
  * @author  zqj
  * @version v1.0
  * @date    2018-07-09
  * @brief   
  ******************************************************************************
*/
#ifndef __LIST_H_0001
#define __LIST_H_0001
#include "typedefs.h"
#include "memory.h"




#define  CMDSIZE  14     // > sizeof(CMD_LIST)
/*����һ������Ľṹ��ָ��*/
struct list {
	uint32 id;				      /*��ʶ���Ԫ�ط������*/
	DMEM * memptr;
	uint16 size;
	uint8 cmdst[CMDSIZE];		/*���������Ԫ��*/
	struct list *next;	/*ָ����һ�������ָ��*/
};


extern int list_add(struct list **head,uint16 nsize,void *node);

extern int list_add(struct list **head,uint16 nsize,void *node);

extern int list_del(struct list **head, int id);

extern int list_chg(struct list **head, int id, void *node);

extern int list_query(struct list **head, int id,void *node);






#endif

