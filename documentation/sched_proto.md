# 调度器原型定义
* `sched_proto.h`包含**处理器无关**的调度器原型定义
* `sched_port.h`包含**处理器相关**的调度器原型定义
* `sched_port.c`实现调度器的**底层接口**

## 调度器相关
### 原型定义
* `SchedBase_t`
    * 处理器架构基本类型, **带符号整型**
* `SchedCPU_t`
    * CPU状态数据类型

### 宏定义

```c
/* 临界区管理 */
#define SCHED_EnterCritical()
#define SCHED_ExitCritical(x)
#define SCHED_EnterCriticalFromISR()
#define SCHED_ExitCriticalFromISR(x)

/* 调度器断言 */
#define SCHED_ASSERT(expr)
```

## 处理器无关
### 原型定义
* `SchedTick_t`
    * 调度器节拍类型
    * 配置项`SCHED_16BIT_TICK_EN`决定调度器是否使用16bit节拍(**默认32bit**)

* `SchedHandle_t`
    * 调度器对象句柄
    * 所有调度器对象, 对应着唯一的句柄
    * 调度器API函数, 使用句柄引用调度器对象

* `SchedEvent_t`
    * 调度器事件类型
    * 相关类型定义

        ```c
        typedef uint8_t  EvtPos_t;  /* 事件队列偏移类型 */
        typedef uint16_t EvtSig_t;  /* 事件信号数据类型 */
        typedef uint32_t EvtMsg_t;  /* 事件消息数据类型 */
        ```

* `SchedEventHandler`
    * 调度器事件处理函数类型
    * 原型定义

        ```c
        typedef SchedBase_t (*SchedEventHandler)(SchedHandle_t me, SchedEvent_t *e);
        ```

* `eSchedBool`
    * 布尔值类型
    * 原型定义

        ```c
        typedef enum {SCHED_FALSE = 0, SCHED_TRUE = 1} eSchedBool;
        ```

* `eSchedError`
    * 调度器错误代码
    * 用于调度器API函数返回值

### 常量定义

```c
/* 内部信号 */
enum {
    SCHED_SIG_EMPTY = 0,        /* 初始化空信号 */
    SCHED_SIG_ENTRY,            /* 状态进入信号 */
    SCHED_SIG_EXIT,             /* 状态退出信号 */
    SCHED_SIG_LOOP,             /* 状态循环信号 */
    SCHED_SIG_USER              /* 用户起始信号 */
};

/* 状态处理函数返回值 */
#define SCHED_RET_HANDLED       ( (SchedBase_t)0 )
#define SCHED_RET_IGNORED       ( (SchedBase_t)1 )
#define SCHED_RET_TRAN          ( (SchedBase_t)2 )
```

### 宏定义

```c
/* 时间转换宏 */
#define SCHED_MS_TO_TICK(nms)
#define SCHED_HZ_TO_TICK(nhz)
```


## 底层接口
* 底层函数
    * `void schedPortInit(void)`
        * 调度器底层初始化
    * `SchedEvent_t *schedPortEvtcpy(SchedEvent_t *dst, const SchedEvent_t *src)`
        * 事件块复制
        * dst: 复制目标事件块指针
        * src: 复制源事件块指针
        * return: 返回`dst`, 支持链式调用
* 内存分配
    * `void schedPortHeapInit(void)`
        * 内存分配初始化
    * `void *schedPortMalloc(size_t nsize)`
        * 内存分配函数
    * `void schedPortFree(void *pv)`
        * 内存释放函数
* Hook函数
    * `void schedPortIdleHook(void)`
        * 空闲任务Hook函数
