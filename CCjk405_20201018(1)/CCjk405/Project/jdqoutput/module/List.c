/**
  ******************************************************************************
  * @file    List.c
  * @author  zqj
  * @version v1.0
  * @date    2018-07-09
  * @brief   
  ******************************************************************************
*/

/* ?????? */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "List.h"


 
/*�����������ͷ��*/
//static struct list *list_head = NULL;
 
//static int list_id = 0;
 
/**	��ָ��Ԫ�ز��뵽����β��
  * 	head	: ��ʾҪ����Ԫ�ص������ͷ��ַ
  *	list    : ��ʾҪ���뵽�����е�Ԫ��
	* ���ز����id,ʧ��Ϊ-1
 */

int list_add(struct list **head,uint16 nsize,void *node)
{
	struct list *temp,*cmdstptr;
	DMEM *dynMemory = DynMemGet(sizeof(struct list)); 
	if( NULL == dynMemory) {
		return -1;
	}
	cmdstptr = dynMemory->addr;
	memset(cmdstptr,0,sizeof(struct list));
	
	cmdstptr->memptr = dynMemory;
	if( NULL == node )
	{
		return -1;
	}
	
  nsize = nsize > CMDSIZE ? CMDSIZE : nsize;
	cmdstptr->size = nsize;
	//memcpy(cmdstptr->cmdst,node,nsize);
	/* �ж������Ƿ�Ϊ�� */
	if(NULL == *head)
	{
		/*Ϊ��*/
		*head = cmdstptr;
		 cmdstptr->id = 1;
		 memcpy(cmdstptr->cmdst,node,cmdstptr->size);
		(*head)->next = NULL;
	}
	else
	{
		/*��Ϊ��*/
		temp = *head;
		while(temp)
		{
			if(NULL == temp->next)
			{
				cmdstptr->id = (temp->id + 1 );
				memcpy(cmdstptr->cmdst,node,cmdstptr->size);
				temp->next = cmdstptr;
				cmdstptr->next = NULL;
				break;
			}
			temp = temp->next;
		}
	}
	return cmdstptr->id;
}

 
///**	ָ��Ԫ�ش�����β��ɾ��
//  * 	head	: ��ʾҪɾ��Ԫ�ص�����ͷ����ַ
//  *	  id    :  ��ʾҪɾ��Ԫ�صı�ʶ
//  *	����ֵ  : 0-�ɹ�,-1-ʧ��
//  */
int list_del(struct list **head, int id)
{
	struct list *temp, *p;
	temp = *head;
 
	if(NULL == temp)
	{
		/*����Ϊ��*/
		//printf("����Ϊ��!\n");
		return -1;
	}
	else
	{
		/*�ж�ƥ���Ԫ���Ƿ�Ϊ����ͷ����Ԫ��*/
		if(id == temp->id)		/*������ͷ��*/
		{
			DynMemPut(temp->memptr);
			temp->memptr = NULL;
			*head = temp->next;
			return 0;
		}
		else					/*��������ͷ��*/
		{
			while(temp->next)
			{
				p = temp;
				temp = temp->next;
 
				if(id == temp->id)
				{
					DynMemPut(temp->memptr);
					temp->memptr = NULL;
					p->next = temp->next;
					return 0;
				}
			}	
			return -1;
		}
	}
}
 
/**��ָ����id��Ԫ������������ݽ����޸�
  * 	head	: ��ʾҪ�ı�Ԫ�ص������ͷ���ĵ�ַ
  *	  id    : ��ʾҪ�ı�Ԫ�صı�ʶ
  *	content : ��ʾҪ�ı������
  *	����ֵ  : 0-�ɹ�,-1-ʧ��
  */
int list_chg(struct list **head, int id, void *node)
{
	struct list *temp;
 
	temp = *head;	/*�������ͷ����ֵ����ʱ ����*/
 
	while(temp)		/* �����������ѯ */
	{
		if(id == temp->id)
		{
			memset(temp->cmdst, 0, sizeof(temp->cmdst));
			memcpy(temp->cmdst,node,temp->size);
			return 0;
		}
		temp = temp->next;
	}
	return -1;
}

/**��ָ��id��Ԫ������������ݽ��в���
  * 	head	: ��ʾҪ��ѯ��Ԫ�ص������ͷ����ַ
  *	  id    : ��ʾҪ��ѯ��Ԫ�صı�ʶ
  *	����ֵ  : 0-�ɹ�,-1-ʧ��
*/
int list_query(struct list **head, int id,void *node)
{
	struct list *temp;
 
	temp = *head;	
 
	while(temp)
	{
		if(id == temp->id)
		{
			if( NULL != node ) {
				memcpy(node,temp->cmdst,temp->size);
			}
			return 0;
		}
		temp = temp->next;
	}
 
	return -1;
}


