/*******************************************************************************
* 文 件 名: sched_queue.h
* 创 建 者: Keda Huang
* 版    本: V1.0
* 创建日期: 2017-01-08
* 文件说明: 内部数据结构 -- 事件队列
*******************************************************************************/

#ifndef __SCHED_QUEUE_H
#define __SCHED_QUEUE_H

/* 头文件 --------------------------------------------------------------------*/
#include "sched_proto.h"

/* 数据类型 ------------------------------------------------------------------*/
/* 事件队列 */
typedef struct sched_queue
{
    EvtPos_t            in;
    EvtPos_t            out;
    EvtPos_t            total;
    EvtPos_t            count;
    size_t              esize;
    uint8_t             buf[];
} sSchedQueue;

/* 宏定义 --------------------------------------------------------------------*/
/*
 * 判断事件队列是否空
 * _queue_: 事件队列指针
 * return:  布尔值
 */
#define SCHED_QUEUE_IS_EMPTY(_queue_)   ( (_queue_)->count == 0 )

/*
 * 判断事件队列是否满
 * _queue_: 事件队列指针
 * return:  布尔值
 */
#define SCHED_QUEUE_IS_FULL(_queue_)    ( (_queue_)->count == (_queue_)->total )

/* 操作函数 ------------------------------------------------------------------*/
sSchedQueue *schedIntQueueCreate(EvtPos_t nMemb, size_t size);
void schedIntQueueDelete(sSchedQueue *queue);
eSchedBool schedIntQueueSend(sSchedQueue *queue, const void *pItem);
eSchedBool schedIntQueueSendToFront(sSchedQueue *queue, const void *pItem);
eSchedBool schedIntQueueReceive(sSchedQueue *queue, void *buffer);
eSchedBool schedIntQueueIsEmpty(sSchedQueue *queue);
eSchedBool schedIntQueueIsFull(sSchedQueue *queue);

#endif  /* __SCHED_QUEUE_H */
