/*
 * fsm_global.c
 *
 *  Created on: Nov 14, 2026
 *      Author: Admin
 */

#include <button.h>
#include <global.h>
#include <lcd.h>
#include "fsm_automatic.h"
#include "fsm_manual.h"
#include "fsm_tuning.h"
#include "traffic_light.h"

uint8_t system_mode = 1;
int red_duration = 5;
int amber_duration = 2;
int green_duration = 3;

void fsm_global_run(void) {
    if (is_button_pressed(0) == 1) {
        system_mode++;
        if (system_mode > 5) system_mode = 1;

        TL1_Set_Off(); TL2_Set_Off(); lcd_clear();

        if (system_mode == 1) {
        	fsm_automatic_init(get_manual_state());
        }
        else if (system_mode == 2) {
        	int current_state = get_auto_state();
        	fsm_manual_init(current_state);
        }
        else {
        	fsm_tuning_init();
        }
    }

    switch (system_mode) {
        case 1: fsm_automatic_run(); break;
        case 2: fsm_manual_run(); break;
        case 3:
        case 4:
        case 5: fsm_tuning_run(); break;
    }
}
