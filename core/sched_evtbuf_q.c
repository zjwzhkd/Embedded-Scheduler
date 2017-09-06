/*******************************************************************************
* 文 件 名: sched_evtbuf_q.c
* 创 建 者: Keda Huang
* 版    本: V1.0
* 创建日期: 2017-03-09
* 文件说明: 事件驱动调度器的事件缓存管理, 允许使用事件队列来保存事件.
*******************************************************************************/


#include "sched_evtbuf.h"
#include "sched_internal.h"

#if SCHED_EVTBUF_QUEUE_EN

/*******************************************************************************

                                  事件缓存管理

*******************************************************************************/
/**
 * 初始化事件缓存区, 支持使用事件队列或者优先级记录表记录事件,
 * 若使用优先级记录表记录事件, 则忽略事件的消息
 *
 * @param evtbuf: 事件缓存区指针
 *
 * @param len: 事件队列的长度, 若设置为0, 使用优先级记录表记录事件
 *
 * @return: 布尔值, 初始化成功返回SCHED_TRUE, 初始化失败返回SCHED_FALSE
 */
eSchedBool schedEvtbufInit(sSchedEvtbuf *evtbuf, EvtPos_t len)
{
eSchedBool ret = SCHED_FALSE;

    evtbuf->len = len;
    if ( len == 0 )
    {
        /* 使用优先级记录表 */
        evtbuf->p.tbl = (sSchedPriotbl *)schedPortMalloc(sizeof(sSchedPriotbl));
        if (evtbuf->p.tbl != NULL)
        {
            schedIntPriotblInit(evtbuf->p.tbl);
            ret = SCHED_TRUE;
        }
    }
    else
    {
        /* 使用事件队列 */
        evtbuf->p.queue = schedIntQueueCreate(len, sizeof(SchedEvent_t));
        if (evtbuf->p.queue != NULL)
        {
            ret = SCHED_TRUE;
        }
    }

    return (ret);
}

/* 释放事件缓存区资源 */
void schedEvtbufRelease(sSchedEvtbuf *evtbuf)
{
    if (evtbuf->len == 0)
    {
        schedPortFree(evtbuf->p.tbl);
    }
    else
    {
        schedIntQueueDelete(evtbuf->p.queue);
    }
}

/* 事件发送至缓存区末端 */
eSchedBool schedEvtbufSend(sSchedEvtbuf *evtbuf, const SchedEvent_t *tx_evt)
{
eSchedBool ret;

    SCHED_ASSERT(tx_evt->sig >= SCHED_SIG_USER);

    if (evtbuf->len == 0)
    {
        SCHED_ASSERT(evtbuf->p.tbl != NULL);
        SCHED_ASSERT(tx_evt->msg == 0);
        schedIntPriotblRecord(evtbuf->p.tbl, tx_evt->sig - SCHED_SIG_USER);
        ret = SCHED_TRUE;
    }
    else
    {
        SCHED_ASSERT(evtbuf->p.queue != NULL);
        ret = schedIntQueueSend(evtbuf->p.queue, tx_evt);
    }

    return (ret);
}

/* 事件发送至缓存区前端 */
eSchedBool schedEvtbufSendToFront(sSchedEvtbuf *evtbuf, const SchedEvent_t *tx_evt)
{
eSchedBool ret;

    SCHED_ASSERT(tx_evt->sig >= SCHED_SIG_USER);

    if (evtbuf->len == 0)
    {
        SCHED_ASSERT(evtbuf->p.tbl != NULL);
        SCHED_ASSERT(tx_evt->msg == 0);
        schedIntPriotblRecord(evtbuf->p.tbl, tx_evt->sig - SCHED_SIG_USER);
        ret = SCHED_TRUE;
    }
    else
    {
        SCHED_ASSERT(evtbuf->p.queue != NULL);
        ret = schedIntQueueSendToFront(evtbuf->p.queue, tx_evt);
    }

    return (ret);
}

/* 从缓存区前端接收事件 */
eSchedBool schedEvtbufReceive(sSchedEvtbuf *evtbuf, SchedEvent_t *rx_evt)
{
eSchedBool ret;
uint8_t sig;

    if (evtbuf->len == 0)
    {
        SCHED_ASSERT(evtbuf->p.tbl != NULL);
        if ( SCHED_PRIOTBL_IS_EMPTY(evtbuf->p.tbl) )
        {
            ret = SCHED_FALSE;
        }
        else
        {
            sig = schedIntPriotblGetHighest(evtbuf->p.tbl);
            schedIntPriotblReset(evtbuf->p.tbl, sig);
            rx_evt->sig = sig + SCHED_SIG_USER;
            rx_evt->msg = 0;
            ret = SCHED_TRUE;
        }
    }
    else
    {
        SCHED_ASSERT(evtbuf->p.queue != NULL);
        ret = schedIntQueueReceive(evtbuf->p.queue, rx_evt);
    }

    return (ret);
}

/* 判断事件缓存区是否空 */
eSchedBool schedEvtbufIsEmpty(sSchedEvtbuf *evtbuf)
{
eSchedBool ret;

    if (evtbuf->len == 0)
    {
        SCHED_ASSERT(evtbuf->p.tbl != NULL);
        if ( SCHED_PRIOTBL_IS_EMPTY(evtbuf->p.tbl) )
        {
            ret = SCHED_TRUE;
        }
        else
        {
            ret = SCHED_FALSE;
        }
    }
    else
    {
        SCHED_ASSERT(evtbuf->p.queue != NULL);
        if ( SCHED_QUEUE_IS_EMPTY(evtbuf->p.queue) )
        {
            ret = SCHED_TRUE;
        }
        else
        {
            ret = SCHED_FALSE;
        }
    }

    return (ret);
}

#endif  /* SCHED_EVTBUF_QUEUE_EN */
