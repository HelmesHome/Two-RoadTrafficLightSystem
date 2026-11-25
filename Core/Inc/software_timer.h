/*
 * software_timer.h
 *
 *  Created on: Nov 25, 2026
 *      Author: Admin
 */

#ifndef INC_SOFTWARE_TIMER_H_
#define INC_SOFTWARE_TIMER_H_

#define TIMER_CYCLE   10
#define MAX_TIMERS    3

#define TIMER_AUTO    0
#define TIMER_BLINK   1
#define TIMER_ERROR   2

void setTimer(int index, int duration_ms);
int isTimerExpired(int index);
void timerRun(void);

#endif /* INC_SOFTWARE_TIMER_H_ */
