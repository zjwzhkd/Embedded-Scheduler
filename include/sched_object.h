/*******************************************************************************
* 文 件 名: sched_object.h
* 创 建 者: Keda Huang
* 版    本: V1.0
* 创建日期: 2017-01-10
* 文件说明: 事件驱动调度器的对象管理
*******************************************************************************/

#ifndef __SCHED_OBJECT_H
#define __SCHED_OBJECT_H

/* 头文件 --------------------------------------------------------------------*/
#include "sched_proto.h"

/* 数据类型 ------------------------------------------------------------------*/
/* 调度器对象类型枚举 */
typedef enum
{
    SCHED_OBJ_UNDEFINED = 0,
    SCHED_OBJ_TASK,
    SCHED_OBJ_TIMER,
    SCHED_OBJ_DEVTMR
} eSchedObjType;

/* 调度器基类对象 */
typedef struct sched_base_obj
{
    eSchedObjType       type;
} sSchedBaseObj;

/* 宏定义 --------------------------------------------------------------------*/
#if SCHED_BASE_OBJ_EN

    #define SCHED_BASE_OBJ_DEFINE(_obj_)    sSchedBaseObj   _obj_;

    #define SCHED_BASE_OBJ_ASSERT(_HANDLE_, _type_)     do                  \
    {                                                                       \
        SCHED_ASSERT( ((sSchedBaseObj *)(_HANDLE_))->type == (_type_) );    \
    } while(0)

    #define SCHED_BASE_OBJ_INITIAL(_pobj_, _type_)      do                  \
    {                                                                       \
        sSchedBaseObj *_tmp_obj = (sSchedBaseObj *)(_pobj_);                \
        _tmp_obj->type = (_type_);                                          \
    } while(0)

#else

    #define SCHED_BASE_OBJ_DEFINE(_obj_)
    #define SCHED_BASE_OBJ_ASSERT(_HANDLE_, _type_)
    #define SCHED_BASE_OBJ_INITIAL(_pobj_, _type_)

#endif

#endif  /* __SCHED_OBJECT_H */
