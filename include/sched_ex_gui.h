/*******************************************************************************
* 文 件 名: sched_ex_gui.h
* 创 建 者: Keda Huang
* 版    本: V1.0
* 创建日期: 2017-04-07
* 文件说明: 事件驱动调度器的GUI扩展
*******************************************************************************/

#ifndef __SCHED_EX_GUI_H
#define __SCHED_EX_GUI_H

/* 头文件 --------------------------------------------------------------------*/
#include "sched_proto.h"

/* 结构体类型 ----------------------------------------------------------------*/
typedef struct sched_gui        sSchedGUI;
typedef struct sched_view       sSchedView;
typedef struct sched_view_phy   sSchedViewPhy;

/* GUI结构体 */
struct sched_gui
{
    SchedHandle_t               hGUITask;           /* GUI任务句柄 */
    SchedHandle_t               hScanTimer;         /* 扫描定时器  */
    SchedHandle_t               hUpdateTimer;       /* 更新定时器  */
};

/* 视图结构体 */
struct sched_view
{
    sSchedViewPhy const         *phy;               /* 视图实现接口 */
};

/* 视图实现接口 */
struct sched_view_phy
{
    /* 动作扫描周期 */
    SchedTick_t                 scanPeriod;
    /* 定时更新周期 */
    SchedTick_t                 updatePeriod;
    /* 状态处理函数 */
    SchedEventHandler           ViewStateHandler;
    /* 用户自定义事件处理函数 */
    SchedEventHandler           UserEventHandler;
    /* 视图初始化 */
    void (*Init)(void);
    /* 视图退出 */
    void (*Exit)(void);
    /* 定时更新函数 */
    void (*Update)(void);
    /*
     * 视图显示函数
     *
     * @param load: SCHED_TRUE表示加载整个视图, SCHED_FALSE表示更新视图
     */
    void (*Display)(eSchedBool load);
    /*
     * 动作扫描函数
     *
     * @param pAction: 保存新动作的事件变量指针
     *
     * @return: 若产生新动作则返回SCHED_TRUE, pAction指向的地址保存新动作,
     *          若没有产生新动作, 返回SCHED_FALSE
     */
    eSchedBool (*Scan)(SchedEvent_t * const pAction);
    /*
     * 动作响应函数
     *
     * @param pAction: 待响应的动作
     *
     * @param p2TargetView: 若进行视图切换, 则保存目标视图结构体指针
     *
     * @return: 若进行视图切换则返回SCHED_TRUE, p2TargetView保存目标视图结构体指针,
     *          若不进行视图切换则返回SCHED_FALSE
     */
    eSchedBool (*Action)(SchedEvent_t const * const pAction, sSchedView** const p2TargetView);
};

/* 常量定义 ------------------------------------------------------------------*/
/* GUI任务信号 */
enum {
    SCHED_SIG_GUI_SCAN = SCHED_SIG_USER,    /* GUI动作扫描信号 */
    SCHED_SIG_GUI_UPDATE,                   /* GUI定时更新信号 */
    SCHED_SIG_GUI_DISPLAY_LOAD,             /* GUI显示加载信号 */
    SCHED_SIG_GUI_DISPLAY_UPDATE,           /* GUI显示更新信号 */
    SCHED_SIG_GUI_USER                      /* GUI用户定义信号 */
};

/* GUI动作信号 */
enum {
    SCHED_GUI_ACTION_NONE = 0,              /* GUI空动作信号   */
    SCHED_GUI_ACTION_USER                   /* GUI用户定义动作 */
};

/* GUI宏定义 -----------------------------------------------------------------*/
#define __SCHED_VIEW_DEFINE(_view_, _phy_)  \
    sSchedView _view_ =                     \
    {                                       \
        .phy          = (_phy_),            \
    }

#define __SCHED_VIEW_PHY_DEFINE(_phy_, _scanPeriod_, _updatePeriod_,    \
        _state_, _user_, _init_, _exit_, _update_, _display_, _scan_, _action_) \
    sSchedViewPhy _phy_   =                     \
    {                                           \
        .scanPeriod       = (_scanPeriod_),     \
        .updatePeriod     = (_updatePeriod_),   \
        .ViewStateHandler = (_state_),          \
        .UserEventHandler = (_user_),           \
        .Init             = (_init_),           \
        .Exit             = (_exit_),           \
        .Update           = (_update_),         \
        .Display          = (_display_),        \
        .Scan             = (_scan_),           \
        .Action           = (_action_)          \
    }

#define __SCHED_VIEW_PHY(_view_)            _view_##__phy

/* 视图结构体定义 */
#define SCHED_VIEW_DEFINE(_view_, _scanPeriod_, _updatePeriod_, \
        _state_, _user_, _init_, _exit_, _update_, _display_, _scan_, _action_) \
                                                                                                    \
    static const __SCHED_VIEW_PHY_DEFINE(__SCHED_VIEW_PHY(_view_), _scanPeriod_, _updatePeriod_,    \
        _state_, _user_, _init_, _exit_, _update_, _display_, _scan_, _action_);                    \
    __SCHED_VIEW_DEFINE(_view_, &__SCHED_VIEW_PHY(_view_))


/* 视图切换 */
#define SCHED_TRAN_VIEW(_pTargetView_)  \
    ( SCHED_TRAN((_pTargetView_)->ViewStateHandler) )

/* GUI操作函数 ---------------------------------------------------------------*/
eSchedError schedExGUIInit(sSchedGUI *pGUI, SchedHandle_t hGUITask);
SchedBase_t schedExGUIStateInitial(sSchedGUI *pGUI, sSchedView *pEntryView,
                                   SchedHandle_t me, SchedEvent_t const * const e);
SchedBase_t schedExGUIStateHandle(sSchedGUI *pGUI, sSchedView *pView,
                                  SchedHandle_t me, SchedEvent_t const * const e);

void schedExGUIDisplayLoad(sSchedGUI *pGUI);
void schedExGUIDisplayUpdate(sSchedGUI *pGUI);

#endif  /* __SCHED_EX_GUI_H */
