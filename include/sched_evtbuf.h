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
#include "sched_core_struct.h"

/* 事件缓存管理 --------------------------------------------------------------*/
eSchedBool schedEvtbufInit(sSchedEvtbuf *evtbuf, EvtPos_t len);
void schedEvtbufRelease(sSchedEvtbuf *evtbuf);
eSchedBool schedEvtbufSend(sSchedEvtbuf *evtbuf, const SchedEvent_t *tx_evt);
eSchedBool schedEvtbufSendToFront(sSchedEvtbuf *evtbuf, const SchedEvent_t *tx_evt);
eSchedBool schedEvtbufReceive(sSchedEvtbuf *evtbuf, SchedEvent_t *rx_evt);
eSchedBool schedEvtbufIsEmpty(sSchedEvtbuf *evtbuf);

#endif  /* __SCHED_EVTBUF_H */
