/*******************************************************************************
* 文 件 名: sched_queue.c
* 创 建 者: Keda Huang
* 版    本: V1.0
* 创建日期: 2017-01-08
* 文件说明: 内部数据结构 -- 事件队列
*******************************************************************************/

#include "sched_queue.h"
#include <string.h>
/*******************************************************************************

                                    操作函数

*******************************************************************************/

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
