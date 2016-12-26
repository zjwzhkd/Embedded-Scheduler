/*******************************************************************************
* 文 件 名: sched_port.c
* 创 建 者: Keda Huang
* 版    本: V1.0
* 创建日期: 2016-11-14
* 文件说明: 事件驱动调度器的底层接口
*******************************************************************************/

#include "sched_proto.h"
#include <stdlib.h>
/*******************************************************************************

                                    底层接口

*******************************************************************************/
/* 调度器底层初始化 */
void schedPortInit(void)
{

}

/**
 * 事件块复制
 *
 * @param dst: 目标事件块指针
 *
 * @param src: 源事件块指针
 *
 * @return: 目标事件块指针
 */
SchedEvent_t *schedPortEvtcpy(SchedEvent_t *dst, const SchedEvent_t *src)
{
    dst->sig = src->sig;
    dst->msg = src->msg;
    return (dst);
}

/**
 * 事件块填充
 *
 * @param dst: 目标事件块指针
 *
 * @param sig: 事件信号
 *
 * @param msg: 事件消息
 *
 * @return: 目标事件块指针
 */
SchedEvent_t *schedPortEvtfill(SchedEvent_t *dst, EvtSig_t sig, EvtMsg_t msg)
{
    dst->sig = sig;
    dst->msg = msg;
    return (dst);
}

void *schedPortMalloc(size_t nsize)
{
void *ret = malloc(nsize);

    SCHED_ASSERT(ret != NULL);
    return (ret);
}

void schedPortFree(void *pv)
{
    free(pv);
}
