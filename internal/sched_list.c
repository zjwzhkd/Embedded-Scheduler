/*******************************************************************************
* 文 件 名: sched_list.c
* 创 建 者: Keda Huang
* 版    本: V1.0
* 创建日期: 2016-12-27
* 文件说明: 内部数据结构 -- 双向排序链表
*******************************************************************************/

#include "sched_internal.h"
/*******************************************************************************

                                    操作函数

*******************************************************************************/

/**
 * 初始化链表或者链表项
 *
 * @param list: 待初始化的链表(项)指针
 */
void schedIntListInit(sSchedList *list)
{
    list->next  = list;
    list->prev  = list;
    list->value = SCHED_TICK_MAX;
}

/**
 * 向指定的链表, 按照排序值从小到大的顺序插入链表项
 *
 * @param list: 目标链表指针
 *
 * @param item: 插入链表项指针
 */
void schedIntListInsert(sSchedList *list, sSchedList *item)
{
sSchedList *pInterator;
SchedTick_t const valueOfItem = item->value;

    SCHED_ASSERT(SCHED_TICK_MAX == list->value);
    SCHED_ASSERT(SCHED_LIST_IS_EMPTY(item));

    if (SCHED_TICK_MAX == valueOfItem)
    {
        pInterator = list->prev;
    }
    else
    {
        for (pInterator = list; pInterator->next->value <= valueOfItem; pInterator = pInterator->next)
        {
        }
    }

    item->next       = pInterator->next;
    item->prev       = pInterator;
    pInterator->next = item;
    item->next->prev = item;
}

/**
 * 向指定链表的尾部插入链表项
 *
 * @param list: 目标链表项
 *
 * @param item: 插入链表项指针
 */
void schedIntListInsertEnd(sSchedList *list, sSchedList *item)
{
    SCHED_ASSERT(SCHED_LIST_IS_EMPTY(item));

    item->prev       = list->prev;
    item->next       = list;
    item->prev->next = item;
    list->prev       = item;
}

/**
 * 移除指定链表项
 *
 * @param item: 待移除的链表项指针
 */
void schedIntListRemove(sSchedList *item)
{
    SCHED_ASSERT(NULL != item->next);
    SCHED_ASSERT(NULL != item->prev);

    item->next->prev = item->prev;
    item->prev->next = item->next;
    item->next       = item;
    item->prev       = item;
}

/**
 * 判断链表是否为空, 或者链表项是否孤立
 *
 * @param list: 链表(项)指针
 *
 * @return: 布尔值
 *          SCHED_TRUE  空链表或者孤立链表项
 *          SCHED_FALSE 非空链表或者非孤立链表项
 */
eSchedBool schedIntListIsEmpty(sSchedList *list)
{
eSchedBool ret;

    if (list->next == list)
    {
        SCHED_ASSERT(list->prev == list);
        ret = SCHED_TRUE;
    }
    else
    {
        SCHED_ASSERT(list->prev != list);
        ret = SCHED_FALSE;
    }
    return (ret);
}
