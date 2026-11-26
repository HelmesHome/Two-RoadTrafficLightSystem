/*
 * fsm_night.h
 *
 * Mode 6: Night (Blinking Amber) / Warning (All-Red) state machine.
 */

#ifndef INC_FSM_NIGHT_H_
#define INC_FSM_NIGHT_H_

#include "main.h"

void fsm_night_init(void);
void fsm_night_run(void);

#endif /* INC_FSM_NIGHT_H_ */
