/****************************************Copyright (c)**************************************************

********************************************************************************************************/
#include "queue.h"
#include "string.h"

/*********************************************************************************************************
** ��������: QueueCreate
** ��������: ��ʼ�����ݶ���
** �䡡��: Buf      ��Ϊ���з���Ĵ洢�ռ��ַ
**         SizeOfBuf��Ϊ���з���Ĵ洢�ռ��С���ֽڣ�
**			dataSize: ����ÿ�δ������ݴ�С ADD
**         ReadEmpty��Ϊ���ж���ʱ�������
**         WriteFull��Ϊ����д��ʱ�������
** �䡡��: NOT_OK:��������
**         QUEUE_OK:�ɹ�
** ȫ�ֱ���: ��
** ����ģ��: OS_ENTER_CRITICAL,OS_EXIT_CRITICAL
**-------------------------------------------------------------------------------------------------------
** �޸���:
** �ա���:
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
uint8_t QueueCreate(void *Buf,
                          uint32_t SizeOfBuf,
						  uint16_t dataSize,
                          uint8_t (* ReadEmpty)(),
                          uint8_t (* WriteFull)()
                          )
{
 
	  CPU_SR_ALLOC();
    DataQueue *Queue;
    
    if (Buf != NULL && SizeOfBuf >= (sizeof(DataQueue)))        /* �жϲ����Ƿ���Ч */
    {
        Queue = (DataQueue *)Buf;
			
        OS_CRITICAL_ENTER();	//�����ٽ���
			
        Queue->DataSize = dataSize;                            /* ��ʼ���ṹ������ */
        Queue->MaxData = (SizeOfBuf - (uint32_t)(((DataQueue *)0)->Buf)) / 
                         Queue->DataSize;               /* ������п��Դ洢��������Ŀ */
        Queue->End = Queue->Buf + Queue->MaxData*Queue->DataSize;       /* �������ݻ���Ľ�����ַ */
        Queue->Out = Queue->Buf;
        Queue->In = Queue->Buf;
        Queue->NData = 0;
        Queue->ReadEmpty = ReadEmpty;	
        Queue->WriteFull = WriteFull;
        OS_CRITICAL_EXIT();	//�˳��ٽ���

        return QUEUE_OK;
    }
    else
    {
        return NOT_OK;
    }
}


/*********************************************************************************************************
** ��������: QueueRead
** ��������: ��ȡ�����е�����
** �䡡��: Ret:�洢���ص���Ϣ�ĵ�ַ
**         Buf:ָ����е�ָ��
** �䡡��: NOT_OK     ����������
**         QUEUE_OK   ���յ���Ϣ
**         QUEUE_EMPTY������Ϣ
** ȫ�ֱ���: ��
** ����ģ��: OS_ENTER_CRITICAL,OS_EXIT_CRITICAL
**-------------------------------------------------------------------------------------------------------
** �޸���:
** �ա���:
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
uint8_t QueueRead(uint8_t *Ret, void *Buf)
{
	  CPU_SR_ALLOC();
    uint8_t err;
    volatile DataQueue *Queue;

    err = NOT_OK;
    if (Buf != NULL)                                            /* �����Ƿ���Ч */
    {                                                           /* ��Ч */
        Queue = (DataQueue *)Buf;
        OS_CRITICAL_ENTER();	//�����ٽ���        
        
        if (Queue->NData > 0)                                   /* �����Ƿ�Ϊ�� */
        {       
                                                /* ����         */
            memcpy(Ret,(void *)Queue->Out,Queue->DataSize);				/* ���ݳ���     */
			//*Ret = Queue->Out[0];                               /* ���ݳ���     */
            Queue->Out = Queue->Out+Queue->DataSize;            /* ��������ָ�� */
            if (Queue->Out >= Queue->End)
            {
                Queue->Out = Queue->Buf;
            }
            Queue->NData--;                                     /* ���ݼ���      */
						
						err = QUEUE_OK;
        }
        else
        {                                                       /* ��              */
            err = QUEUE_EMPTY;
            if (Queue->ReadEmpty != NULL)                       /* �����û������� */
            {
                err = Queue->ReadEmpty(Ret, Queue);
            }
        }
        OS_CRITICAL_EXIT();	//�˳��ٽ���

    }
    return err;
}

/*************************************************
QueueReadInInt -�ж��е��ö�����.

modify:rm

RETURN:		NOT_OK     ����������
**         QUEUE_OK   ���յ���Ϣ
**         QUEUE_EMPTY������Ϣ
*************************************************/
uint8_t QueueReadInInt(uint8_t *Ret, void *Buf)
{
    uint8_t err;
    volatile DataQueue *Queue;

    err = NOT_OK;
    if (Buf != NULL)                                            /* �����Ƿ���Ч */
    {                                                           /* ��Ч */
        Queue = (DataQueue *)Buf;
         
        if (Queue->NData > 0)                                   /* �����Ƿ�Ϊ�� */
        {   
                                                    /* ����         */
             memcpy(Ret,(void *)Queue->Out,Queue->DataSize);				/* ���ݳ���     */
			//*Ret = Queue->Out[0];                               /* ���ݳ���     */
            Queue->Out = Queue->Out+Queue->DataSize;             /* ��������ָ�� */
            if (Queue->Out >= Queue->End)
            {
                Queue->Out = (volatile uint8_t *)Queue->Buf;
            }
            Queue->NData--;                                     /* ���ݼ���      */
            err = QUEUE_OK;
        }
        else
        {                                                       /* ��              */
            err = QUEUE_EMPTY;
            if (Queue->ReadEmpty != NULL)                       /* �����û������� */
            {
                err = Queue->ReadEmpty(Ret, Queue);
            }
        }

    }
    return err;
}
/*********************************************************************************************************
** ��������: QueueWrite
** ��������: FIFO��ʽ��������
** �䡡��: Buf :ָ����е�ָ��
**         Data:��Ϣ����
** �䡡��: NOT_OK   :��������
**         QUEUE_FULL:������
**         QUEUE_OK  :���ͳɹ�
** ȫ�ֱ���: ��
** ����ģ��: OS_ENTER_CRITICAL,OS_EXIT_CRITICAL
**-------------------------------------------------------------------------------------------------------
** �޸���:
** �ա���:
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
#ifndef EN_QUEUE_WRITE
#define EN_QUEUE_WRITE      0
#endif

#if EN_QUEUE_WRITE > 0

uint8_t QueueWrite(void *Buf,uint8_t* Data)
{
 
	  CPU_SR_ALLOC();
    uint8_t err;
    volatile DataQueue *Queue;

    err = NOT_OK;
    if (Buf != NULL)                                                    /* �����Ƿ���Ч */
    {
        Queue = (DataQueue *)Buf;
        OS_CRITICAL_ENTER();	//�����ٽ���         

        if (Queue->NData < Queue->MaxData)                              /* �����Ƿ���  */
        {                                                               /* ����        */
            memcpy((void *)Queue->In,Data,Queue->DataSize);											/* �������*/            
            Queue->In = Queue->In+Queue->DataSize;                      /* �������ָ��*/
            if (Queue->In >= Queue->End)
            {
                Queue->In = Queue->Buf;
            }
            Queue->NData++;                                             /* ��������    */
            err = QUEUE_OK;
        }
        else
        {                                                               /* ��           */
            err = QUEUE_FULL;
            if (Queue->WriteFull != NULL)                               /* �����û������� */
            {
                err = Queue->WriteFull(Queue, Data, Q_WRITE_MODE);
            }
        }
        OS_CRITICAL_EXIT();	//�˳��ٽ���

    }
    return err;
}
#endif

/*************************************************
QueueWriteInInt -�ж��е���д����.

modify:rm

RETURN:		NOT_OK     ����������
**         QUEUE_OK   ���յ���Ϣ
**         QUEUE_EMPTY������Ϣ
*************************************************/
uint8_t QueueWriteInInt(void *Buf, uint8_t* Data)
{
    uint8_t err;
    volatile  DataQueue *Queue;

    err = NOT_OK;
    if (Buf != NULL)                                                    /* �����Ƿ���Ч */
    {
        Queue = ( DataQueue *)Buf;
        
        if (Queue->NData < Queue->MaxData)                              /* �����Ƿ���  */
        {                                                               /* ����        */
					memcpy((void *)Queue->In,Data,Queue->DataSize);	/* �������    */
					//Queue->In[0] = Data;                                        /* �������    */
					//Queue->In++;                                                /* �������ָ��*/
					Queue->In = Queue->In+Queue->DataSize;                        /* �������ָ��*/

					if (Queue->In >= Queue->End)
					{
						Queue->In = (volatile uint8_t *)Queue->Buf;
					}
					
					Queue->NData++;                                             /* ��������    */
					err = QUEUE_OK;
        }
        else
        {                                                               /* ��           */
            err = QUEUE_FULL;
            if (Queue->WriteFull != NULL)                               /* �����û������� */
            {
                err = Queue->WriteFull(Queue, Data, Q_WRITE_MODE);
            }
        }

    }
    return err;
}


/*********************************************************************************************************
** ��������: QueueWriteFront
** ��������: LIFO��ʽ��������
** �䡡��: Buf:ָ����е�ָ��
**         Data:��Ϣ����
** �䡡��: QUEUE_FULL:������
**         QUEUE_OK:���ͳɹ�
** ȫ�ֱ���: ��
** ����ģ��: OS_ENTER_CRITICAL,OS_EXIT_CRITICAL
**-------------------------------------------------------------------------------------------------------
** �޸���:
** �ա���:
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
#ifndef EN_QUEUE_WRITE_FRONT
#define EN_QUEUE_WRITE_FRONT    0
#endif

#if EN_QUEUE_WRITE_FRONT > 0

uint8_t QueueWriteFront(VOID *Buf, uint8_t* Data)
{
    uint8_t err;
    DataQueue *Queue;

    err = NOT_OK;
    if (Buf != NULL)                                                    /* �����Ƿ���Ч */
    {
        Queue = (DataQueue *)Buf;
        
        OS_ENTER_CRITICAL();
        
        if (Queue->NData < Queue->MaxData)                              /* �����Ƿ���  */
        {                                                               /* ���� */
            Queue->Out=-Queue->DataSize;                                /* ��������ָ�� */
			//Queue->Out--;                                               /* ��������ָ�� */
            if (Queue->Out < Queue->Buf)
            {
                Queue->Out = Queue->End - 1;
            }
						memcpy(Queue->Out,Data,Queue->DataSize);
            //Queue->Out[0] = Data;                                       /* �������     */
            Queue->NData++;                                             /* ������Ŀ���� */
            err = QUEUE_OK;
        }
        else
        {                                                               /* ��           */
            err = QUEUE_FULL;
            if (Queue->WriteFull != NULL)                               /* �����û������� */
            {
                err = Queue->WriteFull(Queue, Data, Q_WRITE_FRONT_MODE);
            }
        }
        OS_EXIT_CRITICAL();
    }
    return err;
}

#endif

/*********************************************************************************************************
** ��������: QueueNData
** ��������: ȡ�ö�����������
** �䡡��: Buf:ָ����е�ָ��
** �䡡��: ��Ϣ��
** ȫ�ֱ���: ��
** ����ģ��: OS_ENTER_CRITICAL,OS_EXIT_CRITICAL
**-------------------------------------------------------------------------------------------------------
** �޸���:
** �ա���:
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
#ifndef EN_QUEUE_NDATA
#define EN_QUEUE_NDATA    0
#endif

#if EN_QUEUE_NDATA > 0

uint16_t QueueNData(void *Buf)
{
    CPU_SR_ALLOC();
    uint16_t temp;
    
    temp = 0;                                                   /* ������Ч����0 */
    if (Buf != NULL)
    {
        OS_CRITICAL_ENTER();	//�����ٽ��� 			
        temp = ((DataQueue *)Buf)->NData;
        OS_CRITICAL_EXIT();	//�˳��ٽ���			
    }
    return temp;
}

#endif

/*********************************************************************************************************
** ��������: QueueSize
** ��������: ȡ�ö���������
** �䡡��: Buf:ָ����е�ָ��
** �䡡��: ����������
** ȫ�ֱ���: ��
** ����ģ��: OS_ENTER_CRITICAL,OS_EXIT_CRITICAL
**
** ������: 
** �ա���: 
**-------------------------------------------------------------------------------------------------------
** �޸���:
** �ա���:
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
#ifndef EN_QUEUE_SIZE
#define EN_QUEUE_SIZE    0
#endif

#if EN_QUEUE_SIZE > 0

        uint16_t QueueSize(void *Buf)
{
    CPU_SR_ALLOC();
    uint16_t temp;
    
    temp = 0;                                                   /* ������Ч����0 */
    if (Buf != NULL)
    {
        OS_CRITICAL_ENTER();	//�����ٽ��� 				
        temp = ((DataQueue *)Buf)->MaxData;
        OS_CRITICAL_EXIT();	//�˳��ٽ���			
    }
    return temp;
}

#endif

/*********************************************************************************************************
** ��������: OSQFlush
** ��������: ��ն���
** �䡡��: Buf:ָ����е�ָ��
** �䡡��: ��
** ȫ�ֱ���: ��
** ����ģ��: OS_ENTER_CRITICAL,OS_EXIT_CRITICAL
**-------------------------------------------------------------------------------------------------------
** �޸���:
** �ա���:
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
#ifndef EN_QUEUE_FLUSH
#define EN_QUEUE_FLUSH    0
#endif

#if EN_QUEUE_FLUSH > 0

        void QueueFlush(VOID *Buf)
{
    DataQueue *Queue;
    
    if (Buf != NULL)                                                /* �����Ƿ���Ч */
    {                                                               /* ��Ч         */
        Queue = (DataQueue *)Buf;
        OS_ENTER_CRITICAL();
        Queue->Out = Queue->Buf;
        Queue->In = Queue->Buf;
        Queue->NData = 0;                                           /* ������ĿΪ0 */
        OS_EXIT_CRITICAL();
    }
}

#endif

//�жϻ����Ƿ���
//���أ�0-������1-��
uint16_t IsQueueFull(void *Buf)
{
    CPU_SR_ALLOC();
    uint16_t bRet=FALSE;
	  DataQueue *Queue;                                                   /* ������Ч����0 */
    if (Buf != NULL)                                                    /* �����Ƿ���Ч */
    {
    OS_CRITICAL_ENTER();	//�����ٽ��� 	
    Queue = (DataQueue *)Buf;
		if (Queue->NData >= Queue->MaxData)
    {//��
			bRet = TRUE;		
		}
    OS_CRITICAL_EXIT();	//�˳��ٽ���			
    }
    return bRet;
}

/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/
