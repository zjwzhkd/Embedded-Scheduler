/*******************************************************************************
* 文 件 名: sched_tick.c
* 创 建 者: Keda Huang
* 版    本: V1.0
* 创建日期: 2017-02-21
* 文件说明: 事件驱动调度器的节拍管理
*******************************************************************************/

#include "sched_tick.h"
#include "sched_core.h"
/*******************************************************************************

                                    全局变量

*******************************************************************************/
/* 节拍延时管理 */
static sSchedList delayList1;
static sSchedList delayList2;
static sSchedList * volatile pCurrentDelayList;
static sSchedList * volatile pOverflowDelayList;
static SchedTick_t volatile currentTickCount;
static SchedTick_t volatile nextTickArrival;

/*******************************************************************************

                                     宏定义

*******************************************************************************/
/* 交换延时链表 */
#define SWITCH_DELAY_LIST()     do                              \
{                                                               \
    sSchedList * const _tmp_list = pCurrentDelayList;           \
    /* 段言: 交换延时链表时, 当前延时链表是空链表 */            \
    SCHED_ASSERT(SCHED_LIST_IS_EMPTY(_tmp_list));               \
    pCurrentDelayList       = pOverflowDelayList;               \
    pOverflowDelayList      = _tmp_list;                        \
} while(0)

/* 更新最近结束延时的节拍时刻 */
#define UPDATE_NEXT_ARRIVAL()   do                                          \
{                                                                           \
    sSchedList * const _tmp_list = pCurrentDelayList;                       \
                                                                            \
    if (SCHED_LIST_IS_EMPTY(_tmp_list))                                     \
    {                                                                       \
        nextTickArrival = SCHED_TICK_MAX;                                   \
    }                                                                       \
    else                                                                    \
    {                                                                       \
        nextTickArrival = SCHED_LIST_GET_VALUE(SCHED_LIST_NEXT(_tmp_list)); \
    }                                                                       \
} while(0)

/* 添加新的延时链表项 */
#define ADD_DELAY_ITEM(_pItem_, _now_, _period_)    do          \
{                                                               \
    SchedTick_t const _tmp_current = (_now_);                   \
    SchedTick_t const _tmp_arrival = (_now_) + (_period_);      \
    sSchedList * const _tmp_item = (_pItem_);                   \
    /* 断言: 延时时间不为0 */                                   \
    SCHED_ASSERT(_tmp_arrival != _tmp_current);                 \
    SCHED_LIST_SET_VALUE(_tmp_item, _tmp_arrival);              \
    if (_tmp_arrival > _tmp_current)                            \
    {                                                           \
        schedIntListInsert(pCurrentDelayList, _tmp_item);       \
    }                                                           \
    else                                                        \
    {                                                           \
        schedIntListInsert(pOverflowDelayList, _tmp_item);      \
    }                                                           \
} while(0)

/*******************************************************************************

                                    节拍管理

*******************************************************************************/

/* 初始化调度器节拍管理 */
void schedTickInit(void)
{
    schedIntListInit(&delayList1);
    schedIntListInit(&delayList2);
    pCurrentDelayList   = &delayList1;
    pOverflowDelayList  = &delayList2;
    currentTickCount    = 0;
    nextTickArrival     = SCHED_TICK_MAX;
}

/* 调度器节拍中断处理函数 */
void schedTickHandler(void)
{
    if ( SCHED_CORE_IS_RUNNING() )
    {
    SchedCPU_t cpu_sr;

        cpu_sr = SCHED_EnterCriticalFromISR();  /* 进入临界区 */
        {
        SchedTick_t const newTick = currentTickCount + 1;

            currentTickCount = newTick;

            /* 节拍计数溢出, 交换延时链表 */
            if (newTick == 0)
            {
                SWITCH_DELAY_LIST();
                UPDATE_NEXT_ARRIVAL();
            }

            /* 处理延时结束的延时回调函数 */
            if (newTick >= nextTickArrival)
            {
            sSchedList * const pCurrentList = pCurrentDelayList;
            sSchedList *pListItem;
            sSchedDelay *pDelay;
            SchedTick_t arrival;
            SchedTick_t period;

                for (;;)
                {
                    /* 当前延时链表是空链表 */
                    if (SCHED_LIST_IS_EMPTY(pCurrentList))
                    {
                        nextTickArrival = SCHED_TICK_MAX;
                        break;
                    }

                    pListItem = SCHED_LIST_NEXT(pCurrentList);
                    arrival   = SCHED_LIST_GET_VALUE(pListItem);
                    /* 延时结束时间未到达 */
                    if ( arrival > newTick )
                    {
                        nextTickArrival = arrival;
                        break;
                    }

                    /* 延时结束, 处理延时回调函数 */
                    schedIntListRemove(pListItem);
                    pDelay = SCHED_LIST_ENTRY(pListItem, sSchedDelay, item);
                    period = pDelay->cb(pDelay);
                    if (period > 0)
                    {
                        ADD_DELAY_ITEM(pListItem, newTick, period);
                    }
                }
            }
        }
        SCHED_ExitCriticalFromISR(cpu_sr);      /* 退出临界区 */
    }
}

/* 获取调度器当前节拍计数 */
SchedTick_t schedTickGetCount(void)
{
SchedTick_t current;
SchedCPU_t cpu_sr;

    cpu_sr = SCHED_EnterCritical();
    {
        current = currentTickCount;
    }
    SCHED_ExitCritical(cpu_sr);

    return (current);
}

/* 获取调度器当前节拍计数(中断函数版) */
SchedTick_t schedTickGetCountFromISR(void)
{
SchedTick_t current;
SchedCPU_t cpu_sr;

    cpu_sr = SCHED_EnterCriticalFromISR();
    {
        current = currentTickCount;
    }
    SCHED_ExitCriticalFromISR(cpu_sr);

    return (current);
}

/*******************************************************************************

                                    延时管理

*******************************************************************************/
/* 初始化延时管理结构体 */
void schedDelayInit(sSchedDelay *pDelay, SchedDelayCB cb)
{
    SCHED_ASSERT(cb != 0);
    schedIntListInit(&pDelay->item);
    pDelay->cb = cb;
}

/**
 * 将延时管理结构体添加到延时链表
 *
 * @param pDelay: 延时管理结构体指针
 *
 * @param period: 延时节拍数, 0表示移除延时管理结构体
 *
 * @note: 本函数必须在临界区内调用
 */
void schedDelayAdd(sSchedDelay *pDelay, SchedTick_t period)
{
SchedTick_t const current = currentTickCount;
sSchedList * const pListItem = &pDelay->item;

    if ( !SCHED_LIST_IS_EMPTY(pListItem) )
    {
        schedIntListRemove(pListItem);
    }

    if (period > 0)
    {
        ADD_DELAY_ITEM(pListItem, current, period);
    }

    UPDATE_NEXT_ARRIVAL();
}

/**
 * 将延时管理结构体从延时链表中移除
 *
 * @param pDelay: 延时管理结构体指针
 *
 * @note: 本函数必须在临界区内调用
 */
void schedDelayRemove(sSchedDelay *pDelay)
{
sSchedList * const pListItem = &pDelay->item;

    if ( !SCHED_LIST_IS_EMPTY(pListItem) )
    {
        schedIntListRemove(pListItem);
    }

    UPDATE_NEXT_ARRIVAL();
}
