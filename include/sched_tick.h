/*******************************************************************************
* 文 件 名: sched_tick.h
* 创 建 者: Keda Huang
* 版    本: V1.0
* 创建日期: 2017-02-21
* 文件说明: 事件驱动调度器的节拍管理
*******************************************************************************/

#ifndef __SCHED_TICK_H
#define __SCHED_TICK_H

/* 头文件 --------------------------------------------------------------------*/
#include "sched_prototype.h"
#include "sched_core_struct.h"

/* 节拍管理函数 --------------------------------------------------------------*/
void schedTickInit(void);
void schedTickHandler(void);
SchedTick_t schedTickGetCount(void);
SchedTick_t schedTickGetCountFromISR(void);

/* 延时管理宏 ----------------------------------------------------------------*/
/* 判断延时对象是否空闲(未加入延时链表)
 *
 * _pdelay_: 延时管理结构体指针
 *
 * return: 布尔值
 */
#define SCHED_DELAY_IS_IDLE(_pdelay_)   \
    ( SCHED_LIST_IS_EMPTY(&(_pdelay_)->item) )

/* 延时管理函数 --------------------------------------------------------------*/
/* @note: 1. 除了初始化函数, 延时管理函数必须在临界区内调用
 *        2. 作为调度器内部组成部分, 延时管理函数仅供内部函数调用
 */
void schedDelayInit(sSchedDelay *pDelay, SchedDelayCB cb);
void schedDelayAdd(sSchedDelay *pDelay, SchedTick_t period);
void schedDelayRemove(sSchedDelay *pDelay);

#endif  /* __SCHED_TICK_H */
