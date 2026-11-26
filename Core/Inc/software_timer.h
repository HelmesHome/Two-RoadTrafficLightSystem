/*
 * software_timer.h
 *
 * Monotonic millisecond software timers updated by the 10 ms scheduler task.
 */

#ifndef INC_SOFTWARE_TIMER_H_
#define INC_SOFTWARE_TIMER_H_

#include "main.h"

#define TIMER_AUTO          0
#define TIMER_TUNING_BLINK  1
#define TIMER_ERROR_WAIT    2
#define TIMER_SAVED_MSG     3
#define TIMER_NIGHT_BLINK   4
#define MAX_SW_TIMERS       5

void timer_init(void);
void timer_set_ms(uint8_t index, uint32_t duration_ms);
void timer_stop(uint8_t index);
uint8_t timer_is_expired(uint8_t index);
uint32_t timer_get_remaining_ms(uint8_t index);
void timer_run(void);

#endif /* INC_SOFTWARE_TIMER_H_ */
