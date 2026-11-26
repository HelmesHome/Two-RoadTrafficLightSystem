/*
 * fsm_manual.c
 *
 *  Created on: Nov 13, 2026
 *      Author: Admin
 */

#include "fsm_manual.h"
#include "button.h"
#include "global.h"
#include "lcd.h"
#include "traffic_light.h"
#include "uart_log.h"

typedef enum {
    MAN_INIT = 0,
    MAN_RED_GREEN = 1,
    MAN_RED_AMBER = 2,
    MAN_GREEN_RED = 3,
    MAN_AMBER_RED = 4
} Manual_State;

static Manual_State man_state = MAN_RED_GREEN;

static void manual_apply_outputs(void) {
    switch (man_state) {
        case MAN_RED_GREEN: traffic_set_pair(TL_PAIR_RED_GREEN); break;
        case MAN_RED_AMBER: traffic_set_pair(TL_PAIR_RED_AMBER); break;
        case MAN_GREEN_RED: traffic_set_pair(TL_PAIR_GREEN_RED); break;
        case MAN_AMBER_RED: traffic_set_pair(TL_PAIR_AMBER_RED); break;
        case MAN_INIT:
        default:            traffic_set_pair(TL_PAIR_ALL_RED); break;
    }
    LCD_Update_Traffic_Info(2, (int)man_state, 0);
}

void fsm_manual_init(int current_auto_state) {
    if (current_auto_state >= 1 && current_auto_state <= 4) {
        man_state = (Manual_State)current_auto_state;
    } else {
        man_state = MAN_RED_GREEN;
    }
    manual_apply_outputs();
}

int get_manual_state(void) {
    return (int)man_state;
}

void fsm_manual_run(void) {
    if (is_button_pressed(1) == 1) {
        switch (man_state) {
            case MAN_RED_GREEN: man_state = MAN_RED_AMBER; break;
            case MAN_RED_AMBER: man_state = MAN_GREEN_RED; break;
            case MAN_GREEN_RED: man_state = MAN_AMBER_RED; break;
            case MAN_AMBER_RED: man_state = MAN_RED_GREEN; break;
            case MAN_INIT:
            default:            man_state = MAN_RED_GREEN; break;
        }
        uart_log("[MAN] -> phase %d\r\n", (int)man_state);
    }

    manual_apply_outputs();
}
