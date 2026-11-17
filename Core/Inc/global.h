/*
 * fsm_global.h
 *
 *  Created on: Nov 14, 2026
 *      Author: Admin
 */

#ifndef INC_GLOBAL_H_
#define INC_GLOBAL_H_

#include "main.h"

// Khai báo các biến dùng chung cho toàn hệ thống
extern uint8_t system_mode;
extern int red_duration;
extern int amber_duration;
extern int green_duration;

void fsm_global_run(void);

#endif /* INC_GLOBAL_H_ */
