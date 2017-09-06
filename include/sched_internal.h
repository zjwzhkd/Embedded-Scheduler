/*******************************************************************************
* 文 件 名: sched_internal.h
* 创 建 者: Keda Huang
* 版    本: V1.0
* 创建日期: 2017-09-06
* 文件说明: 内部数据结构的实现
*******************************************************************************/

#ifndef __SCHED_INTERNAL_H
#define __SCHED_INTERNAL_H

#include "sched_prototype.h"
#include "sched_internal_struct.h"
/*******************************************************************************

                                  双向排序链表

*******************************************************************************/
/* 宏定义 --------------------------------------------------------------------*/
/*
 * 获取包含链表项的结构体指针
 * _pitem_:  链表项指针
 * _type_:   包含链表项的结构体类型
 * _member_: 链表项在结构体中的成员变量名称
 * return:   结构体指针
 */
#define SCHED_LIST_ENTRY(_pitem_, _type_, _member_) \
    container_of(_pitem_, _type_, _member_)

/*
 * 遍历指定链表
 * _pos_:  遍历链表项指针
 * _head_: 链表头结点指针
 */
#define SCHED_LIST_FOR_EACH(_pos_, _head_)  \
    for (_pos_ = (_head_)->next; _pos_ != (_head_); _pos_ = _pos_->next)

/*
 * 设置链表项排序值
 * _pitem_: 链表项指针
 * _value_: 链表项排序值
 */
#define SCHED_LIST_SET_VALUE(_pitem_, _value_)  (_pitem_)->value = (_value_)

/*
 * 获取链表项排序值
 * _pitem_: 链表项指针
 * return:  链表项排序值
 */
#define SCHED_LIST_GET_VALUE(_pitem_)           ( (_pitem_)->value )

/*
 * 获取链表后一项
 * _pitem_: 链表项指针
 * return:  链表后一项指针
 */
#define SCHED_LIST_NEXT(_pitem_)                ( (_pitem_)->next )

/*
 * 获取链表前一项
 * _pitem_: 链表项指针
 * return:  链表前一项指针
 */
#define SCHED_LIST_PREV(_pitem_)                ( (_pitem_)->prev )

/*
 * 判断是否为空链表
 * _plist_: 链表指针
 * return:  布尔值
 */
#define SCHED_LIST_IS_EMPTY(_plist_)            ( (_plist_)->next == (_plist_) )

/* 操作函数 ------------------------------------------------------------------*/
void schedIntListInit(sSchedList *list);
void schedIntListInsert(sSchedList *list, sSchedList *item);
void schedIntListInsertEnd(sSchedList *list, sSchedList *item);
void schedIntListRemove(sSchedList *item);
eSchedBool schedIntListIsEmpty(sSchedList *list);

/*******************************************************************************

                                  优先级记录表

*******************************************************************************/
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

/*******************************************************************************

                                    事件队列

*******************************************************************************/
/* 宏定义 --------------------------------------------------------------------*/
/*
 * 判断事件队列是否空
 * _queue_: 事件队列指针
 * return:  布尔值
 */
#define SCHED_QUEUE_IS_EMPTY(_queue_)   ( (_queue_)->count == 0 )

/*
 * 判断事件队列是否满
 * _queue_: 事件队列指针
 * return:  布尔值
 */
#define SCHED_QUEUE_IS_FULL(_queue_)    ( (_queue_)->count == (_queue_)->total )

/* 操作函数 ------------------------------------------------------------------*/
sSchedQueue *schedIntQueueCreate(EvtPos_t nMemb, size_t size);
void schedIntQueueDelete(sSchedQueue *queue);
eSchedBool schedIntQueueSend(sSchedQueue *queue, const void *pItem);
eSchedBool schedIntQueueSendToFront(sSchedQueue *queue, const void *pItem);
eSchedBool schedIntQueueReceive(sSchedQueue *queue, void *buffer);
eSchedBool schedIntQueueIsEmpty(sSchedQueue *queue);
eSchedBool schedIntQueueIsFull(sSchedQueue *queue);

/*******************************************************************************

                                   有限状态机

*******************************************************************************/
/* 状态机函数 ----------------------------------------------------------------*/
void schedIntFSMCtor(sSchedFSM *fsm, SchedEventHandler initial);
void schedIntFSMInit(sSchedFSM *fsm, void *me);
void schedIntFSMDispatch(sSchedFSM *fsm, void *me, SchedEvent_t const *e);

#endif  /* __SCHED_INTERNAL_H */
