/*
 * fsm_manual.h
 *
 *  Created on: Nov 13, 2026
 *      Author: Admin
 */

#ifndef INC_FSM_MANUAL_H_
#define INC_FSM_MANUAL_H_

void fsm_manual_init(int current_auto_state);
int get_manual_state(void);
void fsm_manual_run(void);

#endif /* INC_FSM_MANUAL_H_ */
