/*******************************************************************************
* 文 件 名: sched_ex_gui.c
* 创 建 者: Keda Huang
* 版    本: V1.0
* 创建日期: 2017-04-07
* 文件说明: 事件驱动调度器的GUI扩展
*******************************************************************************/

#include "sched_ex_gui.h"
#include "sched_api.h"
/*******************************************************************************

                                    操作函数

*******************************************************************************/
/**
 * 初始化GUI控制器
 *
 * @param pGui: 待初始化的GUI结构体指针
 *
 * @param hGuiTask: GUI任务句柄
 *
 * @return: 若GUI控制器初始化成功, 返回SCHED_OK,
 *          若初始化过程出现错误, 返回错误代码
 *
 */
eSchedError schedExGuiInit(sSchedGui *pGui, SchedHandle_t hGuiTask)
{
eSchedError ret_err = SCHED_OK;

    pGui->hGuiTask     = hGuiTask;
    pGui->hScanTimer   = NULL;
    pGui->hUpdateTimer = NULL;

    if (hGuiTask == NULL)
    {
        ret_err = SCHED_ERR_PARAM;
    }

    if (ret_err == SCHED_OK)
    {
        ret_err = schedTimerCreate(hGuiTask, SCHED_SIG_GUI_SCAN, 0,
                    SCHED_TIMER_RELOAD, 0, &pGui->hScanTimer);
    }

    if (ret_err == SCHED_OK)
    {
        ret_err = schedTimerCreate(hGuiTask, SCHED_SIG_GUI_UPDATE, 0,
                    SCHED_TIMER_RELOAD, 0, &pGui->hUpdateTimer);
    }

    return (ret_err);
}

/**
 * 初始化视图控制器
 *
 * @param pView: 待初始化的视图结构体指针
 *
 * @param pViewPhy: 视图实现接口结构体指针
 *
 * @param scanPeriod: 动作扫描周期
 *
 * @param updatePeriod: 视图更新周期
 *
 * @return: 若视图控制器初始化成功, 返回SCHED_OK
 */
eSchedError schedExViewInit(sSchedView              *pView,
                            sSchedViewPhy const     *pViewPhy,
                            SchedTick_t             scanPeriod,
                            SchedTick_t             updatePeriod)
{
    pView->phy          = pViewPhy;
    pView->scanPeriod   = scanPeriod;
    pView->updatePeriod = updatePeriod;
    return (SCHED_OK);
}

/**
 * GUI任务初始化状态的实现函数
 *
 * @param pGui: GUI控制器指针
 *
 * @param pEntryView: 入口视图控制器指针
 *
 * @param me: 状态函数的参数 - 当前任务句柄
 *
 * @param e:  状态函数的参数 - 当前处理事件
 *
 * @return: 状态函数返回值
 */
SchedBase_t schedExGuiStateInitial(sSchedGui *pGui, sSchedView *pEntryView,
                                   SchedHandle_t me, SchedEvent_t const * const e)
{
    SCHED_ASSERT(me == pGui->hGuiTask);
    return schedStateTransfer(me, pEntryView->phy->ViewStateHandler);
}

/**
 * 指定视图的GUI任务状态的实现函数
 *
 * @param pGui: GUI控制器指针
 *
 * @param pView: 指定视图控制器指针
 *
 * @param me: 状态函数的参数 - 当前任务句柄
 *
 * @param e:  状态函数的参数 - 当前处理事件
 *
 * @return: 状态函数返回值
 */
SchedBase_t schedExGuiStateHandle(sSchedGui *pGui, sSchedView *pView,
                                  SchedHandle_t me, SchedEvent_t const * const e)
{
SchedBase_t ret;
eSchedError err;

    SCHED_ASSERT(me == pGui->hGuiTask);

    switch (e->sig)
    {
    /* 状态初始化信号 */
    case SCHED_SIG_ENTRY:
    {
        err = schedTimerChangePeriod(pGui->hScanTimer, pView->scanPeriod);
        SCHED_ASSERT(err == SCHED_OK);
        err = schedTimerChangePeriod(pGui->hUpdateTimer, pView->updatePeriod);
        SCHED_ASSERT(err == SCHED_OK);
        err = schedTimerStart(pGui->hScanTimer);
        SCHED_ASSERT(err == SCHED_OK);
        err = schedTimerStart(pGui->hUpdateTimer);
        SCHED_ASSERT(err == SCHED_OK);
        err = schedEventSend(pGui->hGuiTask, SCHED_SIG_GUI_DISPLAY_LOAD, 0);
        SCHED_ASSERT(err == SCHED_OK);
        ((void)err);

        if (pView->phy->Init != 0)
        {
            pView->phy->Init();
        }
        return SCHED_HANDLED();
    }

    /* 状态退出信号 */
    case SCHED_SIG_EXIT:
    {
        err = schedTimerReset(pGui->hScanTimer);
        SCHED_ASSERT(err == SCHED_OK);
        err = schedTimerReset(pGui->hUpdateTimer);
        SCHED_ASSERT(err == SCHED_OK);
        ((void)err);

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
        }

        if (view_tran)
        {
            ret = schedStateTransfer(me, target_view->phy->ViewStateHandler);
        }
        return (ret);
    }

    /* 视图更新信号 */
    case SCHED_SIG_GUI_UPDATE:
    {
        if (pView->phy->Update != 0)
        {
            pView->phy->Update();
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

/* 加载当前视图显示 */
void schedExGuiDisplayLoad(sSchedGui *pGui)
{
    schedEventSend(pGui->hGuiTask, SCHED_SIG_GUI_DISPLAY_LOAD, 0);
}

/* 更新当前视图显示 */
void schedExGuiDisplayUpdate(sSchedGui *pGui)
{
    schedEventSend(pGui->hGuiTask, SCHED_SIG_GUI_DISPLAY_UPDATE, 0);
}
