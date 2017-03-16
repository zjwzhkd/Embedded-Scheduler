/*******************************************************************************
* 文 件 名: sched_evtbuf_nq.c
* 创 建 者: Keda Huang
* 版    本: V1.0
* 创建日期: 2017-03-09
* 文件说明: 事件驱动调度器的事件缓存管理, 禁止使用事件队列来保存事件.
*           注意, 此方案仅支持传递事件信号, 不支持传递事件消息
*******************************************************************************/

#include "sched_evtbuf.h"

#if SCHED_EVTBUF_QUEUE_EN == 0

#include "sched_priotbl.h"
/*******************************************************************************

                                  事件缓存定义

*******************************************************************************/
struct sched_evtbuf
{
    sSchedPriotbl   tbl;
};

/*******************************************************************************

                                  事件缓存管理

*******************************************************************************/
/**
 * 创建事件缓存区, 使用优先级记录表记录事件的信号, 忽略事件的消息
 *
 * @param len: 事件队列的长度, 必须设置为0, 否则事件缓存区将创建失败
 *
 * @return: 创建成功返回事件缓存区指针, 创建失败返回NULL
 */
sSchedEvtbuf *schedEvtbufCreate(EvtPos_t len)
{
sSchedEvtbuf *pCreatedEvtbuf = NULL;

    if (len == 0)
    {
        pCreatedEvtbuf = (sSchedEvtbuf *)schedPortMalloc(sizeof(sSchedEvtbuf));
        if (pCreatedEvtbuf != NULL)
        {
            schedIntPriotblInit(&pCreatedEvtbuf->tbl);
        }
    }

    return (pCreatedEvtbuf);
}

/* 删除事件缓存区 */
void schedEvtbufDelete(sSchedEvtbuf *evtbuf)
{
    schedPortFree(evtbuf);
}

/* 事件发送至缓存区末端 */
eSchedBool schedEvtbufSend(sSchedEvtbuf *evtbuf, const SchedEvent_t *tx_evt)
{
    SCHED_ASSERT(evtbuf != NULL);
    SCHED_ASSERT(tx_evt->sig >= SCHED_SIG_USER);
    SCHED_ASSERT(tx_evt->msg == 0);

    schedIntPriotblRecord(&evtbuf->tbl, tx_evt->sig - SCHED_SIG_USER);
    return (SCHED_TRUE);
}

/* 事件发送至缓存区前端 */
eSchedBool schedEvtbufSendToFront(sSchedEvtbuf *evtbuf, const SchedEvent_t *tx_evt)
{
    return schedEvtbufSend(evtbuf, tx_evt);
}

/* 从缓存区前端接收事件 */
eSchedBool schedEvtbufReceive(sSchedEvtbuf *evtbuf, SchedEvent_t *rx_evt)
{
uint8_t sig;

    SCHED_ASSERT(evtbuf != NULL);

    if ( SCHED_PRIOTBL_IS_EMPTY(&evtbuf->tbl) )
    {
        return (SCHED_FALSE);
    }
    else
    {
        sig = schedIntPriotblGetHighest(&evtbuf->tbl);
        schedIntPriotblReset(&evtbuf->tbl, sig);
        rx_evt->sig = sig + SCHED_SIG_USER;
        rx_evt->msg = 0;
        return (SCHED_TRUE);
    }
}

/* 判断事件缓存区是否空 */
eSchedBool schedEvtbufIsEmpty(sSchedEvtbuf *evtbuf)
{
    SCHED_ASSERT(evtbuf != NULL);

    if ( SCHED_PRIOTBL_IS_EMPTY(&evtbuf->tbl) )
    {
        return (SCHED_TRUE);
    }
    else
    {
        return (SCHED_FALSE);
    }
}

#endif  /* SCHED_EVTBUF_QUEUE_EN == 0 */
