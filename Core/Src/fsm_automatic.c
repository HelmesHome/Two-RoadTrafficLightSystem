/*
 * fsm_automatic.c
 *
 *  Created on: Nov 13, 2026
 *      Author: Admin
 */

#include "fsm_automatic.h"
#include "global.h"
#include "lcd.h"
#include "software_timer.h"
#include "traffic_light.h"

typedef enum {
    AUTO_INIT = 0,
    AUTO_RED_GREEN = 1,
    AUTO_RED_AMBER = 2,
    AUTO_GREEN_RED = 3,
    AUTO_AMBER_RED = 4
} Auto_State;

static Auto_State auto_state = AUTO_INIT;
static int count_TL1 = 0;
static int count_TL2 = 0;

static void auto_apply_outputs(void) {
    switch (auto_state) {
        case AUTO_RED_GREEN: traffic_set_pair(TL_PAIR_RED_GREEN); break;
        case AUTO_RED_AMBER: traffic_set_pair(TL_PAIR_RED_AMBER); break;
        case AUTO_GREEN_RED: traffic_set_pair(TL_PAIR_GREEN_RED); break;
        case AUTO_AMBER_RED: traffic_set_pair(TL_PAIR_AMBER_RED); break;
        case AUTO_INIT:
        default:             traffic_set_pair(TL_PAIR_ALL_RED); break;
    }
    LCD_Update_Traffic_Info(1, count_TL1, count_TL2);
}

static void auto_ensure_valid_durations(void) {
    if (amber_duration < 1 || green_duration < 1 || red_duration > 99 ||
        red_duration != (green_duration + amber_duration)) {
        red_duration = 5;
        amber_duration = 2;
        green_duration = 3;
    }
}

int get_auto_state(void) {
    return (int)auto_state;
}

void fsm_automatic_init(int current_man_state) {
    auto_ensure_valid_durations();

    if (current_man_state >= 1 && current_man_state <= 4) {
        auto_state = (Auto_State)current_man_state;
    } else {
        auto_state = AUTO_RED_GREEN;
    }

    switch (auto_state) {
        case AUTO_RED_GREEN:
            count_TL1 = red_duration;
            count_TL2 = green_duration;
            break;
        case AUTO_RED_AMBER:
            count_TL1 = amber_duration;
            count_TL2 = amber_duration;
            break;
        case AUTO_GREEN_RED:
            count_TL1 = green_duration;
            count_TL2 = red_duration;
            break;
        case AUTO_AMBER_RED:
            count_TL1 = amber_duration;
            count_TL2 = amber_duration;
            break;
        case AUTO_INIT:
        default:
            auto_state = AUTO_RED_GREEN;
            count_TL1 = red_duration;
            count_TL2 = green_duration;
            break;
    }

    timer_set_ms(TIMER_AUTO, 1000);
    auto_apply_outputs();
}

void fsm_automatic_run(void) {
    if (auto_state == AUTO_INIT) {
        fsm_automatic_init(0);
        return;
    }

    if (!timer_is_expired(TIMER_AUTO)) {
        return;
    }

    timer_set_ms(TIMER_AUTO, 1000);
    if (count_TL1 > 0) count_TL1--;
    if (count_TL2 > 0) count_TL2--;

    switch (auto_state) {
        case AUTO_RED_GREEN:
            if (count_TL2 <= 0) {
                auto_state = AUTO_RED_AMBER;
                count_TL2 = amber_duration;
                if (count_TL1 <= 0) {
                    count_TL1 = amber_duration;
                }
            }
            break;

        case AUTO_RED_AMBER:
            if (count_TL2 <= 0 || count_TL1 <= 0) {
                auto_state = AUTO_GREEN_RED;
                count_TL1 = green_duration;
                count_TL2 = red_duration;
            }
            break;

        case AUTO_GREEN_RED:
            if (count_TL1 <= 0) {
                auto_state = AUTO_AMBER_RED;
                count_TL1 = amber_duration;
                if (count_TL2 <= 0) {
                    count_TL2 = amber_duration;
                }
            }
            break;

        case AUTO_AMBER_RED:
            if (count_TL1 <= 0 || count_TL2 <= 0) {
                auto_state = AUTO_RED_GREEN;
                count_TL1 = red_duration;
                count_TL2 = green_duration;
            }
            break;

        case AUTO_INIT:
        default:
            fsm_automatic_init(0);
            return;
    }

    auto_apply_outputs();
}
