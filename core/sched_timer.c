/*******************************************************************************
* 文 件 名: sched_timer.c
* 创 建 者: Keda Huang
* 版    本: V1.0
* 创建日期: 2017-03-16
* 文件说明: 事件驱动调度器的定时器管理
*******************************************************************************/

#include "sched_timer.h"
#include "sched_object.h"
#include "sched_core.h"
#include "sched_tick.h"
#include "sched_task.h"
#include "sched_api.h"
/*******************************************************************************

                                 定时器类型定义

*******************************************************************************/
typedef struct sched_timer
{
    SCHED_BASE_OBJ_DEFINE(obj)

    sSchedDelay         delay;
    SchedHandle_t       hTargetTask;
    SchedEvent_t        event;
    SchedTick_t         period;
    eSchedTimerMode     mode;
    eSchedFlag          arrival;
} sSchedTimer;

/*******************************************************************************

                                 定时器延时回调

*******************************************************************************/
/* 定时器延时对象回调函数 */
static SchedTick_t timer_delay_cb(sSchedDelay *me)
{
sSchedTimer *pTimer = container_of(me, sSchedTimer, delay);

    schedEventSendFromISR(pTimer->hTargetTask, pTimer->event.sig, pTimer->event.msg);
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
 * @param hTargetTask: 目标任务句柄
 *
 * @param eventSig: 定时器事件的信号
 *
 * @param eventMsg: 定时器事件的消息
 *
 * @param mode: 定时器工作模式
 *
 * @param period: 定时器的定时时长
 *
 * @param phCreatedTimer: 定时器创建成功时, 保存新创建定时器句柄的句柄变量地址
 *
 * @return: SCHED_OK                定时器创建成功
 *          SCHED_ERR_CORE_STATE    调度器处于错误的状态
 *          SCHED_ERR_PARAM         参数不符合要求
 *          SCHED_ERR_CREATE_FAILED 定时器创建失败(内存分配失败)
 */
eSchedError schedTimerCreate(SchedHandle_t          hTargetTask,
                             EvtSig_t               eventSig,
                             EvtMsg_t               eventMsg,
                             eSchedTimerMode        mode,
                             SchedTick_t            period,
                             SchedHandle_t * const  phCreatedTimer)
{
sSchedTimer *pCreatedTimer = NULL;

    /* 当调度器处于停止状态时, 允许创建新定时器 */
    if ( !SCHED_CORE_IS_STOPPED() )
    {
        return (SCHED_ERR_CORE_STATE);
    }

    /* 检验参数是否符合要求 */
    if ( hTargetTask == NULL )
    {
        return (SCHED_ERR_PARAM);
    }
    SCHED_BASE_OBJ_ASSERT(hTargetTask, SCHED_OBJ_TASK);

    /* 创建并初始化定时器 */
    pCreatedTimer = (sSchedTimer *)schedPortMalloc(sizeof(sSchedTimer));
    if (pCreatedTimer != NULL)
    {
        SCHED_BASE_OBJ_INITIAL(pCreatedTimer, SCHED_OBJ_TIMER);
        schedDelayInit(&pCreatedTimer->delay, timer_delay_cb);
        pCreatedTimer->hTargetTask = hTargetTask;
        pCreatedTimer->event.sig   = eventSig;
        pCreatedTimer->event.msg   = eventMsg;
        pCreatedTimer->period      = period;
        pCreatedTimer->mode        = mode;
        pCreatedTimer->arrival     = SCHED_RESET;
    }

    /* 定时器创建成功 */
    if (pCreatedTimer != NULL)
    {
        *phCreatedTimer = pCreatedTimer;
        return (SCHED_OK);
    }
    else
    {
        return (SCHED_ERR_CREATE_FAILED);
    }
}

/**
 * 启动(重启)定时器
 *
 * @param hTimer: 待操作的定时器句柄
 *
 * @return: SCHED_OK                定时器成功启动(重启)
 *          SCHED_ERR_CORE_STATE    调度器处于错误的状态
 *          SCHED_ERR_HANDLE_NULL   定时器句柄为NULL(定时器未创建)
 */
eSchedError schedTimerStart(SchedHandle_t hTimer)
{
sSchedTimer *pTimer = (sSchedTimer *)hTimer;
SchedCPU_t  cpu_sr;

    /* 检测调度器是否处于运行状态 */
    /*
    if ( !SCHED_CORE_IS_RUNNING() )
    {
        return (SCHED_ERR_CORE_STATE);
    }
    */

    /* 检测定时器句柄有效性 */
    if ( hTimer == NULL )
    {
        return (SCHED_ERR_HANDLE_NULL);
    }
    SCHED_BASE_OBJ_ASSERT(hTimer, SCHED_OBJ_TIMER);

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
 * @param hTimer: 待操作的定时器句柄
 *
 * @return: SCHED_OK                定时器成功复位
 *          SCHED_ERR_CORE_STATE    调度器处于错误的状态
 *          SCHED_ERR_HANDLE_NULL   定时器句柄为NULL(定时器未创建)
 */
eSchedError schedTimerReset(SchedHandle_t hTimer)
{
sSchedTimer *pTimer = (sSchedTimer *)hTimer;
SchedCPU_t  cpu_sr;

    /* 检测调度器是否处于运行状态 */
    /*
    if ( !SCHED_CORE_IS_RUNNING() )
    {
        return (SCHED_ERR_CORE_STATE);
    }
    */

    /* 检测定时器句柄有效性 */
    if ( hTimer == NULL )
    {
        return (SCHED_ERR_HANDLE_NULL);
    }
    SCHED_BASE_OBJ_ASSERT(hTimer, SCHED_OBJ_TIMER);

    cpu_sr = SCHED_EnterCritical();
    {
        pTimer->arrival = SCHED_RESET;
        schedDelayRemove(&pTimer->delay);
    }
    SCHED_ExitCritical(cpu_sr);

    return (SCHED_OK);
}

/**
 * 改变定时器的目标任务句柄
 *
 * @param hTimer: 待操作的定时器句柄
 *
 * @param hTargetTask: 目标任务句柄
 *
 * @return: SCHED_OK                目标任务句柄修改成功
 *          SCHED_ERR_HANDLE_NULL   定时器句柄为NULL(定时器未创建)
 *          SCHED_ERR_PARAM         参数不符合要求
 */
eSchedError schedTimerChangeTarget(SchedHandle_t hTimer, SchedHandle_t hTargetTask)
{
sSchedTimer *pTimer = (sSchedTimer *)hTimer;
SchedCPU_t  cpu_sr;

    /* 检测定时器句柄有效性 */
    if ( hTimer == NULL )
    {
        return (SCHED_ERR_HANDLE_NULL);
    }
    SCHED_BASE_OBJ_ASSERT(hTimer, SCHED_OBJ_TIMER);

    /* 检验参数是否符合要求 */
    if ( hTargetTask == NULL )
    {
        return (SCHED_ERR_PARAM);
    }
    SCHED_BASE_OBJ_ASSERT(hTargetTask, SCHED_OBJ_TASK);

    cpu_sr = SCHED_EnterCritical();
    {
        pTimer->hTargetTask = hTargetTask;
    }
    SCHED_ExitCritical(cpu_sr);

    return (SCHED_OK);
}

/**
 * 改变定时器的定时器事件
 *
 * @param hTimer: 待操作的定时器句柄
 *
 * @param eventSig: 定时器事件的信号
 *
 * @param eventMsg: 定时器事件的消息
 *
 * @return: SCHED_OK                定时器事件修改成功
 *          SCHED_ERR_HANDLE_NULL   定时器句柄为NULL(定时器未创建)
 */
eSchedError schedTimerChangeEvent(SchedHandle_t hTimer, EvtSig_t eventSig, EvtMsg_t eventMsg)
{
sSchedTimer *pTimer = (sSchedTimer *)hTimer;
SchedCPU_t  cpu_sr;

    /* 检测任务句柄有效性 */
    if ( hTimer == NULL )
    {
        return (SCHED_ERR_HANDLE_NULL);
    }
    SCHED_BASE_OBJ_ASSERT(hTimer, SCHED_OBJ_TIMER);

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
 * @param hTimer: 待操作的定时器句柄
 *
 * @param mode: 定时器工作模式
 *
 * @return: SCHED_OK                工作模式修改成功
 *          SCHED_ERR_HANDLE_NULL   定时器句柄为NULL(定时器未创建)
 */
eSchedError schedTimerChangeMode(SchedHandle_t hTimer, eSchedTimerMode mode)
{
sSchedTimer *pTimer = (sSchedTimer *)hTimer;
SchedCPU_t  cpu_sr;

    /* 检测定时器句柄有效性 */
    if ( hTimer == NULL )
    {
        return (SCHED_ERR_HANDLE_NULL);
    }
    SCHED_BASE_OBJ_ASSERT(hTimer, SCHED_OBJ_TIMER);

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
 * @param hTimer: 待操作的定时器句柄
 *
 * @param period: 定时器的定时时长
 *
 * @return: SCHED_OK                定时时长修改成功
 *          SCHED_ERR_HANDLE_NULL   定时器句柄为NULL(定时器未创建)
 */
eSchedError schedTimerChangePeriod(SchedHandle_t hTimer, SchedTick_t period)
{
sSchedTimer *pTimer = (sSchedTimer *)hTimer;
SchedCPU_t  cpu_sr;

    /* 检测定时器句柄有效性 */
    if ( hTimer == NULL )
    {
        return (SCHED_ERR_HANDLE_NULL);
    }
    SCHED_BASE_OBJ_ASSERT(hTimer, SCHED_OBJ_TIMER);

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
 * @param hTimer: 定时器句柄
 *
 * @return: SCHED_TIMER_RESET       定时器处于复位状态, 或者定时器未创建
 *          SCHED_TIMER_RUNNING     定时器正在运行
 *          SCHED_TIMER_ARRIVAL     定时器计时结束, 并且处于停止状态
 */
eSchedTimerStatus schedTimerGetStatus(SchedHandle_t hTimer)
{
sSchedTimer *pTimer = (sSchedTimer *)hTimer;
SchedCPU_t  cpu_sr;
eSchedTimerStatus ret;

    if (hTimer == NULL)
    {
        return (SCHED_TIMER_RESET);
    }
    SCHED_BASE_OBJ_ASSERT(hTimer, SCHED_OBJ_TIMER);

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
