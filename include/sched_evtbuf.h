/*******************************************************************************
* 文 件 名: sched_evtbuf.h
* 创 建 者: Keda Huang
* 版    本: V1.0
* 创建日期: 2017-02-22
* 文件说明: 事件驱动调度器的事件缓存管理
*******************************************************************************/

#ifndef __SCHED_EVTBUF_H
#define __SCHED_EVTBUF_H

/* 头文件 --------------------------------------------------------------------*/
#include "sched_proto.h"

/* 事件缓存类型 --------------------------------------------------------------*/
struct sched_evtbuf;
typedef struct sched_evtbuf sSchedEvtbuf;

#if SCHED_EVTBUF_QUEUE_EN

    #include "sched_priotbl.h"
    #include "sched_queue.h"
    /* 支持使用优先级记录表或者事件队列作为事件缓存区 */
    struct sched_evtbuf
    {
        EvtPos_t            len;
        union {
            sSchedPriotbl   *tbl;
            sSchedQueue     *queue;
        } p;
    };

#else

    #include "sched_priotbl.h"
    /* 仅支持使用优先级记录表作为事件缓存区 */
    struct sched_evtbuf
    {
        sSchedPriotbl       tbl;
    };

#endif

/* 事件缓存管理 --------------------------------------------------------------*/
eSchedBool schedEvtbufInit(sSchedEvtbuf *evtbuf, EvtPos_t len);
void schedEvtbufRelease(sSchedEvtbuf *evtbuf);
eSchedBool schedEvtbufSend(sSchedEvtbuf *evtbuf, const SchedEvent_t *tx_evt);
eSchedBool schedEvtbufSendToFront(sSchedEvtbuf *evtbuf, const SchedEvent_t *tx_evt);
eSchedBool schedEvtbufReceive(sSchedEvtbuf *evtbuf, SchedEvent_t *rx_evt);
eSchedBool schedEvtbufIsEmpty(sSchedEvtbuf *evtbuf);

#endif  /* __SCHED_EVTBUF_H */
