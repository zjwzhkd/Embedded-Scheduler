/*******************************************************************************
* 文 件 名: sched_api.h
* 创 建 者: Keda Huang
* 版    本: V1.0
* 创建日期: 2017-03-19
* 文件说明: 事件驱动调度器的核心API接口
*******************************************************************************/

#ifndef __SCHED_API_H
#define __SCHED_API_H

#include "sched_proto.h"
#include "sched_core_struct.h"

/*******************************************************************************

                                     调度器

*******************************************************************************/
void schedInit(void);
void schedStart(void);
void schedTickHandler(void);
eSchedBool schedHasReadyTask(void);

/*******************************************************************************

                                    节拍管理

*******************************************************************************/

#define SCHED_SEC_TO_TICK(_nsec_)   \
    ( (SchedTick_t)((uint32_t)(_nsec_)*SCHED_TICK_HZ) )

#define SCHED_MS_TO_TICK(_nms_) \
    ( (SchedTick_t)((uint32_t)(_nms_)*SCHED_TICK_HZ/1000) )

#define SCHED_HZ_TO_TICK(_nhz_) \
    ( (SchedTick_t)(SCHED_TICK_HZ/(_nhz_)) )

SchedTick_t schedTickGetCount(void);
SchedTick_t schedTickGetCountFromISR(void);

/*******************************************************************************

                                    任务管理

*******************************************************************************/

#define SCHED_STATE_FUNCTION(_func_)    \
    SchedBase_t _func_(void *me, SchedEvent_t const * const e)

#define THIS_TASK()             ( (sSchedTask *) me )
#define EVT_SIG()               ( e->sig )
#define EVT_MSG()               ( e->msg )

#define SCHED_HANDLED()         ( SCHED_RET_HANDLED )
#define SCHED_IGNORED()         ( SCHED_RET_IGNORED )
#define SCHED_TRAN(_target_)    ( schedStateTransfer(THIS_TASK(), (_target_)) )

eSchedError schedTaskCreate(SchedPrio_t             prio,
                            EvtPos_t                eventLen,
                            SchedEventHandler       initial,
                            sSchedTask * const      pCreatedTask);

eSchedError schedEventSend(sSchedTask *pTask, EvtSig_t sig, EvtMsg_t msg);
eSchedError schedEventSendToFront(sSchedTask *pTask, EvtSig_t sig, EvtMsg_t msg);
eSchedError schedEventSendFromISR(sSchedTask *pTask, EvtSig_t sig, EvtMsg_t msg);
eSchedError schedEventSendToFrontFromISR(sSchedTask *pTask, EvtSig_t sig, EvtMsg_t msg);

SchedBase_t schedStateTransfer(sSchedTask *pTask, SchedEventHandler target);

/*******************************************************************************

                                   定时器管理

*******************************************************************************/
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

/*******************************************************************************

                                    HOOK函数

*******************************************************************************/
extern void schedIdleHook(void);
extern void schedLoopHook(void);

#endif  /* __SCHED_API_H */
