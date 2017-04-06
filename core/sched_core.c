/*******************************************************************************
* 文 件 名: sched_core.c
* 创 建 者: Keda Huang
* 版    本: V1.0
* 创建日期: 2017-02-21
* 文件说明: 事件驱动调度器的核心管理
*******************************************************************************/

#include "sched_core.h"
#include "sched_task.h"
#include "sched_tick.h"
#include "sched_api.h"
/*******************************************************************************

                                    全局变量

*******************************************************************************/
eSchedCoreState schedCoreCurrentState = SCHED_CORE_UNINITIALIZED;

/*******************************************************************************

                                    核心管理

*******************************************************************************/
/* 调度器初始化 */
void schedInit(void)
{
    schedPortInit();
    schedTickInit();
    schedTaskInit();
    schedCoreCurrentState = SCHED_CORE_STOP;
}

/* 启动调度器 */
void schedStart(void)
{
eSchedBool exec;

    schedCoreCurrentState = SCHED_CORE_RUNNING;
    schedTaskInitialiseAll();

    for (;;)
    {
    #if SCHED_LOOP_HOOK_EN
        schedLoopHook();
    #endif
        exec = schedTaskExecute();
        if ( exec == SCHED_FALSE )
        {
        #if SCHED_IDLE_HOOK_EN
            schedIdleHook();
        #endif
        }
    }
}
