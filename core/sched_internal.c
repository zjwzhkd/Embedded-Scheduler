/*******************************************************************************
* 文 件 名: sched_internal.c
* 创 建 者: Keda Huang
* 版    本: V1.0
* 创建日期: 2017-09-06
* 文件说明: 内部数据结构的实现
*******************************************************************************/

#include "sched_internal.h"
#include <string.h>
/*******************************************************************************

                                  双向排序链表

*******************************************************************************/
/* 操作函数 ------------------------------------------------------------------*/
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

/*******************************************************************************

                                  优先级记录表

*******************************************************************************/
/* 常数数组 ------------------------------------------------------------------*/
static uint8_t const priotbl_unmap[] =
{
    0u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0x00 to 0x0F */
    4u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0x10 to 0x1F */
    5u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0x20 to 0x2F */
    4u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0x30 to 0x3F */
    6u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0x40 to 0x4F */
    4u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0x50 to 0x5F */
    5u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0x60 to 0x6F */
    4u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0x70 to 0x7F */
    7u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0x80 to 0x8F */
    4u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0x90 to 0x9F */
    5u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0xA0 to 0xAF */
    4u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0xB0 to 0xBF */
    6u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0xC0 to 0xCF */
    4u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0xD0 to 0xDF */
    5u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0xE0 to 0xEF */
    4u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u  /* 0xF0 to 0xFF */
};

/* 操作函数 ------------------------------------------------------------------*/
/**
 * 初始化优先级记录表
 *
 * @param priotbl: 待出始化的优先级记录表指针
 */
void schedIntPriotblInit(sSchedPriotbl *priotbl)
{
uint8_t i;

    for (i = 0; i < SCHED_PRIOTBL_TBL_SIZE; i++)
    {
        priotbl->tbl[i] = 0;
    }
    priotbl->grp = 0;
}

/**
 * 在指定的优先级记录表中, 记录一个优先级
 *
 * @param priotbl: 指定的优先级记录表指针
 *
 * @param prio: 记录的优先级, 限制范围 0 - SCHED_PRIOTBL_LOWEST_PRIO
 */
void schedIntPriotblRecord(sSchedPriotbl *priotbl, uint8_t prio)
{
    if (prio <= SCHED_PRIOTBL_LOWEST_PRIO)
    {
    uint8_t x = prio & 0x07;
    uint8_t y = prio >> 3;

        priotbl->tbl[y] |= (uint8_t)1<<x;
        priotbl->grp    |= (uint8_t)1<<y;
    }
    else
    {
        /* 断言: 记录的优先级没有超出限制范围 */
        SCHED_ASSERT(0);
    }
}

/**
 * 在指定的优先级记录表中, 清除一个优先级
 *
 * @param priotbl: 指定的优先级记录表指针
 *
 * @param prio: 清除的优先级, 限制范围 0 - SCHED_PRIOTBL_LOWEST_PRIO
 */
void schedIntPriotblReset(sSchedPriotbl *priotbl, uint8_t prio)
{
    if (prio <= SCHED_PRIOTBL_LOWEST_PRIO)
    {
    uint8_t x = prio & 0x07;
    uint8_t y = prio >> 3;

        priotbl->tbl[y]  &= ~( (uint8_t)1 << x );
        if (0 == priotbl->tbl[y])
        {
            priotbl->grp &= ~( (uint8_t)1 << y );
        }

    }
    else
    {
        /* 断言: 清除的优先级没有超出限制范围 */
        SCHED_ASSERT(0);
    }
}

/**
 * 获取指定的优先级记录表中记录的最高优先级
 *
 * @note: 在调用本函数获取最高优先级之前, 需要确保优先级记录表非空
 *
 * @param priotbl: 指定的优先级记录表指针
 *
 * @return: 若优先级记录表非空, 返回最高优先级
 *          若优先级记录表为空, 返回0
 */
uint8_t schedIntPriotblGetHighest(sSchedPriotbl *priotbl)
{
uint8_t prio;
uint8_t x,y;

    y = priotbl_unmap[priotbl->grp];
    SCHED_ASSERT(y < SCHED_PRIOTBL_TBL_SIZE);
    SCHED_ASSERT(priotbl->tbl[y] > 0);
    x = priotbl_unmap[priotbl->tbl[y]];
    prio = (y<<3) + x;
    return (prio);
}

/**
 * 判断指定的优先级记录表是否为空
 *
 * @param priotbl: 指定的优先级记录表指针
 *
 * @return: 布尔值
 *          SCHED_TRUE  表示优先级记录表为空
 *          SCHED_FALSE 表示优先级记录表非空
 */
eSchedBool schedIntPriotblIsEmpty(sSchedPriotbl *priotbl)
{
    if (0 == priotbl->grp)
    {
        return SCHED_TRUE;
    }
    else
    {
        return SCHED_FALSE;
    }
}

/*******************************************************************************

                                    事件队列

*******************************************************************************/
/* 操作函数 ------------------------------------------------------------------*/
/**
 * 创建事件队列
 *
 * @param nMemb: 队列中元素的个数
 *
 * @param size: 队列中元素的大小
 *
 * @return: 若创建成功, 返回事件队列指针
 *          若创建失败, 返回 NULL
 */
sSchedQueue *schedIntQueueCreate(EvtPos_t nMemb, size_t size)
{
sSchedQueue *pCreatedQueue = NULL;
size_t buf_size = (size_t)nMemb * size;

    if (buf_size > 0)
    {
        pCreatedQueue = (sSchedQueue *)schedPortMalloc(sizeof(sSchedQueue) + buf_size);
        if (NULL != pCreatedQueue)
        {
            pCreatedQueue->in       = 0;
            pCreatedQueue->out      = 0;
            pCreatedQueue->total    = nMemb;
            pCreatedQueue->count    = 0;
            pCreatedQueue->esize    = size;
        }
    }
    return (pCreatedQueue);
}

/**
 * 删除事件队列
 *
 * @param queue: 待删除的事件队列指针
 */
void schedIntQueueDelete(sSchedQueue *queue)
{
    schedPortFree(queue);
}

/**
 * 向队列尾部添加一个元素
 *
 * @param queue: 目标事件队列指针
 *
 * @param pItem: 待添加的元素指针
 *
 * @return: 布尔值
 *          SCHED_TRUE  元素添加成功
 *          SCHED_FALSE 元素添加失败
 */
eSchedBool schedIntQueueSend(sSchedQueue *queue, const void *pItem)
{
eSchedBool ret = SCHED_FALSE;

    if (queue->count < queue->total)
    {
    size_t const esize = queue->esize;
    size_t off = (size_t)(queue->in);

        queue->in++;
        if (queue->in == queue->total)
        {
            queue->in = 0;
        }
        off *= esize;
        memcpy(&queue->buf[off], pItem, esize);
        queue->count++;
        ret = SCHED_TRUE;
    }
    return (ret);
}

/**
 * 向队列尾部添加一个元素
 *
 * @param queue: 目标事件队列指针
 *
 * @param pItem: 待添加的元素指针
 *
 * @return: 布尔值
 *          SCHED_TRUE  元素添加成功
 *          SCHED_FALSE 元素添加失败
 */
eSchedBool schedIntQueueSendToFront(sSchedQueue *queue, const void *pItem)
{
eSchedBool ret = SCHED_FALSE;

    if (queue->count < queue->total)
    {
    size_t const esize = queue->esize;
    size_t off;

        if (queue->out > 0)
        {
            queue->out--;
        }
        else
        {
            queue->out = queue->total - 1;
        }
        off = (size_t)(queue->out);
        off *= esize;
        memcpy(&queue->buf[off], pItem, esize);
        queue->count++;
        ret = SCHED_TRUE;
    }
    return (ret);
}

/**
 * 从队列头部位置取出一个元素
 *
 * @param queue: 目标事件队列指针
 *
 * @param buffer: 保存取出元素的缓存地址
 *
 * @return: 布尔值
 *          SCHED_TRUE  事件接收成功
 *          SCHED_FALSE 事件接收失败
 */
eSchedBool schedIntQueueReceive(sSchedQueue *queue, void *buffer)
{
eSchedBool ret = SCHED_FALSE;

    if (queue->count > 0)
    {
    size_t const esize = queue->esize;
    size_t off = (size_t)(queue->out);

        queue->out++;
        if (queue->out == queue->total)
        {
            queue->out = 0;
        }
        off *= esize;
        memcpy(buffer, &queue->buf[off], esize);
        queue->count--;
        ret = SCHED_TRUE;
    }
    return (ret);
}

/**
 * 判断指定的事件队列是否为空
 *
 * @param queue: 指定的事件队列指针
 *
 * @return: 布尔值
 *          SCHED_TRUE  事件队列为空
 *          SCHED_FALSE 事件队列非空
 */
eSchedBool schedIntQueueIsEmpty(sSchedQueue *queue)
{
    if (0 == queue->count)
    {
        return (SCHED_TRUE);
    }
    else
    {
        return (SCHED_FALSE);
    }
}

/**
 * 判断指定的事件队列是否已满
 *
 * @param queue: 指定的事件队列指针
 *
 * @return: 布尔值
 *          SCHED_TRUE  事件队列已满
 *          SCHED_FALSE 事件队列未满
 */
eSchedBool schedIntQueueIsFull(sSchedQueue *queue)
{
    if (queue->total == queue->count)
    {
        return (SCHED_TRUE);
    }
    else
    {
        return (SCHED_FALSE);
    }
}

/*******************************************************************************

                                   有限状态机

*******************************************************************************/
/* 状态机内部事件 ------------------------------------------------------------*/
static const SchedEvent_t internal_event[3] =
{
    {SCHED_SIG_EMPTY,   0},
    {SCHED_SIG_ENTRY,   0},
    {SCHED_SIG_EXIT,    0}
};

/* 状态机函数 ----------------------------------------------------------------*/
/* 构造有限状态机 */
void schedIntFSMCtor(sSchedFSM *fsm, SchedEventHandler initial)
{
    fsm->state = initial;
}

/* 初始化状态机, 实现初始化状态转移 */
void schedIntFSMInit(sSchedFSM *fsm, void *me)
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
void schedIntFSMDispatch(sSchedFSM *fsm, void *me, SchedEvent_t const *e)
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
