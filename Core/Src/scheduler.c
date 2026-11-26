/*
 * scheduler.c
 *
 *  Created on: Nov 12, 2026
 *      Author: Admin
 */

#include "scheduler.h"

sTask SCH_tasks_G[SCH_MAX_TASKS];
static uint8_t SCH_count = 0;
static uint32_t next_task_id = 1;
static uint32_t sch_overflow_count = 0;

static uint32_t sch_enter_critical(void) {
    uint32_t primask = __get_PRIMASK();
    __disable_irq();
    return primask;
}

static void sch_exit_critical(uint32_t primask) {
    __set_PRIMASK(primask);
}

static void sch_remove_at_locked(uint8_t index) {
    if (index >= SCH_count) {
        return;
    }

    if ((uint8_t)(index + 1) < SCH_count) {
        SCH_tasks_G[index + 1].Delay += SCH_tasks_G[index].Delay;
    }

    for (uint8_t i = index; (uint8_t)(i + 1) < SCH_count; i++) {
        SCH_tasks_G[i] = SCH_tasks_G[i + 1];
    }

    SCH_count--;
    SCH_tasks_G[SCH_count].pTask = NULL;
    SCH_tasks_G[SCH_count].Delay = 0;
    SCH_tasks_G[SCH_count].Period = 0;
    SCH_tasks_G[SCH_count].RunMe = 0;
    SCH_tasks_G[SCH_count].TaskID = NO_TASK_ID;
}

static uint32_t sch_add_task_with_id(void (*pFunc)(void), uint32_t Delay, uint32_t Period, uint32_t existingID) {
    if (pFunc == NULL) {
        return NO_TASK_ID;
    }

    uint32_t primask = sch_enter_critical();
    if (SCH_count >= SCH_MAX_TASKS) {
        sch_overflow_count++;
        sch_exit_critical(primask);
        return NO_TASK_ID;
    }

    uint32_t assigned_id = existingID;
    if (assigned_id == NO_TASK_ID) {
        assigned_id = next_task_id++;
        if (next_task_id == NO_TASK_ID) {
            next_task_id = 1;
        }
    }

    uint32_t newDelay = Delay;
    uint8_t insertIndex = 0;
    while (insertIndex < SCH_count && newDelay >= SCH_tasks_G[insertIndex].Delay) {
        newDelay -= SCH_tasks_G[insertIndex].Delay;
        insertIndex++;
    }

    for (uint8_t i = SCH_count; i > insertIndex; i--) {
        SCH_tasks_G[i] = SCH_tasks_G[i - 1];
    }

    SCH_tasks_G[insertIndex].pTask = pFunc;
    SCH_tasks_G[insertIndex].Delay = newDelay;
    SCH_tasks_G[insertIndex].Period = Period;
    SCH_tasks_G[insertIndex].RunMe = 0;
    SCH_tasks_G[insertIndex].TaskID = assigned_id;

    if ((uint8_t)(insertIndex + 1) <= SCH_count) {
        SCH_tasks_G[insertIndex + 1].Delay -= newDelay;
    }

    SCH_count++;
    sch_exit_critical(primask);
    return assigned_id;
}

void SCH_Init(void) {
    uint32_t primask = sch_enter_critical();
    SCH_count = 0;
    next_task_id = 1;
    sch_overflow_count = 0;
    for (uint8_t i = 0; i < SCH_MAX_TASKS; i++) {
        SCH_tasks_G[i].pTask = NULL;
        SCH_tasks_G[i].Delay = 0;
        SCH_tasks_G[i].Period = 0;
        SCH_tasks_G[i].RunMe = 0;
        SCH_tasks_G[i].TaskID = NO_TASK_ID;
    }
    sch_exit_critical(primask);
}

uint32_t SCH_Add_Task(void (*pFunc)(void), uint32_t Delay, uint32_t Period) {
    return sch_add_task_with_id(pFunc, Delay, Period, NO_TASK_ID);
}

uint32_t SCH_Delete_Task(uint32_t taskID) {
    if (taskID == NO_TASK_ID) {
        return 0;
    }

    uint32_t primask = sch_enter_critical();
    for (uint8_t i = 0; i < SCH_count; i++) {
        if (SCH_tasks_G[i].TaskID == taskID) {
            sch_remove_at_locked(i);
            sch_exit_critical(primask);
            return 1;
        }
    }
    sch_exit_critical(primask);
    return 0;
}

void SCH_Dispatch_Tasks(void) {
    while (1) {
        void (*pFunc)(void) = NULL;
        uint32_t run_period = 0;
        uint32_t task_id = NO_TASK_ID;

        uint32_t primask = sch_enter_critical();
        if (SCH_count > 0 && SCH_tasks_G[0].RunMe > 0) {
            pFunc = SCH_tasks_G[0].pTask;
            run_period = SCH_tasks_G[0].Period;
            task_id = SCH_tasks_G[0].TaskID;
            sch_remove_at_locked(0);
            if (SCH_count > 0 && SCH_tasks_G[0].Delay == 0) {
                SCH_tasks_G[0].RunMe = 1;
            }
        }
        sch_exit_critical(primask);

        if (pFunc == NULL) {
            break;
        }

        (*pFunc)();

        if (run_period > 0) {
            sch_add_task_with_id(pFunc, run_period, run_period, task_id);
        }
    }
}

void SCH_Update(void) {
    if (SCH_count == 0) {
        return;
    }
    if (SCH_tasks_G[0].Delay > 0) {
        SCH_tasks_G[0].Delay--;
    }
    if (SCH_tasks_G[0].Delay == 0) {
        SCH_tasks_G[0].RunMe = 1;
    }
}

uint32_t SCH_Get_Overflow_Count(void) {
    return sch_overflow_count;
}
