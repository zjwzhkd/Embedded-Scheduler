/*******************************************************************************
* 文 件 名: sched_task.c
* 创 建 者: Keda Huang
* 版    本: V1.0
* 创建日期: 2017-03-16
* 文件说明: 事件驱动调度器的任务管理
*******************************************************************************/

#include "sched_task.h"
#include "sched_object.h"
#include "sched_fsm.h"
#include "sched_list.h"
#include "sched_core.h"
#include "sched_evtbuf.h"
#include "sched_api.h"
/*******************************************************************************

                                  任务类型定义

*******************************************************************************/
typedef struct sched_task
{
    SCHED_BASE_OBJ_DEFINE(obj)

    sSchedFSM               fsm;
    sSchedList              prio;
    sSchedEvtbuf            evtbuf;
} sSchedTask;

/*******************************************************************************

                                    全局函数

*******************************************************************************/
static sSchedList readyTaskList;

/*******************************************************************************

                                 任务优先级管理

*******************************************************************************/
/* 是否存在就绪任务 */
#define READY_TASK_IS_EXISTING()    ( !SCHED_LIST_IS_EMPTY(&readyTaskList) )

/* 获取最高优先级的就绪任务 */
#define GET_READY_TASK(_ptask_)     do                                          \
{                                                                               \
sSchedList * const _tmp_item = SCHED_LIST_NEXT(&readyTaskList);                 \
sSchedTask * const _tmp_task = SCHED_LIST_ENTRY(_tmp_item, sSchedTask, prio);   \
                                                                                \
    _ptask_ = _tmp_task;                                                        \
} while(0)

/* 将指定任务从就绪链表中移除 */
#define RESET_READY_TASK(_ptask_)   do                                          \
{                                                                               \
sSchedList * const _tmp_item = &(_ptask_)->prio;                                \
                                                                                \
    if ( !SCHED_LIST_IS_EMPTY(_tmp_item) )                                      \
    {                                                                           \
        schedIntListRemove(_tmp_item);                                          \
    }                                                                           \
} while(0)

/* 将指定任务添加到就绪链表 */
#define RECORD_READY_TASK(_ptask_)  do                                          \
{                                                                               \
sSchedList * const _tmp_item = &(_ptask_)->prio;                                \
                                                                                \
    if ( SCHED_LIST_IS_EMPTY(_tmp_item) )                                       \
    {                                                                           \
        schedIntListInsert(&readyTaskList, _tmp_item);                          \
    }                                                                           \
} while(0)

/*******************************************************************************

                                    管理函数

*******************************************************************************/
/**
 * 创建调度器任务
 *
 * @param prio: 任务优先级
 *
 * @param eventLen: 事件缓存区大小
 *
 * @param initial: 初始化伪状态
 *
 * @param phCreatedTask: 任务创建成功时, 保存新创建任务句柄的句柄变量地址
 *
 * @return: SCHED_OK                任务创建成功
 *          SCHED_ERR_CORE_STATE    调度器处于错误的状态
 *          SCHED_ERR_CREATE_FAILED 任务创建失败(内存分配失败)
 */
eSchedError schedTaskCreate(SchedPrio_t             prio,
                            EvtPos_t                eventLen,
                            SchedEventHandler       initial,
                            SchedHandle_t * const   phCreatedTask)
{
sSchedTask *pCreatedTask = NULL;
eSchedBool evtbuf_init;

    /* 当调度器处于停止状态时, 允许创建新任务 */
    if ( !SCHED_CORE_IS_STOPPED() )
    {
        return (SCHED_ERR_CORE_STATE);
    }

    /* 创建并初始化任务 */
    pCreatedTask = (sSchedTask *)schedPortMalloc(sizeof(sSchedTask));
    if (pCreatedTask != NULL)
    {
        SCHED_BASE_OBJ_INITIAL(pCreatedTask, SCHED_OBJ_TASK);
        schedFSMCtor(&pCreatedTask->fsm, initial);
        schedIntListInit(&pCreatedTask->prio);
        SCHED_LIST_SET_VALUE(&pCreatedTask->prio, prio);
        evtbuf_init = schedEvtbufInit(&pCreatedTask->evtbuf, eventLen);
        if ( evtbuf_init == SCHED_FALSE )
        {
            schedPortFree(pCreatedTask);
            pCreatedTask = NULL;
        }
    }

    /* 任务创建成功 */
    if (pCreatedTask != NULL)
    {
        *phCreatedTask = pCreatedTask;
        RECORD_READY_TASK(pCreatedTask);
        return (SCHED_OK);
    }
    else
    {
        return (SCHED_ERR_CREATE_FAILED);
    }
}

/*******************************************************************************

                                    事件函数

*******************************************************************************/
/**
 * 向指定的任务发送事件请求
 *
 * @param hTask: 目标任务的句柄
 *
 * @param sig: 事件请求的信号域
 *
 * @param msg: 事件请求的消息域
 *
 * @return: SCHED_OK                    事件请求发送成功
 *          SCHED_ERR_CORE_STATE        调度器处于错误的状态
 *          SCHED_ERR_HANDLE_NULL       任务句柄为NULL(任务未创建)
 *          SCHED_ERR_EVENT_SEND_FAILED 事件请求发送失败
 */
eSchedError schedEventSend(SchedHandle_t hTask, EvtSig_t sig, EvtMsg_t msg)
{
sSchedTask *pTask = (sSchedTask *)hTask;
eSchedBool tx_ret;
SchedEvent_t event;
SchedCPU_t cpu_sr;

    /* 当调度器处于运行状态时, 允许发送事件请求 */
    if ( !SCHED_CORE_IS_RUNNING() )
    {
        return (SCHED_ERR_CORE_STATE);
    }

    /* 检测任务句柄有效性 */
    if ( hTask == NULL )
    {
        return (SCHED_ERR_HANDLE_NULL);
    }
    SCHED_BASE_OBJ_ASSERT(hTask, SCHED_OBJ_TASK);

    event.sig = sig;
    event.msg = msg;
    cpu_sr = SCHED_EnterCritical();
    {
        tx_ret = schedEvtbufSend(&pTask->evtbuf, &event);
        if ( tx_ret )
        {
            RECORD_READY_TASK(pTask);
        }
    }
    SCHED_ExitCritical(cpu_sr);

    if ( tx_ret )
    {
        return (SCHED_OK);
    }
    else
    {
        return (SCHED_ERR_EVENT_SEND_FAILED);
    }
}

/**
 * 向指定的任务发送紧急事件请求
 *
 * @param hTask: 目标任务的句柄
 *
 * @param sig: 事件请求的信号域
 *
 * @param msg: 事件请求的消息域
 *
 * @return: SCHED_OK                    事件请求发送成功
 *          SCHED_ERR_CORE_STATE        调度器处于错误的状态
 *          SCHED_ERR_HANDLE_NULL       任务句柄为NULL(任务未创建)
 *          SCHED_ERR_EVENT_SEND_FAILED 事件请求发送失败
 */
eSchedError schedEventSendToFront(SchedHandle_t hTask, EvtSig_t sig, EvtMsg_t msg)
{
sSchedTask *pTask = (sSchedTask *)hTask;
eSchedBool tx_ret;
SchedEvent_t event;
SchedCPU_t cpu_sr;

    /* 当调度器处于运行状态时, 允许发送事件请求 */
    if ( !SCHED_CORE_IS_RUNNING() )
    {
        return (SCHED_ERR_CORE_STATE);
    }

    /* 检测任务句柄有效性 */
    if ( hTask == NULL )
    {
        return (SCHED_ERR_HANDLE_NULL);
    }
    SCHED_BASE_OBJ_ASSERT(hTask, SCHED_OBJ_TASK);

    event.sig = sig;
    event.msg = msg;
    cpu_sr = SCHED_EnterCritical();
    {
        tx_ret = schedEvtbufSendToFront(&pTask->evtbuf, &event);
        if ( tx_ret )
        {
            RECORD_READY_TASK(pTask);
        }
    }
    SCHED_ExitCritical(cpu_sr);

    if ( tx_ret )
    {
        return (SCHED_OK);
    }
    else
    {
        return (SCHED_ERR_EVENT_SEND_FAILED);
    }
}

/**
 * 在中断服务中向指定的任务发送事件请求
 *
 * @param hTask: 目标任务的句柄
 *
 * @param sig: 事件请求的信号域
 *
 * @param msg: 事件请求的消息域
 *
 * @return: SCHED_OK                    事件请求发送成功
 *          SCHED_ERR_CORE_STATE        调度器处于错误的状态
 *          SCHED_ERR_HANDLE_NULL       任务句柄为NULL(任务未创建)
 *          SCHED_ERR_EVENT_SEND_FAILED 事件请求发送失败
 */
eSchedError schedEventSendFromISR(SchedHandle_t hTask, EvtSig_t sig, EvtMsg_t msg)
{
sSchedTask *pTask = (sSchedTask *)hTask;
eSchedBool tx_ret;
SchedEvent_t event;
SchedCPU_t cpu_sr;

    /* 当调度器处于运行状态时, 允许发送事件请求 */
    if ( !SCHED_CORE_IS_RUNNING() )
    {
        return (SCHED_ERR_CORE_STATE);
    }

    /* 检测任务句柄有效性 */
    if ( hTask == NULL )
    {
        return (SCHED_ERR_HANDLE_NULL);
    }
    SCHED_BASE_OBJ_ASSERT(hTask, SCHED_OBJ_TASK);

    event.sig = sig;
    event.msg = msg;
    cpu_sr = SCHED_EnterCriticalFromISR();
    {
        tx_ret = schedEvtbufSend(&pTask->evtbuf, &event);
        if ( tx_ret )
        {
            RECORD_READY_TASK(pTask);
        }
    }
    SCHED_ExitCriticalFromISR(cpu_sr);

    if ( tx_ret )
    {
        return (SCHED_OK);
    }
    else
    {
        return (SCHED_ERR_EVENT_SEND_FAILED);
    }
}

/**
 * 在中断服务中向指定的任务发送紧急事件请求
 *
 * @param hTask: 目标任务的句柄
 *
 * @param sig: 事件请求的信号域
 *
 * @param msg: 事件请求的消息域
 *
 * @return: SCHED_OK                    事件请求发送成功
 *          SCHED_ERR_CORE_STATE        调度器处于错误的状态
 *          SCHED_ERR_HANDLE_NULL       任务句柄为NULL(任务未创建)
 *          SCHED_ERR_EVENT_SEND_FAILED 事件请求发送失败
 */
eSchedError schedEventSendToFrontFromISR(SchedHandle_t hTask, EvtSig_t sig, EvtMsg_t msg)
{
sSchedTask *pTask = (sSchedTask *)hTask;
eSchedBool tx_ret;
SchedEvent_t event;
SchedCPU_t cpu_sr;

    /* 当调度器处于运行状态时, 允许发送事件请求 */
    if ( !SCHED_CORE_IS_RUNNING() )
    {
        return (SCHED_ERR_CORE_STATE);
    }

    /* 检测任务句柄有效性 */
    if ( hTask == NULL )
    {
        return (SCHED_ERR_HANDLE_NULL);
    }
    SCHED_BASE_OBJ_ASSERT(hTask, SCHED_OBJ_TASK);

    event.sig = sig;
    event.msg = msg;
    cpu_sr = SCHED_EnterCriticalFromISR();
    {
        tx_ret = schedEvtbufSendToFront(&pTask->evtbuf, &event);
        if ( tx_ret )
        {
            RECORD_READY_TASK(pTask);
        }
    }
    SCHED_ExitCriticalFromISR(cpu_sr);

    if ( tx_ret )
    {
        return (SCHED_OK);
    }
    else
    {
        return (SCHED_ERR_EVENT_SEND_FAILED);
    }
}

/*******************************************************************************

                                    执行函数

*******************************************************************************/
/* 初始化任务管理 */
void schedTaskInit(void)
{
    schedIntListInit(&readyTaskList);
}

/* 完成所有任务状态机的初始化状态转移 */
void schedTaskInitialiseAll(void)
{
sSchedList *pos;
sSchedTask *task;

    SCHED_LIST_FOR_EACH(pos, &readyTaskList)
    {
        task = SCHED_LIST_ENTRY(pos, sSchedTask, prio);
        schedFSMInit(&task->fsm, task);
    }
}

/**
 * 实现一次任务调度
 *
 * @return: 布尔值,
 *          SCHED_TRUE  表示完成一次就绪任务的调度
 *          SCHED_FALSE 表示当前无就绪任务等待执行
 */
eSchedBool schedTaskExecute(void)
{
eSchedBool task_ready = SCHED_FALSE;
eSchedBool task_exec  = SCHED_FALSE;
SchedCPU_t cpu_sr;
sSchedTask *task = NULL;
SchedEvent_t event;

    /* 获取就绪任务以及事件请求 */
    cpu_sr = SCHED_EnterCritical();
    {
        if ( READY_TASK_IS_EXISTING() )
        {
            GET_READY_TASK(task);
            task_ready = SCHED_TRUE;
            task_exec  = schedEvtbufReceive(&task->evtbuf, &event);
        }
    }
    SCHED_ExitCritical(cpu_sr);

    /* 任务状态机处理事件 */
    if ( task_exec )
    {
        schedFSMDispatch(&task->fsm, task, &event);
    }

    /* 轮转就绪任务 */
    if ( task_ready )
    {
        cpu_sr = SCHED_EnterCritical();
        {
            RESET_READY_TASK(task);
            if ( !schedEvtbufIsEmpty(&task->evtbuf) )
            {
                RECORD_READY_TASK(task);
            }
        }
        SCHED_ExitCritical(cpu_sr);
    }

    return (task_ready);
}

/* 当前是否存在就绪任务 */
eSchedBool schedHasReadyTask(void)
{
eSchedBool task_ready = SCHED_FALSE;
SchedCPU_t cpu_sr;

    cpu_sr = SCHED_EnterCritical();
    {
        if ( READY_TASK_IS_EXISTING() )
        {
            task_ready = SCHED_TRUE;
        }
    }
    SCHED_ExitCritical(cpu_sr);

    return (task_ready);
}

/* 设置状态转移目标 */
SchedBase_t schedStateTransfer(SchedHandle_t hTask, SchedEventHandler target)
{
sSchedTask *pTask = (sSchedTask *)hTask;

    SCHED_BASE_OBJ_ASSERT(hTask, SCHED_OBJ_TASK);
    pTask->fsm.state = target;
    return (SCHED_RET_TRAN);
}
