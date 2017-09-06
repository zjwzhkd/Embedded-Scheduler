/*******************************************************************************
* 文 件 名: sched_port.c
* 创 建 者: Keda Huang
* 版    本: V1.0
* 创建日期: 2016-11-14
* 文件说明: 事件驱动调度器的底层接口
*******************************************************************************/

#include "sched_prototype.h"
/*******************************************************************************

                                    底层接口

*******************************************************************************/
/* 调度器底层初始化 */
void schedPortInit(void)
{
    schedPortHeapInit();
}
