/*******************************************************************************
* 文 件 名: sched_fsm.c
* 创 建 者: Keda Huang
* 版    本: V1.0
* 创建日期: 2017-02-22
* 文件说明: 事件驱动调度器的有限状态机
*******************************************************************************/

#include "sched_fsm.h"
/*******************************************************************************

                                    全局变量

*******************************************************************************/
/* 状态机内部事件 */
static const SchedEvent_t internal_event[3] =
{
    {SCHED_SIG_EMPTY,   0},
    {SCHED_SIG_ENTRY,   0},
    {SCHED_SIG_EXIT,    0}
};

/*******************************************************************************

                                   状态机函数

*******************************************************************************/
/* 构造有限状态机 */
void schedFSMCtor(sSchedFSM *fsm, SchedEventHandler initial)
{
    fsm->state = initial;
}

/* 初始化状态机, 实现初始化状态转移 */
void schedFSMInit(sSchedFSM *fsm, SchedHandle_t me)
{
SchedBase_t ret;

    ret = fsm->state(me, &internal_event[SCHED_SIG_EMPTY]);
    SCHED_ASSERT(ret == SCHED_RET_TRAN);
    /* 新状态进入 */
    ret = fsm->state(me, &internal_event[SCHED_SIG_ENTRY]);
    SCHED_ASSERT(ret != SCHED_RET_TRAN);
    /* 清除编译器警告 */
    ((void) ret);
}

/* 状态机处理事件 */
void schedFSMDispatch(sSchedFSM *fsm, SchedHandle_t me, SchedEvent_t const *e)
{
SchedEventHandler tmp;
SchedBase_t ret;

    tmp = fsm->state;
    ret = fsm->state(me, e);
    if (ret == SCHED_RET_TRAN)
    {
        /* 旧状态退出 */
        ret = tmp(me, &internal_event[SCHED_SIG_EXIT]);
        SCHED_ASSERT(ret != SCHED_RET_TRAN);
        /* 新状态进入 */
        ret = fsm->state(me, &internal_event[SCHED_SIG_ENTRY]);
        SCHED_ASSERT(ret != SCHED_RET_TRAN);
        /* 清除编译器警告 */
        ((void) ret);
    }
}
