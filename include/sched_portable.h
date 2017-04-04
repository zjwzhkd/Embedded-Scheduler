/*******************************************************************************
* 文 件 名: sched_portable.h
* 创 建 者: Keda Huang
* 版    本: V1.0
* 创建日期: 2017-02-16
* 文件说明: 事件驱动调度器的底层接口定义
*******************************************************************************/

#ifndef __SCHED_PORTABLE_H
#define __SCHED_PORTABLE_H

/* 头文件 --------------------------------------------------------------------*/
#include "sched_portmacro.h"

/* 调度器变量类型 ------------------------------------------------------------*/
/* 节拍计数类型 */
#if SCHED_16BIT_TICK_EN
    typedef uint16_t SchedTick_t;
    #define SCHED_TICK_MAX  ( (SchedTick_t)0xFFFF )
#else
    typedef uint32_t SchedTick_t;
    #define SCHED_TICK_MAX  ( (SchedTick_t)0xFFFFFFFF )
#endif

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

/* 数据字节对齐 --------------------------------------------------------------*/
#if SCHED_BYTE_ALIGNMENT == 32
    #define SCHED_BYTE_ALIGNMENT_MASK   ( 0x001f )
#endif

#if SCHED_BYTE_ALIGNMENT == 16
    #define SCHED_BYTE_ALIGNMENT_MASK   ( 0x000f )
#endif

#if SCHED_BYTE_ALIGNMENT == 8
    #define SCHED_BYTE_ALIGNMENT_MASK   ( 0x0007 )
#endif

#if SCHED_BYTE_ALIGNMENT == 4
    #define SCHED_BYTE_ALIGNMENT_MASK   ( 0x0003 )
#endif

#if SCHED_BYTE_ALIGNMENT == 2
    #define SCHED_BYTE_ALIGNMENT_MASK   ( 0x0001 )
#endif

#if SCHED_BYTE_ALIGNMENT == 1
    #define SCHED_BYTE_ALIGNMENT_MASK   ( 0x0000 )
#endif

#ifndef SCHED_BYTE_ALIGNMENT_MASK
    #error Invalid SCHED_BYTE_ALIGNMENT definition.
#endif

/* 底层初始化函数 ------------------------------------------------------------*/
void schedPortInit(void);

/* 内存分配函数 --------------------------------------------------------------*/
void *schedPortMalloc(size_t nsize);
void schedPortFree(void *pv);

#endif  /* __SCHED_PORTABLE_H */
