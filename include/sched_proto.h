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
#include "sched_port.h"

/* 数据类型 ------------------------------------------------------------------*/
/* 调度器节拍 */
#if SCHED_16BIT_TICK_EN
    typedef uint16_t SchedTick_t;
    #define SCHED_TICK_MAX  ( (SchedTick_t)0xFFFF )
#else
    typedef uint32_t SchedTick_t;
    #define SCHED_TICK_MAX  ( (SchedTick_t)0xFFFFFFFF )
#endif

/* 调度器对象句柄 */
typedef void * SchedHandle_t;

/* 调度器事件 */
typedef uint8_t  EvtPos_t;  /* 事件队列偏移类型 */
typedef uint16_t EvtSig_t;  /* 事件信号数据类型 */
typedef uint32_t EvtMsg_t;  /* 事件消息数据类型 */
typedef struct sched_event
{
    EvtSig_t    sig;
    EvtMsg_t    msg;
} SchedEvent_t;

/* 调度器事件处理函数 */
typedef SchedBase_t (*SchedEventHandler)(SchedHandle_t me, SchedEvent_t *e);

/* 调度器布尔值 */
typedef enum {SCHED_FALSE = 0, SCHED_TRUE = 1} eSchedBool;

/* 调度器错误代码 */
typedef enum
{
    SCHED_OK = 0,
    SCHED_ERR_UNKNOWN
} eSchedError;

/* 调度器核心状态 */
typedef enum
{
    SCHED_CORE_UNINITIAL = 0,   /* 调度器未初始化 */
    SCHED_CORE_STOP,            /* 调度器停止状态 */
    SCHED_CORE_RUNNING,         /* 调度器正在运行 */
} eSchedCoreState;

/* 常量定义 ------------------------------------------------------------------*/
/* 内部信号 */
enum {
    SCHED_SIG_EMPTY = 0,        /* 初始化空信号 */
    SCHED_SIG_ENTRY,            /* 状态进入信号 */
    SCHED_SIG_EXIT,             /* 状态退出信号 */
    SCHED_SIG_LOOP,             /* 状态循环信号 */
    SCHED_SIG_USER              /* 用户起始信号 */
};

/* 状态处理函数返回值 */
#define SCHED_RET_HANDLED       ( (SchedBase_t)0 )
#define SCHED_RET_IGNORED       ( (SchedBase_t)1 )
#define SCHED_RET_TRAN          ( (SchedBase_t)2 )

/* 宏定义 --------------------------------------------------------------------*/
/* 时间转换宏 */
#define SCHED_MS_TO_TICK(nms)   ( (SchedTick_t)((uint32_t)(nms)*SCHED_TICK_HZ/1000) )
#define SCHED_HZ_TO_TICK(nhz)   ( (SchedTick_t)(SCHED_TICK_HZ/(nhz)) )

/* 底层接口 ------------------------------------------------------------------*/
void schedPortInit(void);
SchedEvent_t *schedPortEvtcpy(SchedEvent_t *dst, const SchedEvent_t *src);
void schedPortHeapInit(void);
void *schedPortMalloc(size_t nsize);
void schedPortFree(void *pv);
void schedPortIdleHook(void);

#endif  /* __SCHED_PROTO_H */
