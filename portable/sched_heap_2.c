/*******************************************************************************
* 文 件 名: sched_heap_2.c
* 创 建 者: Keda Huang
* 版    本: V1.0
* 创建日期: 2017-04-04
* 文件说明: 动态内存分配 -- 仅实现内存分配函数
*******************************************************************************/

#include "sched_proto.h"
/*******************************************************************************

                                    全局变量

*******************************************************************************/
#define ADJUSTED_HEAP_SIZE      ( SCHED_HEAP_SIZE - SCHED_BYTE_ALIGNMENT )
static uint8_t  heapMemory[SCHED_HEAP_SIZE];
static uint8_t *heapAlignedStart = NULL;
static size_t   heapNextFreeByte = 0;

/*******************************************************************************

                                    内存分配

*******************************************************************************/
void schedPortHeapInit(void)
{
    SCHED_ASSERT(SCHED_HEAP_SIZE > SCHED_BYTE_ALIGNMENT);
    heapNextFreeByte = 0;
    if ( ( ((size_t)heapMemory)&SCHED_BYTE_ALIGNMENT_MASK ) != 0 )
    {
        heapAlignedStart = heapMemory + SCHED_BYTE_ALIGNMENT - ( ((size_t)heapMemory)&SCHED_BYTE_ALIGNMENT_MASK );
    }
    else
    {
        heapAlignedStart = heapMemory;
    }
}

void *schedPortMalloc(size_t nsize)
{
void *ret = NULL;

    if ( (nsize&SCHED_BYTE_ALIGNMENT_MASK) != 0 )
    {
        nsize += SCHED_BYTE_ALIGNMENT - (nsize&SCHED_BYTE_ALIGNMENT_MASK);
    }

    if ( (nsize+heapNextFreeByte < ADJUSTED_HEAP_SIZE) &&
         (nsize+heapNextFreeByte > heapNextFreeByte) )
    {
        ret = heapAlignedStart + heapNextFreeByte;
        heapNextFreeByte += nsize;
    }

    SCHED_ASSERT(ret != NULL);
    SCHED_ASSERT((((size_t)ret)&SCHED_BYTE_ALIGNMENT_MASK) == 0);
    return (ret);
}

void schedPortFree(void *pv)
{
    SCHED_ASSERT(pv == NULL);
}
