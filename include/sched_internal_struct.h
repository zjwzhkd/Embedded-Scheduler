/*******************************************************************************
* 文 件 名: sched_internal_struct.h
* 创 建 者: Keda Huang
* 版    本: V1.0
* 创建日期: 2017-09-06
* 文件说明: 事件驱动调度器内部数据结构的定义
*******************************************************************************/

#ifndef __SCHED_INTERNAL_STRUCT_H
#define __SCHED_INTERNAL_STRUCT_H

/* 头文件 --------------------------------------------------------------------*/
#include "sched_prototype.h"

/* 双向排序链表 --------------------------------------------------------------*/
typedef struct sched_list
{
    struct sched_list   *next;  /* 指向链表后一项 */
    struct sched_list   *prev;  /* 指向链表前一项 */
    SchedTick_t         value;  /* 链表项排序值   */
} sSchedList;

/* 优先级记录表 --------------------------------------------------------------*/
typedef struct sched_priotbl
{
    uint8_t             tbl[SCHED_PRIOTBL_TBL_SIZE];
    uint8_t             grp;
} sSchedPriotbl;

/* 事件队列 ------------------------------------------------------------------*/
typedef struct sched_queue
{
    EvtPos_t            in;
    EvtPos_t            out;
    EvtPos_t            total;
    EvtPos_t            count;
    size_t              esize;
    uint8_t             buf[];
} sSchedQueue;

/* 有限状态机 ----------------------------------------------------------------*/
typedef struct sched_fsm
{
    SchedEventHandler   state;
} sSchedFSM;

#endif  /* __SCHED_INTERNAL_STRUCT_H */
