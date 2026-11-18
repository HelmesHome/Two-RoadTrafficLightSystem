/*
 * scheduler.c
 *
 *  Created on: Nov 12, 2026
 *      Author: Admin
 */
#include"scheduler.h"

uint8_t SCH_count = 0;

void SCH_Init(){
	SCH_count = 0;
}
uint32_t SCH_Add_Task(void (*pFunc)(), uint32_t Delay, uint32_t Period){
	if (SCH_count >= SCH_MAX_TASKS) return 0;

	uint32_t newDelay = Delay;
	uint8_t insertIndex = 0;
	while(insertIndex < SCH_count && newDelay >=  SCH_tasks_G[insertIndex].Delay){
		newDelay -= SCH_tasks_G[insertIndex].Delay;
		insertIndex++;
	}
	for (int i = SCH_count; i > insertIndex; i--) {
		SCH_tasks_G[i] = SCH_tasks_G[i - 1];
	}
	SCH_tasks_G[insertIndex].pTask = pFunc;
	SCH_tasks_G[insertIndex].Delay = newDelay;
	SCH_tasks_G[insertIndex].Period = Period;
	SCH_tasks_G[insertIndex].RunMe = 0;
	if (insertIndex + 1 <= SCH_count) {
		SCH_tasks_G[insertIndex + 1].Delay -= newDelay;
	}
	SCH_count++;
	return SCH_count;
}
uint32_t SCH_Delete_Task(uint32_t taskID) {
	if (taskID >= SCH_count) return 0;
	if (taskID + 1 < SCH_count) {
		SCH_tasks_G[taskID + 1].Delay += SCH_tasks_G[taskID].Delay;
	}
	for (int i = taskID; i < SCH_count - 1; i++) {
		SCH_tasks_G[i] = SCH_tasks_G[i + 1];
	}
	SCH_tasks_G[SCH_count - 1].pTask = 0;
	SCH_tasks_G[SCH_count - 1].Delay = 0;
	SCH_tasks_G[SCH_count - 1].Period = 0;
	SCH_tasks_G[SCH_count - 1].RunMe = 0;
	SCH_count--;
	return SCH_count;
}
void SCH_Dispatch_Tasks(void) {
	if (SCH_tasks_G[0].RunMe > 0) {
		void (*pFunc)(void) = SCH_tasks_G[0].pTask;
		uint32_t run_period = SCH_tasks_G[0].Period;
		SCH_Delete_Task(0);
		(*pFunc)();
		if (run_period > 0) {
			SCH_Add_Task(pFunc, run_period, run_period);
		}
	}
}
void SCH_Update() {
	if (SCH_count == 0) return;
	if (SCH_tasks_G[0].Delay > 0) SCH_tasks_G[0].Delay--;
	if (SCH_tasks_G[0].Delay == 0) SCH_tasks_G[0].RunMe = 1;
}
