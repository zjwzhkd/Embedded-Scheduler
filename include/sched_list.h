/*******************************************************************************
* 文 件 名: sched_list.h
* 创 建 者: Keda Huang
* 版    本: V1.0
* 创建日期: 2016-12-27
* 文件说明: 内部数据结构 -- 双向排序链表
*******************************************************************************/

#ifndef __SCHED_LIST_H
#define __SCHED_LIST_H

/* 头文件 --------------------------------------------------------------------*/
#include "sched_prototype.h"
#include "sched_internal_struct.h"

/* 宏定义 --------------------------------------------------------------------*/
/* 通过结构体成员变量指针来获得结构体指针 */
#ifndef container_of
    #define container_of(ptr, type, member) \
        ( (type *)((char *)(ptr) - offsetof(type, member)) )
#endif

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

#endif  /* __SCHED_LIST_H */
