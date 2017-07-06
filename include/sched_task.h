/*******************************************************************************
* 文 件 名: sched_task.h
* 创 建 者: Keda Huang
* 版    本: V1.0
* 创建日期: 2017-03-16
* 文件说明: 事件驱动调度器的任务管理
*******************************************************************************/

#ifndef __SCHED_TASK_H
#define __SCHED_TASK_H

/* 头文件 --------------------------------------------------------------------*/
#include "sched_proto.h"
#include "sched_core_struct.h"

/* 管理函数 ------------------------------------------------------------------*/
eSchedError schedTaskCreate(SchedPrio_t             prio,
                            EvtPos_t                eventLen,
                            SchedEventHandler       initial,
                            sSchedTask * const      pCreatedTask);

/* 事件函数 ------------------------------------------------------------------*/
eSchedError schedEventSend(sSchedTask *pTask, EvtSig_t sig, EvtMsg_t msg);
eSchedError schedEventSendToFront(sSchedTask *pTask, EvtSig_t sig, EvtMsg_t msg);
eSchedError schedEventSendFromISR(sSchedTask *pTask, EvtSig_t sig, EvtMsg_t msg);
eSchedError schedEventSendToFrontFromISR(sSchedTask *pTask, EvtSig_t sig, EvtMsg_t msg);

/* 执行函数 ------------------------------------------------------------------*/
void schedTaskInit(void);
void schedTaskInitialiseAll(void);
eSchedBool schedTaskExecute(void);
eSchedBool schedHasReadyTask(void);
SchedBase_t schedStateTransfer(sSchedTask *pTask, SchedEventHandler target);

#endif  /* __SCHED_TASK_H */
