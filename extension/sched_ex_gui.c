/*******************************************************************************
* 文 件 名: sched_ex_gui.c
* 创 建 者: Keda Huang
* 版    本: V1.0
* 创建日期: 2017-04-07
* 文件说明: 事件驱动调度器的GUI扩展
*******************************************************************************/

#include "sched_ex_gui.h"

/*******************************************************************************

                                    操作函数

*******************************************************************************/
/**
 * 创建GUI控制器
 *
 * @param prio: GUI任务优先级
 *
 * @param eventLen: 事件缓存区大小
 *
 * @param initial: 初始化伪状态
 *
 * @param pCreatedGUI: 待创建的GUI控制器指针
 *
 * @return: SCHED_OK    控制器创建成功
 */
eSchedError schedExGUICreate(SchedPrio_t             prio,
                             EvtPos_t                eventLen,
                             SchedEventHandler       initial,
                             sSchedGUI * const       pCreatedGUI)
{
    schedTaskCreate(prio, eventLen, initial, &pCreatedGUI->task);

#if SCHED_EX_GUI_SCAN_TIMER_EN
    schedTimerCreate(&pCreatedGUI->task, SCHED_SIG_GUI_SCAN, 0,
                SCHED_TIMER_RELOAD, 0, &pCreatedGUI->scanTimer);

#endif

#if SCHED_EX_GUI_UPDATE_TIMER_EN
    schedTimerCreate(&pCreatedGUI->task, SCHED_SIG_GUI_UPDATE, 0,
                SCHED_TIMER_RELOAD, 0, &pCreatedGUI->updateTimer);
#endif

    return (SCHED_OK);
}

/**
 * GUI任务初始化状态的实现函数
 *
 * @param pGUI: GUI控制器指针
 *
 * @param pEntryView: 入口视图控制器指针
 *
 * @param me: 状态函数的参数 - 当前任务结构体指针
 *
 * @param e:  状态函数的参数 - 当前处理事件
 *
 * @return: 状态函数返回值
 */
SchedBase_t schedExGUIStateInitial(sSchedGUI *pGUI, sSchedView *pEntryView,
                                   void *me, SchedEvent_t const * const e)
{
    SCHED_ASSERT(THIS_TASK() == &pGUI->task);
    return schedStateTransfer(THIS_TASK(), pEntryView->phy->ViewStateHandler);
}

/**
 * 指定视图的GUI任务状态的实现函数
 *
 * @param pGUI: GUI控制器指针
 *
 * @param pView: 指定视图控制器指针
 *
 * @param me: 状态函数的参数 - 当前任务结构体指针
 *
 * @param e:  状态函数的参数 - 当前处理事件
 *
 * @return: 状态函数返回值
 */
SchedBase_t schedExGUIStateHandle(sSchedGUI *pGUI, sSchedView *pView,
                                  void *me, SchedEvent_t const * const e)
{
SchedBase_t ret;

    SCHED_ASSERT(THIS_TASK() == &pGUI->task);

    switch (e->sig)
    {
    /* 状态初始化信号 */
    case SCHED_SIG_ENTRY:
    {
    #if SCHED_EX_GUI_SCAN_TIMER_EN
        schedTimerChangePeriod(&pGUI->scanTimer, pView->phy->scanPeriod);
        schedTimerStart(&pGUI->scanTimer);
    #endif

    #if SCHED_EX_GUI_UPDATE_TIMER_EN
        schedTimerChangePeriod(&pGUI->updateTimer, pView->phy->updatePeriod);
        schedTimerStart(&pGUI->updateTimer);
    #endif

        schedEventSend(THIS_TASK(), SCHED_SIG_GUI_DISPLAY_LOAD, 0);

        if (pView->phy->Init != 0)
        {
            pView->phy->Init();
        }
        return SCHED_HANDLED();
    }

    /* 状态退出信号 */
    case SCHED_SIG_EXIT:
    {
    #if SCHED_EX_GUI_SCAN_TIMER_EN
        schedTimerReset(&pGUI->scanTimer);
    #endif

    #if SCHED_EX_GUI_UPDATE_TIMER_EN
        schedTimerReset(&pGUI->updateTimer);
    #endif

        if (pView->phy->Exit != 0)
        {
            pView->phy->Exit();
        }
        return SCHED_HANDLED();
    }

    /* 动作扫描信号 */
    case SCHED_SIG_GUI_SCAN:
    {
    eSchedBool action_detect = SCHED_FALSE;
    eSchedBool view_tran = SCHED_FALSE;
    SchedEvent_t action;
    sSchedView *target_view = NULL;

        ret = SCHED_RET_HANDLED;
        if (pView->phy->Scan != 0)
        {
            action.sig = SCHED_GUI_ACTION_NONE;
            action.msg = 0;
            action_detect = pView->phy->Scan(&action);
        }

        if ( action_detect && (pView->phy->Action != 0) )
        {
            view_tran = pView->phy->Action(&action, &target_view);
            if (view_tran)
            {
                SCHED_ASSERT(target_view != NULL);
                ret = schedStateTransfer(THIS_TASK(), target_view->phy->ViewStateHandler);
            }
        }

        return (ret);
    }

    /* 定时更新信号 */
    case SCHED_SIG_GUI_UPDATE:
    {
    eSchedBool display_update = SCHED_TRUE;

        if (pView->phy->Update != 0)
        {
            display_update = pView->phy->Update();
        }

        if (display_update)
        {
            schedEventSend(THIS_TASK(), SCHED_SIG_GUI_DISPLAY_UPDATE, 0);
        }
        return SCHED_HANDLED();
    }

    /* 显示加载信号 */
    case SCHED_SIG_GUI_DISPLAY_LOAD:
    {
        if (pView->phy->Display != 0)
        {
            pView->phy->Display(SCHED_TRUE);
        }
        return SCHED_HANDLED();
    }

    /* 显示更新信号 */
    case SCHED_SIG_GUI_DISPLAY_UPDATE:
    {
        if (pView->phy->Display != 0)
        {
            pView->phy->Display(SCHED_FALSE);
        }
        return SCHED_HANDLED();
    }

    /* 用户自定义信号 */
    default:
    {
        ret = SCHED_RET_IGNORED;
        if (pView->phy->UserEventHandler != 0)
        {
            ret = pView->phy->UserEventHandler(me, e);
        }
        return (ret);
    }

    }
}

/* 动作扫描外部定时器 */
void schedExGUIExternalScanTimer(sSchedGUI *pGUI)
{
#if SCHED_EX_GUI_SCAN_TIMER_EN == 0
    schedEventSend(&pGUI->task, SCHED_SIG_GUI_SCAN, 0);
#endif
}

/* 定时更新外部定时器 */
void schedExGUIExternalUpdateTimer(sSchedGUI *pGUI)
{
#if SCHED_EX_GUI_UPDATE_TIMER_EN == 0
    schedEventSend(&pGUI->task, SCHED_SIG_GUI_UPDATE, 0);
#endif
}

/* 加载当前视图显示 */
void schedExGUIDisplayLoad(sSchedGUI *pGUI)
{
    schedEventSend(&pGUI->task, SCHED_SIG_GUI_DISPLAY_LOAD, 0);
}

/* 更新当前视图显示 */
void schedExGUIDisplayUpdate(sSchedGUI *pGUI)
{
    schedEventSend(&pGUI->task, SCHED_SIG_GUI_DISPLAY_UPDATE, 0);
}
