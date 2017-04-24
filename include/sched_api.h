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
    SchedBase_t _func_(SchedHandle_t me, SchedEvent_t const * const e)

#define SCHED_HANDLED()         ( SCHED_RET_HANDLED )
#define SCHED_IGNORED()         ( SCHED_RET_IGNORED )
#define SCHED_TRAN(_target_)    ( schedStateTransfer(me, (_target_)) )

eSchedError schedTaskCreate(SchedPrio_t             prio,
                            EvtPos_t                eventLen,
                            SchedEventHandler       initial,
                            SchedHandle_t * const   phCreatedTask);

eSchedError schedEventSend(SchedHandle_t hTask, EvtSig_t sig, EvtMsg_t msg);
eSchedError schedEventSendToFront(SchedHandle_t hTask, EvtSig_t sig, EvtMsg_t msg);
eSchedError schedEventSendFromISR(SchedHandle_t hTask, EvtSig_t sig, EvtMsg_t msg);
eSchedError schedEventSendToFrontFromISR(SchedHandle_t hTask, EvtSig_t sig, EvtMsg_t msg);

SchedBase_t schedStateTransfer(SchedHandle_t hTask, SchedEventHandler target);

/*******************************************************************************

                                   定时器管理

*******************************************************************************/
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

/*******************************************************************************

                                    HOOK函数

*******************************************************************************/
extern void schedIdleHook(void);
extern void schedLoopHook(void);

#endif  /* __SCHED_API_H */
