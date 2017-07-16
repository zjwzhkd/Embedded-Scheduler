/*******************************************************************************
* 文 件 名: sched_timer.c
* 创 建 者: Keda Huang
* 版    本: V1.0
* 创建日期: 2017-03-16
* 文件说明: 事件驱动调度器的定时器管理
*******************************************************************************/

#include "sched_timer.h"
#include "sched_core.h"
#include "sched_tick.h"
#include "sched_task.h"
#include "sched_api.h"

/*******************************************************************************

                                 定时器延时回调

*******************************************************************************/
/* 定时器延时对象回调函数 */
static SchedTick_t timer_delay_cb(sSchedDelay *me)
{
sSchedTimer *pTimer = container_of(me, sSchedTimer, delay);

    schedEventSendFromISR(pTimer->pTargetTask, pTimer->event.sig, pTimer->event.msg);
    pTimer->arrival = SCHED_SET;
    if (pTimer->mode == SCHED_TIMER_ONCE)
    {
        return 0;
    }
    else
    {
        return (pTimer->period);
    }
}

/*******************************************************************************

                                 定时器管理函数

*******************************************************************************/
/**
 * 创建定时器
 *
 * @param pTargetTask: 目标任务的结构体指针
 *
 * @param eventSig: 定时器事件的信号
 *
 * @param eventMsg: 定时器事件的消息
 *
 * @param mode: 定时器工作模式
 *
 * @param period: 定时器的定时时长
 *
 * @param pCreatedTimer: 待创建定时器的结构体指针
 *
 * @return: SCHED_OK                定时器创建成功
 */
eSchedError schedTimerCreate(sSchedTask * const     pTargetTask,
                             EvtSig_t               eventSig,
                             EvtMsg_t               eventMsg,
                             eSchedTimerMode        mode,
                             SchedTick_t            period,
                             sSchedTimer * const    pCreatedTimer)
{
    /* 当调度器处于停止状态时, 允许创建新定时器 */
    SCHED_ASSERT( SCHED_CORE_IS_STOPPED() );

    /* 初始化定时器结构体 */
    schedDelayInit(&pCreatedTimer->delay, timer_delay_cb);
    pCreatedTimer->pTargetTask = pTargetTask;
    pCreatedTimer->event.sig   = eventSig;
    pCreatedTimer->event.msg   = eventMsg;
    pCreatedTimer->period      = period;
    pCreatedTimer->mode        = mode;
    pCreatedTimer->arrival     = SCHED_RESET;

    return (SCHED_OK);
}

/**
 * 启动(重启)定时器
 *
 * @param pTimer: 待操作定时器的结构体指针
 *
 * @return: SCHED_OK                定时器成功启动(重启)
 *          SCHED_ERR_CORE_STATE    调度器处于错误的状态
 */
eSchedError schedTimerStart(sSchedTimer *pTimer)
{
SchedCPU_t  cpu_sr;

    /* 检测调度器是否处于运行状态 */
    if ( !SCHED_CORE_IS_RUNNING() )
    {
        return (SCHED_ERR_CORE_STATE);
    }

    cpu_sr = SCHED_EnterCritical();
    {
        pTimer->arrival = SCHED_RESET;
        schedDelayAdd(&pTimer->delay, pTimer->period);
    }
    SCHED_ExitCritical(cpu_sr);

    return (SCHED_OK);
}

/**
 * 复位定时器
 *
 * @param pTimer: 待操作定时器的结构体指针
 *
 * @return: SCHED_OK                定时器成功复位
 *          SCHED_ERR_CORE_STATE    调度器处于错误的状态
 */
eSchedError schedTimerReset(sSchedTimer *pTimer)
{
SchedCPU_t  cpu_sr;

    /* 检测调度器是否处于运行状态 */
    if ( !SCHED_CORE_IS_RUNNING() )
    {
        return (SCHED_ERR_CORE_STATE);
    }

    cpu_sr = SCHED_EnterCritical();
    {
        pTimer->arrival = SCHED_RESET;
        schedDelayRemove(&pTimer->delay);
    }
    SCHED_ExitCritical(cpu_sr);

    return (SCHED_OK);
}

/**
 * 改变定时器的目标任务
 *
 * @param pTimer: 待操作定时器的结构体指针
 *
 * @param pTargetTask: 目标任务的结构体指针
 *
 * @return: SCHED_OK                目标任务句柄修改成功
 */
eSchedError schedTimerChangeTarget(sSchedTimer *pTimer, sSchedTask * const pTargetTask)
{
SchedCPU_t  cpu_sr;

    cpu_sr = SCHED_EnterCritical();
    {
        pTimer->pTargetTask = pTargetTask;
    }
    SCHED_ExitCritical(cpu_sr);

    return (SCHED_OK);
}

/**
 * 改变定时器的定时器事件
 *
 * @param pTimer: 待操作定时器的结构体指针
 *
 * @param eventSig: 定时器事件的信号
 *
 * @param eventMsg: 定时器事件的消息
 *
 * @return: SCHED_OK                定时器事件修改成功
 */
eSchedError schedTimerChangeEvent(sSchedTimer *pTimer, EvtSig_t eventSig, EvtMsg_t eventMsg)
{
SchedCPU_t  cpu_sr;

    cpu_sr = SCHED_EnterCritical();
    {
        pTimer->event.sig = eventSig;
        pTimer->event.msg = eventMsg;
    }
    SCHED_ExitCritical(cpu_sr);

    return (SCHED_OK);
}

/**
 * 改变定时器的工作模式
 *
 * @param pTimer: 待操作定时器的结构体指针
 *
 * @param mode: 定时器工作模式
 *
 * @return: SCHED_OK                工作模式修改成功
 */
eSchedError schedTimerChangeMode(sSchedTimer *pTimer, eSchedTimerMode mode)
{
SchedCPU_t  cpu_sr;

    cpu_sr = SCHED_EnterCritical();
    {
        pTimer->mode = mode;
    }
    SCHED_ExitCritical(cpu_sr);

    return (SCHED_OK);
}

/**
 * 改变定时器的定时时长
 *
 * @param pTimer: 待操作定时器的结构体指针
 *
 * @param period: 定时器的定时时长
 *
 * @return: SCHED_OK                定时时长修改成功
 */
eSchedError schedTimerChangePeriod(sSchedTimer *pTimer, SchedTick_t period)
{
SchedCPU_t  cpu_sr;

    cpu_sr = SCHED_EnterCritical();
    {
        pTimer->period = period;
    }
    SCHED_ExitCritical(cpu_sr);

    return (SCHED_OK);
}

/**
 * 获取定时器当前状态
 *
 * @param pTimer: 待操作定时器的结构体指针
 *
 * @return: SCHED_TIMER_RESET       定时器处于复位状态
 *          SCHED_TIMER_RUNNING     定时器正在运行
 *          SCHED_TIMER_ARRIVAL     定时器计时结束, 并且处于停止状态
 */
eSchedTimerStatus schedTimerGetStatus(sSchedTimer *pTimer)
{
SchedCPU_t  cpu_sr;
eSchedTimerStatus ret;

    cpu_sr = SCHED_EnterCritical();
    {
        if ( !SCHED_DELAY_IS_IDLE(&pTimer->delay) )
        {
            ret = SCHED_TIMER_RUNNING;
        }
        else if (pTimer->arrival == SCHED_RESET)
        {
            ret = SCHED_TIMER_RESET;
        }
        else
        {
            ret = SCHED_TIMER_ARRIVAL;
        }
    }
    SCHED_ExitCritical(cpu_sr);

    return (ret);
}
