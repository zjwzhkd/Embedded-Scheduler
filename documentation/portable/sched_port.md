# 调度器底层接口定义
* **体系相关**的调度器原型定义
* 底层接口函数定义

## 原型定义
* `SchedBase_t`
    * 处理器架构基本类型, **带符号整型**
* `SchedCPU_t`
    * CPU状态数据类型
* `SchedEvent_t`
    * 调度器事件结构体类型

## 宏定义
* 临界区管理

    ```c
    #define SCHED_EnterCritical()
    #define SCHED_ExitCritical(x)
    #define SCHED_EnterCriticalFromISR()
    #define SCHED_ExitCriticalFromISR(x)
    ```

* 调度器断言

    ```c
    #define SCHED_ASSERT(expr)
    ```

## 底层接口
* 底层函数

    ```c
    void schedPortInit(void);
    ```

    * 调度器底层初始化

* 事件块操作函数

    ```c
    /**
     * 事件块复制
     *
     * @param dst: 目标事件块指针
     *
     * @param src: 源事件块指针
     *
     * @return: 目标事件块指针
     */
    SchedEvent_t *schedPortEvtcpy(SchedEvent_t *dst,
                                  SchedEvent_t const *src);

    /**
     * 事件块填充
     *
     * @param dst: 目标事件块指针
     *
     * @param sig: 事件信号
     *
     * @param msg: 事件消息
     *
     * @return: 目标事件块指针
     */
    SchedEvent_t *schedPortEvtfill(SchedEvent_t *dst,
                                   EvtSig_t sig, EvtMsg_t msg)

    ```

* 内存分配函数

    ```c
    /* 动态内存分配函数 */
    void *schedPortMalloc(size_t nsize);

    /* 动态内存释放函数 */
    void schedPortFree(void *pv);
    ```
