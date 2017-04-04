/*******************************************************************************
* 文 件 名: sched_core.h
* 创 建 者: Keda Huang
* 版    本: V1.0
* 创建日期: 2017-02-21
* 文件说明: 事件驱动调度器的核心管理
*******************************************************************************/

#ifndef __SCHED_CORE_H
#define __SCHED_CORE_H

/* 调度器核心状态 ------------------------------------------------------------*/
typedef enum
{
    SCHED_CORE_UNINITIALIZED = 0,   /* 调度器未初始化 */
    SCHED_CORE_STOP,                /* 调度器停止状态 */
    SCHED_CORE_RUNNING,             /* 调度器正在运行 */
} eSchedCoreState;

extern eSchedCoreState schedCoreCurrentState;

#define SCHED_CURRENT_STATE()       ( schedCoreCurrentState )
#define SCHED_CORE_IS_RUNNING()     ( schedCoreCurrentState == SCHED_CORE_RUNNING )
#define SCHED_CORE_IS_STOPPED()     ( schedCoreCurrentState == SCHED_CORE_STOP )

#endif  /* __SCHED_CORE_H */
