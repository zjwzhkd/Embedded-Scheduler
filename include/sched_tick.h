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
#include "sched_proto.h"
#include "sched_list.h"

/* 节拍管理函数 --------------------------------------------------------------*/
void schedTickInit(void);
void schedTickHandler(void);
SchedTick_t schedTickGetCount(void);
SchedTick_t schedTickGetCountFromISR(void);

/* 延时管理类型 --------------------------------------------------------------*/
struct sched_delay;
typedef struct sched_delay sSchedDelay;

/* 延时回调函数原型
 *
 * @param me: 指向延时结束的延时管理结构体
 *
 * @return: 继续延时的节拍数, 若为0, 则表示不继续延时
 */
typedef SchedTick_t (*SchedDelayCB)(sSchedDelay *me);

/* 延时管理结构体 */
struct sched_delay
{
    sSchedList      item;   /* 节拍延时链表项 */
    SchedDelayCB    cb;     /* 延时回调函数   */
};

/* 延时管理函数 --------------------------------------------------------------*/
/* @note: 1. 除了初始化函数, 延时管理函数必须在临界区内调用
 *        2. 作为调度器内部组成部分, 延时管理函数仅供内部函数调用
 */
void schedDelayInit(sSchedDelay *pDelay, SchedDelayCB cb);
void schedDelayAdd(sSchedDelay *pDelay, SchedTick_t period);
void schedDelayRemove(sSchedDelay *pDelay);

#endif  /* __SCHED_TICK_H */
