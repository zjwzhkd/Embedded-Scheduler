# 调度器原型定义
* **处理器无关**的调度器原型定义

## 原型定义
* `SchedTick_t`
    * 调度器节拍类型
    * 配置项`SCHED_16BIT_TICK_EN`决定调度器是否使用16bit节拍, **默认32bit**

* `SchedPrio_t`
    * 调度器任务优先级

* `SchedHandle_t`
    * 调度器对象句柄
    * 所有调度器对象对应着唯一的句柄
    * 调度器API函数使用句柄引用调度器对象

* `SchedEventHandler`
    * 调度器事件处理函数类型
    * 原型定义

        ```c
        typedef SchedBase_t (*SchedEventHandler)(SchedHandle_t me, SchedEvent_t const * const e);
        ```

* `eSchedBool`
    * 调度器布尔值类型

* `eSchedFlag`
    * 调度器标志类型

* `eSchedError`
    * 调度器错误代码
    * 用于调度器API函数返回值

## 常量定义
* 调度器错误代码

    ```c
    typedef enum
    {
        SCHED_OK = 0,
        /* 通用错误代码 */
        SCHED_ERR_HANDLE_NULL,
        SCHED_ERR_PARAM,
        SCHED_ERR_CREATE_FAILED,
        /* 未知错误代码 */
        SCHED_ERR_UNKNOWN
    } eSchedError;
    ```

* 内部信号

    ```c
    enum {
        SCHED_SIG_EMPTY = 0,        /* 初始化空信号 */
        SCHED_SIG_ENTRY,            /* 状态进入信号 */
        SCHED_SIG_EXIT,             /* 状态退出信号 */
        SCHED_SIG_USER              /* 用户起始信号 */
    };
    ```

* 事件处理函数返回值

    ```c
    #define SCHED_RET_HANDLED       ( (SchedBase_t)0 )
    #define SCHED_RET_IGNORED       ( (SchedBase_t)1 )
    #define SCHED_RET_TRAN          ( (SchedBase_t)2 )
    ```
