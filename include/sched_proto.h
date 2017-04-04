/*******************************************************************************
* 文 件 名: sched_proto.h
* 创 建 者: Keda Huang
* 版    本: V1.0
* 创建日期: 2016-11-14
* 文件说明: 事件驱动调度器的原型定义
*******************************************************************************/

#ifndef __SCHED_PROTO_H
#define __SCHED_PROTO_H

/* 头文件 --------------------------------------------------------------------*/
#include <stdint.h>
#include <stddef.h>
#include "sched_config.h"
#include "sched_portable.h"

/* 调度器数据类型 ------------------------------------------------------------*/

/* 调度器任务优先级 */
typedef SchedTick_t SchedPrio_t;

/* 调度器对象句柄 */
typedef void * SchedHandle_t;

/* 调度器事件处理函数 */
typedef SchedBase_t (*SchedEventHandler)(SchedHandle_t me, SchedEvent_t const * const e);

/* 调度器布尔值 */
typedef enum {SCHED_FALSE = 0, SCHED_TRUE = 1} eSchedBool;

/* 调度器标志 */
typedef enum {SCHED_RESET = 0, SCHED_SET = 1} eSchedFlag;

/* 调度器常量定义 ------------------------------------------------------------*/
/* 调度器错误代码 */
typedef enum
{
    SCHED_OK = 0,
    /* 通用错误代码 */
    SCHED_ERR_HANDLE_NULL,
    SCHED_ERR_PARAM,
    SCHED_ERR_CREATE_FAILED,
    /* 核心状态错误代码 */
    SCHED_ERR_CORE_STATE,
    /* 事件错误代码 */
    SCHED_ERR_EVENT_SEND_FAILED,
    /* 未知错误代码 */
    SCHED_ERR_UNKNOWN
} eSchedError;

/* 定时器运行模式 */
typedef enum
{
    SCHED_TIMER_ONCE = 0,       /* 定时器单次运行模式 */
    SCHED_TIMER_RELOAD          /* 定时器自动重载模式 */
} eSchedTimerMode;

/* 定时器状态 */
typedef enum
{
    SCHED_TIMER_RESET = 0,      /* 定时器复位状态 */
    SCHED_TIMER_RUNNING,        /* 定时器运行状态 */
    SCHED_TIMER_ARRIVAL         /* 定时器到达状态 */
} eSchedTimerStatus;

/* 内部信号 */
enum {
    SCHED_SIG_EMPTY = 0,        /* 初始化空信号 */
    SCHED_SIG_ENTRY,            /* 状态进入信号 */
    SCHED_SIG_EXIT,             /* 状态退出信号 */
    SCHED_SIG_USER              /* 用户起始信号 */
};

/* 事件处理函数返回值 */
#define SCHED_RET_HANDLED       ( (SchedBase_t)0 )
#define SCHED_RET_IGNORED       ( (SchedBase_t)1 )
#define SCHED_RET_TRAN          ( (SchedBase_t)2 )

#endif  /* __SCHED_PROTO_H */
