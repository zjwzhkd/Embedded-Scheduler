/*******************************************************************************
* 文 件 名: sched_port.h
* 创 建 者: Keda Huang
* 版    本: V1.0
* 创建日期: 2016-07-25
* 文件说明: 事件驱动调度器的底层相关定义
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

#endif  /* __SCHED_PORT_H */
