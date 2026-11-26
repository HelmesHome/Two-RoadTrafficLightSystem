/*
 * button.h
 *
 *  Created on: Nov 12, 2026
 *      Author: Admin
 */

#ifndef INC_BUTTON_H_
#define INC_BUTTON_H_

#include "main.h"

void button_init(void);
void button_clear_all(void);
void button_reading(void);
uint8_t is_button_pressed(uint8_t index);
uint8_t is_button_pressed_hold(uint8_t index);

#endif /* INC_BUTTON_H_ */
