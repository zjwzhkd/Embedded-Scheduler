/*******************************************************************************
* 文 件 名: sched_evtbuf_q.c
* 创 建 者: Keda Huang
* 版    本: V1.0
* 创建日期: 2017-03-09
* 文件说明: 事件驱动调度器的事件缓存管理, 允许使用事件队列来保存事件.
*******************************************************************************/


#include "sched_evtbuf.h"

#if SCHED_EVTBUF_QUEUE_EN

#include "sched_priotbl.h"
#include "sched_queue.h"
/*******************************************************************************

                                  事件缓存定义

*******************************************************************************/
struct sched_evtbuf
{
    EvtPos_t            len;
    union {
        sSchedPriotbl   *tbl;
        sSchedQueue     *queue;
    } p;
};

/*******************************************************************************

                                  事件缓存管理

*******************************************************************************/
/**
 * 创建事件缓存区, 支持使用事件队列或者优先级记录表记录事件,
 * 若使用优先级记录表记录事件, 则忽略事件的消息
 *
 * @param len: 事件队列的长度, 若设置为0, 使用优先级记录表记录事件
 *
 * @return: 创建成功返回事件缓存区指针, 创建失败返回NULL
 */
sSchedEvtbuf *schedEvtbufCreate(EvtPos_t len)
{
sSchedEvtbuf *pCreatedEvtbuf = NULL;

    pCreatedEvtbuf = (sSchedEvtbuf *)schedPortMalloc(sizeof(sSchedEvtbuf));
    if (pCreatedEvtbuf != NULL)
    {
        pCreatedEvtbuf->len = len;
        if ( len == 0 )
        {
            /* 使用优先级记录表 */
            pCreatedEvtbuf->p.tbl = (sSchedPriotbl *)schedPortMalloc(sizeof(sSchedPriotbl));
            if (pCreatedEvtbuf->p.tbl != NULL)
            {
                schedIntPriotblInit(pCreatedEvtbuf->p.tbl);
            }
            else
            {
                schedPortFree(pCreatedEvtbuf);
                pCreatedEvtbuf = NULL;
            }
        }
        else
        {
            /* 使用事件队列 */
            pCreatedEvtbuf->p.queue = schedIntQueueCreate(len, sizeof(SchedEvent_t));
            if (pCreatedEvtbuf->p.queue == NULL)
            {
                schedPortFree(pCreatedEvtbuf);
                pCreatedEvtbuf = NULL;
            }
        }
    }

    return (pCreatedEvtbuf);
}

/* 删除事件缓存区 */
void schedEvtbufDelete(sSchedEvtbuf *evtbuf)
{
    if (evtbuf != NULL)
    {
        if (evtbuf->len == 0)
        {
            schedPortFree(evtbuf->p.tbl);
        }
        else
        {
            schedIntQueueDelete(evtbuf->p.queue);
        }
        schedPortFree(evtbuf);
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
