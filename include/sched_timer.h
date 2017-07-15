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
#include "sched_core_struct.h"

/* 定时器管理 ----------------------------------------------------------------*/
eSchedError schedTimerCreate(sSchedTask * const     pTargetTask,
                             EvtSig_t               eventSig,
                             EvtMsg_t               eventMsg,
                             eSchedTimerMode        mode,
                             SchedTick_t            period,
                             sSchedTimer * const    pCreatedTimer);

eSchedError schedTimerStart(sSchedTimer *pTimer);
eSchedError schedTimerReset(sSchedTimer *pTimer);

eSchedError schedTimerChangeTarget(sSchedTimer *pTimer, sSchedTask * const pTargetTask);
eSchedError schedTimerChangeEvent(sSchedTimer *pTimer, EvtSig_t eventSig, EvtMsg_t eventMsg);
eSchedError schedTimerChangeMode(sSchedTimer *pTimer, eSchedTimerMode mode);
eSchedError schedTimerChangePeriod(sSchedTimer *pTimer, SchedTick_t period);

eSchedTimerStatus schedTimerGetStatus(sSchedTimer *pTimer);

#endif  /* __SCHED_TIMER_H */
