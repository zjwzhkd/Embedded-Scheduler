/*******************************************************************************
* 文 件 名: sched_priotbl.h
* 创 建 者: Keda Huang
* 版    本: V1.0
* 创建日期: 2017-01-08
* 文件说明: 内部数据结构 -- 优先级记录表
*******************************************************************************/

#ifndef __SCHED_PRIOTBL_H
#define __SCHED_PRIOTBL_H

/* 头文件 --------------------------------------------------------------------*/
#include "sched_prototype.h"

/* 数据类型 ------------------------------------------------------------------*/
/* 优先级记录表 */
typedef struct sched_priotbl
{
    uint8_t         tbl[SCHED_PRIOTBL_TBL_SIZE];
    uint8_t         grp;
} sSchedPriotbl;

/* 常量定义 ------------------------------------------------------------------*/
/* 优先级记录表的最低优先级 */
#define SCHED_PRIOTBL_LOWEST_PRIO           ( 8*SCHED_PRIOTBL_TBL_SIZE - 1 )

/* 宏定义 --------------------------------------------------------------------*/
/*
 * 判断优先级记录表是否为空
 * _priotbl_: 优先级记录表指针
 * return:    布尔值
 */
#define SCHED_PRIOTBL_IS_EMPTY(_priotbl_)   ( (_priotbl_)->grp == 0 )

/* 操作函数 ------------------------------------------------------------------*/
void schedIntPriotblInit(sSchedPriotbl *priotbl);
void schedIntPriotblRecord(sSchedPriotbl *priotbl, uint8_t prio);
void schedIntPriotblReset(sSchedPriotbl *priotbl, uint8_t prio);
uint8_t schedIntPriotblGetHighest(sSchedPriotbl *priotbl);
eSchedBool schedIntPriotblIsEmpty(sSchedPriotbl *priotbl);

#endif  /* __SCHED_PRIOTBL_H */
