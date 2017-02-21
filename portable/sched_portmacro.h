/*******************************************************************************
* 文 件 名: sched_portmacro.h
* 创 建 者: Keda Huang
* 版    本: V1.0
* 创建日期: 2017-02-16
* 文件说明: 事件驱动调度器的底层移植接口
*******************************************************************************/

#ifndef __SCHED_PORTMACRO_H
#define __SCHED_PORTMACRO_H

/* 头文件 --------------------------------------------------------------------*/
#include "cpu.h"

/* 架构变量类型 --------------------------------------------------------------*/
/* CPU基本类型 */
typedef base_t SchedBase_t;

/* CPU状态类型 */
typedef cpu_t  SchedCPU_t;

/* 临界区管理 ----------------------------------------------------------------*/
#define SCHED_EnterCritical()           CPU_EnterCritical()
#define SCHED_ExitCritical(x)           CPU_ExitCritical(x)
#define SCHED_EnterCriticalFromISR()    CPU_EnterCriticalFromISR()
#define SCHED_ExitCriticalFromISR(x)    CPU_ExitCriticalFromISR(x)

/* 调度器断言 ----------------------------------------------------------------*/
#if SCHED_ASSERT_EN
    #define SCHED_ASSERT(expr)          CPU_Assert(expr)
#else
    #define SCHED_ASSERT(expr)          ((void)0)
#endif

#endif  /* __SCHED_PORTMACRO_H */
