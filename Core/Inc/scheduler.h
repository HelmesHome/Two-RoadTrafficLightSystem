/*
 * scheduler.h
 *
 *  Created on: Nov 12, 2026
 *      Author: Admin
 */

#ifndef INC_SCHEDULER_H_
#define INC_SCHEDULER_H_

#include "main.h"

typedef struct {
    void (*pTask)(void);
    uint32_t Delay;
    uint32_t Period;
    uint8_t RunMe;
    uint32_t TaskID;
} sTask;

#define SCH_MAX_TASKS 10
#define NO_TASK_ID 0

extern sTask SCH_tasks_G[SCH_MAX_TASKS];

void SCH_Init(void);
uint32_t SCH_Add_Task(void (*pFunc)(void), uint32_t Delay, uint32_t Period);
uint32_t SCH_Delete_Task(uint32_t taskID);
void SCH_Update(void);          // called in the timer interrupt (O(1))
void SCH_Dispatch_Tasks(void);  // called in while(1)
uint32_t SCH_Get_Overflow_Count(void);

#endif /* INC_SCHEDULER_H_ */
