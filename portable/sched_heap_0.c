/*******************************************************************************
* 文 件 名: sched_heap_0.c
* 创 建 者: Keda Huang
* 版    本: V1.0
* 创建日期: 2017-07-06
* 文件说明: 动态内存分配 -- 实现无效的动态内存分配
*******************************************************************************/

#include "sched_proto.h"

void schedPortHeapInit(void)
{

}

void *schedPortMalloc(size_t nsize)
{
    return (NULL);
}

void schedPortFree(void *pv)
{
    SCHED_ASSERT(pv == NULL);
}
