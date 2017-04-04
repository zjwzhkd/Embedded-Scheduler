/*******************************************************************************
* 文 件 名: sched_timer.h
* 创 建 者: Keda Huang
* 版    本: V1.0
* 创建日期: 2017-03-16
* 文件说明: 事件驱动调度器的定时器管理
*******************************************************************************/

#ifndef __SCHED_TIMER_H
#define __SCHED_TIMER_H

/* 头文件 --------------------------------------------------------------------*/
#include "sched_proto.h"

/* 定时器管理 ----------------------------------------------------------------*/
eSchedError schedTimerCreate(SchedHandle_t          hTargetTask,
                             EvtSig_t               eventSig,
                             EvtMsg_t               eventMsg,
                             eSchedTimerMode        mode,
                             SchedTick_t            period,
                             SchedHandle_t * const  phCreatedTimer);

eSchedError schedTimerStart(SchedHandle_t hTimer);
eSchedError schedTimerReset(SchedHandle_t hTimer);

eSchedError schedTimerChangeTarget(SchedHandle_t hTimer, SchedHandle_t hTargetTask);
eSchedError schedTimerChangeEvent(SchedHandle_t hTimer, EvtSig_t eventSig, EvtMsg_t eventMsg);
eSchedError schedTimerChangeMode(SchedHandle_t hTimer, eSchedTimerMode mode);
eSchedError schedTimerChangePeriod(SchedHandle_t hTimer, SchedTick_t period);

eSchedTimerStatus schedTimerGetStatus(SchedHandle_t hTimer);

#endif  /* __SCHED_TIMER_H */
