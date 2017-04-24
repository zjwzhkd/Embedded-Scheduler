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

/* 管理函数 ------------------------------------------------------------------*/
eSchedError schedTaskCreate(SchedPrio_t             prio,
                            EvtPos_t                eventLen,
                            SchedEventHandler       initial,
                            SchedHandle_t * const   phCreatedTask);

/* 事件函数 ------------------------------------------------------------------*/
eSchedError schedEventSend(SchedHandle_t hTask, EvtSig_t sig, EvtMsg_t msg);
eSchedError schedEventSendToFront(SchedHandle_t hTask, EvtSig_t sig, EvtMsg_t msg);
eSchedError schedEventSendFromISR(SchedHandle_t hTask, EvtSig_t sig, EvtMsg_t msg);
eSchedError schedEventSendToFrontFromISR(SchedHandle_t hTask, EvtSig_t sig, EvtMsg_t msg);

/* 执行函数 ------------------------------------------------------------------*/
void schedTaskInit(void);
void schedTaskInitialiseAll(void);
eSchedBool schedTaskExecute(void);
eSchedBool schedHasReadyTask(void);
SchedBase_t schedStateTransfer(SchedHandle_t hTask, SchedEventHandler target);

#endif  /* __SCHED_TASK_H */
