/*******************************************************************************
* 文 件 名: sched_core.c
* 创 建 者: Keda Huang
* 版    本: V1.0
* 创建日期: 2017-02-21
* 文件说明: 事件驱动调度器的核心管理
*******************************************************************************/

#include "sched_core.h"
/*******************************************************************************

                                    全局变量

*******************************************************************************/
eSchedCoreState schedCoreCurrentState = SCHED_CORE_UNINITIALIZED;