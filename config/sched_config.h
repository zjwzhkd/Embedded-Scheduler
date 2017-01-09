/*******************************************************************************
* 文 件 名: sched_config.h
* 创 建 者: Keda Huang
* 版    本: V1.0
* 创建日期: 2016-11-14
* 文件说明: 事件驱动调度器的配置
*******************************************************************************/

#ifndef __SCHED_CONFIG_H
#define __SCHED_CONFIG_H

/* 调度器参数 ----------------------------------------------------------------*/
/* 调度器节拍频率 */
#define SCHED_TICK_HZ               ( CPU_TICK_HZ )
/* 调度器最低优先级 */
#define SCHED_LOWEST_PRIORITY       ( 3 )
/* 调度器内存分配大小 */
#define SCHED_HEAP_SIZE             ( 1000 )
/* 处理器字节对齐 */
#define SCHED_BYTE_ALIGNMENT        ( CPU_BYTE_ALIGNMENT )
/* 优先级记录表大小 */
#define SCHED_PRIOTBL_TBL_SIZE      ( 3 )

/* 调度器配置 ----------------------------------------------------------------*/
/* 节拍大小配置
 * 0 - 使用32bit节拍
 * 1 - 使用16bit节拍
 */
#define SCHED_16BIT_TICK_EN         ( 0 )
/* 调度器断言配置
 * 0 - 禁止调度器断言
 * 1 - 使能调度器断言
 */
#define SCHED_ASSERT_EN             ( 1 )
/* 空闲任务Hook配置
 * 0 - 禁止空闲任务Hook
 * 1 - 使能空闲任务Hook
 */
#define SCHED_IDLE_HOOK_EN          ( 0 )

#endif  /* __SCHED_CONFIG_H */
