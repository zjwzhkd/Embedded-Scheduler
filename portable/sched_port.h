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

/* 接口数据类型 --------------------------------------------------------------*/
/* 架构基本类型 */
typedef base_t SchedBase_t;

/* CPU状态类型 */
typedef cpu_t  SchedCPU_t;

/* 节拍计数类型 */
#if SCHED_16BIT_TICK_EN
    typedef uint16_t SchedTick_t;
    #define SCHED_TICK_MAX  ( (SchedTick_t)0xFFFF )
#else
    typedef uint32_t SchedTick_t;
    #define SCHED_TICK_MAX  ( (SchedTick_t)0xFFFFFFFF )
#endif

/* 事件数据类型 --------------------------------------------------------------*/
/* 事件队列偏移类型 */
#if   SCHED_EVTPOS_TYPE == 0
    typedef uint8_t     EvtPos_t;
#elif SCHED_EVTPOS_TYPE == 1
    typedef size_t      EvtPos_t;
#else
    #error config 'SCHED_EVTPOS_TYPE' is error.
#endif

/* 事件信号变量类型 */
#if   SCHED_EVTSIG_TYPE == 0
    typedef uint8_t     EvtSig_t;
#elif SCHED_EVTSIG_TYPE == 1
    typedef uint16_t    EvtSig_t;
#elif SCHED_EVTSIG_TYPE == 2
    typedef uint32_t    EvtSig_t;
#else
    #error config 'SCHED_EVTSIG_TYPE' is error.
#endif

/* 事件消息变量类型 */
#if   SCHED_EVTMSG_TYPE == 0
    typedef uint8_t     EvtMsg_t;
#elif SCHED_EVTMSG_TYPE == 1
    typedef uint16_t    EvtMsg_t;
#elif SCHED_EVTMSG_TYPE == 2
    typedef uint32_t    EvtMsg_t;
#else
    #error config 'SCHED_EVTMSG_TYPE' is error.
#endif

/* 调度器事件变量类型 */
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
