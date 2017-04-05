/*******************************************************************************
* 文 件 名: sched_heap_1.c
* 创 建 者: Keda Huang
* 版    本: V1.0
* 创建日期: 2017-04-04
* 文件说明: 动态内存分配 -- 封装C标准库的动态内存分配函数
*******************************************************************************/

#include "sched_proto.h"
#include <stdlib.h>

void schedPortHeapInit(void)
{

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
