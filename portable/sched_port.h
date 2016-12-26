/*******************************************************************************
* 文 件 名: sched_port.h
* 创 建 者: Keda Huang
* 版    本: V1.0
* 创建日期: 2016-11-14
* 文件说明: 事件驱动调度器的底层接口
*******************************************************************************/

#ifndef __SCHED_PORT_H
#define __SCHED_PORT_H

/* 头文件 --------------------------------------------------------------------*/
#include "cpu.h"

/* 数据类型 ------------------------------------------------------------------*/
/* 架构基本类型 */
typedef base_t SchedBase_t;

/* CPU状态类型 */
typedef cpu_t  SchedCPU_t;

/* 调度器事件 */
typedef uint8_t  EvtPos_t;  /* 事件队列偏移类型 */
typedef uint32_t EvtSig_t;  /* 事件信号数据类型 */
typedef uint32_t EvtMsg_t;  /* 事件消息数据类型 */
typedef struct sched_event
{
    EvtSig_t        sig;
    EvtMsg_t        msg;
} SchedEvent_t;

/* 宏定义 --------------------------------------------------------------------*/
/* 临界区管理 */
#define SCHED_EnterCritical()           CPU_EnterCritical()
#define SCHED_ExitCritical(x)           CPU_ExitCritical(x)
#define SCHED_EnterCriticalFromISR()    CPU_EnterCriticalFromISR()
#define SCHED_ExitCriticalFromISR(x)    CPU_ExitCriticalFromISR(x)

/* 调度器断言 */
#if SCHED_ASSERT_EN
    #define SCHED_ASSERT(expr)          CPU_Assert(expr)
#else
    #define SCHED_ASSERT(expr)          ((void)0)
#endif

/* 底层接口 ------------------------------------------------------------------*/
/* 调度器底层函数 */
void schedPortInit(void);

/* 事件块操作函数 */
SchedEvent_t *schedPortEvtcpy(SchedEvent_t *dst, SchedEvent_t const *src);
SchedEvent_t *schedPortEvtfill(SchedEvent_t *dst, EvtSig_t sig, EvtMsg_t msg);

/* 内存分配函数 */
void *schedPortMalloc(size_t nsize);
void schedPortFree(void *pv);

#endif  /* __SCHED_PORT_H */
