/*
 * fsm_global.c
 *
 *  Created on: Nov 14, 2026
 *      Author: Admin
 */

#include "global.h"
#include "button.h"
#include "fsm_automatic.h"
#include "fsm_manual.h"
#include "fsm_night.h"
#include "fsm_tuning.h"
#include "lcd.h"
#include "software_timer.h"
#include "traffic_light.h"
#include "uart_log.h"

uint8_t system_mode = 1;
int red_duration = 5;
int amber_duration = 2;
int green_duration = 3;

void fsm_global_run(void) {
    /* In Tuning modes (3..5), fsm_tuning_run() owns Button 1 so it can
       step Red -> Yellow -> Green, validate at the end, or escape Error-Wait.
       Outside Tuning (modes 1, 2, 6), global handles Button 1 transitions. */
    if ((system_mode < 3 || system_mode == 6) && is_button_pressed(0) == 1) {
        system_mode++;
        if (system_mode > 6) {
            system_mode = 1;
        }

        button_clear_all();
        timer_stop(TIMER_TUNING_BLINK);
        timer_stop(TIMER_ERROR_WAIT);
        timer_stop(TIMER_SAVED_MSG);
        timer_stop(TIMER_NIGHT_BLINK);

        if (system_mode == 1) {
            fsm_automatic_init(0);
            uart_log("[MODE] -> AUTO (R=%02d G=%02d A=%02d)\r\n",
                     red_duration, green_duration, amber_duration);
        } else if (system_mode == 2) {
            fsm_manual_init(get_auto_state());
            uart_log("[MODE] -> MANUAL (state=%d)\r\n", get_manual_state());
        } else {
            fsm_tuning_enter_session();
            uart_log("[MODE] -> TUNE RED (R=%02d A=%02d G=%02d)\r\n",
                     red_duration, amber_duration, green_duration);
        }
    }

    switch (system_mode) {
        case 1:
            fsm_automatic_run();
            break;
        case 2:
            fsm_manual_run();
            break;
        case 3:
        case 4:
        case 5:
            fsm_tuning_run();
            break;
        case 6:
            fsm_night_run();
            break;
        default:
            system_mode = 1;
            fsm_automatic_init(0);
            break;
    }
}
