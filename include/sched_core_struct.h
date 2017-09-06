/*******************************************************************************
* 文 件 名: sched_core_struct.h
* 创 建 者: Keda Huang
* 版    本: V1.0
* 创建日期: 2017-07-06
* 文件说明: 事件驱动调度器核心组件的结构体定义
*******************************************************************************/

#ifndef __SCHED_CORE_STRUCT_H
#define __SCHED_CORE_STRUCT_H

/* 头文件 --------------------------------------------------------------------*/
#include "sched_prototype.h"
#include "sched_internal_struct.h"

/* 事件缓存 ------------------------------------------------------------------*/
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

/* 延时管理 ------------------------------------------------------------------*/
struct sched_delay;
typedef struct sched_delay sSchedDelay;

/* 延时回调函数原型
 *
 * @param me: 指向延时结束的延时管理结构体
 *
 * @return: 继续延时的节拍数, 若为0, 则表示不继续延时
 */
typedef SchedTick_t (*SchedDelayCB)(sSchedDelay *me);

/* 延时管理结构体 */
struct sched_delay
{
    sSchedList              item;   /* 节拍延时链表项 */
    SchedDelayCB            cb;     /* 延时回调函数   */
};

/* 任务管理 ------------------------------------------------------------------*/
typedef struct sched_task
{
    sSchedFSM               fsm;
    sSchedList              prio;
    sSchedEvtbuf            evtbuf;
} sSchedTask;

/* 定时器管理 ----------------------------------------------------------------*/
typedef struct sched_timer
{
    sSchedDelay             delay;
    sSchedTask             *pTargetTask;
    SchedEvent_t            event;
    SchedTick_t             period;
    eSchedTimerMode         mode;
    eSchedFlag              arrival;
} sSchedTimer;

#endif  /* __SCHED_CORE_STRUCT_H */
