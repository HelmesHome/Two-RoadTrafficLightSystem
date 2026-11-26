/*
 * fsm_tuning.h
 *
 *  Created on: Nov 14, 2026
 *      Author: Admin
 */

#ifndef INC_FSM_TUNING_H_
#define INC_FSM_TUNING_H_

#include "main.h"

void fsm_tuning_enter_session(void);
void fsm_tuning_init(void);
void fsm_tuning_run(void);
uint8_t fsm_tuning_is_error_wait(void);

#endif /* INC_FSM_TUNING_H_ */
