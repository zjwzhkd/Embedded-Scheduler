/*******************************************************************************
* 文 件 名: sched_priotbl.c
* 创 建 者: Keda Huang
* 版    本: V1.0
* 创建日期: 2017-01-08
* 文件说明: 内部数据结构 -- 优先级记录表
*******************************************************************************/

#include "sched_internal.h"
/*******************************************************************************

                                    常量数组

*******************************************************************************/
static uint8_t const priotbl_unmap[] =
{
    0u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0x00 to 0x0F */
    4u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0x10 to 0x1F */
    5u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0x20 to 0x2F */
    4u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0x30 to 0x3F */
    6u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0x40 to 0x4F */
    4u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0x50 to 0x5F */
    5u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0x60 to 0x6F */
    4u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0x70 to 0x7F */
    7u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0x80 to 0x8F */
    4u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0x90 to 0x9F */
    5u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0xA0 to 0xAF */
    4u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0xB0 to 0xBF */
    6u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0xC0 to 0xCF */
    4u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0xD0 to 0xDF */
    5u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0xE0 to 0xEF */
    4u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u  /* 0xF0 to 0xFF */
};

/*******************************************************************************

                                    操作函数

*******************************************************************************/

/**
 * 初始化优先级记录表
 *
 * @param priotbl: 待出始化的优先级记录表指针
 */
void schedIntPriotblInit(sSchedPriotbl *priotbl)
{
uint8_t i;

    for (i = 0; i < SCHED_PRIOTBL_TBL_SIZE; i++)
    {
        priotbl->tbl[i] = 0;
    }
    priotbl->grp = 0;
}

/**
 * 在指定的优先级记录表中, 记录一个优先级
 *
 * @param priotbl: 指定的优先级记录表指针
 *
 * @param prio: 记录的优先级, 限制范围 0 - SCHED_PRIOTBL_LOWEST_PRIO
 */
void schedIntPriotblRecord(sSchedPriotbl *priotbl, uint8_t prio)
{
    if (prio <= SCHED_PRIOTBL_LOWEST_PRIO)
    {
    uint8_t x = prio & 0x07;
    uint8_t y = prio >> 3;

        priotbl->tbl[y] |= (uint8_t)1<<x;
        priotbl->grp    |= (uint8_t)1<<y;
    }
    else
    {
        /* 断言: 记录的优先级没有超出限制范围 */
        SCHED_ASSERT(0);
    }
}

/**
 * 在指定的优先级记录表中, 清除一个优先级
 *
 * @param priotbl: 指定的优先级记录表指针
 *
 * @param prio: 清除的优先级, 限制范围 0 - SCHED_PRIOTBL_LOWEST_PRIO
 */
void schedIntPriotblReset(sSchedPriotbl *priotbl, uint8_t prio)
{
    if (prio <= SCHED_PRIOTBL_LOWEST_PRIO)
    {
    uint8_t x = prio & 0x07;
    uint8_t y = prio >> 3;

        priotbl->tbl[y]  &= ~( (uint8_t)1 << x );
        if (0 == priotbl->tbl[y])
        {
            priotbl->grp &= ~( (uint8_t)1 << y );
        }

    }
    else
    {
        /* 断言: 清除的优先级没有超出限制范围 */
        SCHED_ASSERT(0);
    }
}

/**
 * 获取指定的优先级记录表中记录的最高优先级
 *
 * @note: 在调用本函数获取最高优先级之前, 需要确保优先级记录表非空
 *
 * @param priotbl: 指定的优先级记录表指针
 *
 * @return: 若优先级记录表非空, 返回最高优先级
 *          若优先级记录表为空, 返回0
 */
uint8_t schedIntPriotblGetHighest(sSchedPriotbl *priotbl)
{
uint8_t prio;
uint8_t x,y;

    y = priotbl_unmap[priotbl->grp];
    SCHED_ASSERT(y < SCHED_PRIOTBL_TBL_SIZE);
    SCHED_ASSERT(priotbl->tbl[y] > 0);
    x = priotbl_unmap[priotbl->tbl[y]];
    prio = (y<<3) + x;
    return (prio);
}

/**
 * 判断指定的优先级记录表是否为空
 *
 * @param priotbl: 指定的优先级记录表指针
 *
 * @return: 布尔值
 *          SCHED_TRUE  表示优先级记录表为空
 *          SCHED_FALSE 表示优先级记录表非空
 */
eSchedBool schedIntPriotblIsEmpty(sSchedPriotbl *priotbl)
{
    if (0 == priotbl->grp)
    {
        return SCHED_TRUE;
    }
    else
    {
        return SCHED_FALSE;
    }
}
