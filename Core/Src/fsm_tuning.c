/*
 * fsm_tuning.c
 *
 *  Created on: Nov 14, 2026
 *      Author: Admin
 */

#include "fsm_tuning.h"
#include "button.h"
#include "fsm_night.h"
#include "global.h"
#include "lcd.h"
#include "software_timer.h"
#include "traffic_light.h"
#include "uart_log.h"
#include <stdio.h>

#define TUNE_MIN_DURATION   1
#define TUNE_MAX_DURATION   99
#define TUNE_BLINK_MS       250
#define TUNE_ERROR_WAIT_MS  10000
#define TUNE_SAVED_MSG_MS   1000

typedef enum {
    TUNE_STATE_EDIT = 0,
    TUNE_STATE_ERROR_WAIT = 1,
    TUNE_STATE_SAVED = 2
} Tuning_SubState;

static Tuning_SubState tune_substate = TUNE_STATE_EDIT;

/* Confirmed session drafts (updated ONLY when BTN_4 Save is pressed) */
static int draft_red = 5;
static int draft_amber = 2;
static int draft_green = 3;

/* Live unconfirmed value currently being adjusted with BTN_2 / BTN_3 */
static int edit_val = 5;
static uint8_t led_toggle = 1;

static uint8_t err_return_mode = 5;
static uint8_t err_can_auto_fix = 0;
static char err_reason_buf[17] = "ERR: R != G + A";
static int pending_red = 0;
static int pending_amber = 0;
static int pending_green = 0;

static void tuning_apply_lights(uint8_t mode, uint8_t on) {
    if (mode == 3) {
        traffic_set_pair(on ? TL_PAIR_ALL_RED : TL_PAIR_OFF);
    } else if (mode == 4) {
        traffic_set_pair(on ? TL_PAIR_ALL_AMBER : TL_PAIR_OFF);
    } else if (mode == 5) {
        traffic_set_pair(on ? TL_PAIR_TUNE_GREEN_ON : TL_PAIR_TUNE_GREEN_OFF);
    } else {
        traffic_set_pair(TL_PAIR_OFF);
    }
}

static int *tuning_get_draft_ptr(uint8_t mode) {
    if (mode == 3) return &draft_red;
    if (mode == 4) return &draft_amber;
    if (mode == 5) return &draft_green;
    return &draft_red;
}

static void tuning_show_current_edit(void) {
    int r = (system_mode == 3) ? edit_val : draft_red;
    int a = (system_mode == 4) ? edit_val : draft_amber;
    int g = (system_mode == 5) ? edit_val : draft_green;
    LCD_Show_Tuning_Drafts(system_mode, r, a, g);
}

static void tuning_reset_drafts_from_active(void) {
    draft_red = red_duration;
    draft_amber = amber_duration;
    draft_green = green_duration;
}

static void tuning_commit_drafts(int new_r, int new_g, int new_a) {
    red_duration = new_r;
    green_duration = new_g;
    amber_duration = new_a;
    draft_red = new_r;
    draft_green = new_g;
    draft_amber = new_a;
}

static void tuning_exit_to_night(void) {
    button_clear_all();
    timer_stop(TIMER_TUNING_BLINK);
    timer_stop(TIMER_ERROR_WAIT);
    timer_stop(TIMER_SAVED_MSG);
    system_mode = 6;
    fsm_night_init();
    LCD_Update_Environment();
}

static void tuning_enter_error_wait(uint8_t can_fix, int new_r, int new_g, int new_a) {
    err_return_mode = system_mode;
    err_can_auto_fix = can_fix;
    pending_red = new_r;
    pending_green = new_g;
    pending_amber = new_a;

    tune_substate = TUNE_STATE_ERROR_WAIT;
    traffic_set_pair(TL_PAIR_ALL_RED);
    timer_set_ms(TIMER_ERROR_WAIT, TUNE_ERROR_WAIT_MS);
    LCD_Show_Tuning_Error(err_reason_buf, 10);
    uart_log("[TUNE] %s -> wait 10s\r\n", err_reason_buf);
}

static void tuning_enter_saved_all(void) {
    tune_substate = TUNE_STATE_SAVED;
    timer_set_ms(TIMER_SAVED_MSG, TUNE_SAVED_MSG_MS);
    LCD_Show_Tuning_Saved((uint8_t)red_duration, (uint8_t)green_duration, (uint8_t)amber_duration);
}

static void tuning_enter_step_confirmed(uint8_t mode, int val) {
    tune_substate = TUNE_STATE_SAVED;
    timer_set_ms(TIMER_SAVED_MSG, TUNE_SAVED_MSG_MS);
    LCD_Show_Tuning_Step_Confirmed(mode, val);
}

static void tuning_validate_final(void) {
    if (draft_green >= TUNE_MIN_DURATION &&
        draft_amber >= TUNE_MIN_DURATION &&
        draft_red <= TUNE_MAX_DURATION &&
        draft_red == (draft_green + draft_amber)) {
        tuning_commit_drafts(draft_red, draft_green, draft_amber);
        uart_log("[TUNE] Saved R=%02d G=%02d A=%02d\r\n",
                 red_duration, green_duration, amber_duration);
        tuning_enter_saved_all();
        return;
    }

    /* Compare red against green + amber:
       - If red < green + amber: auto-fix updates red = green + amber (if <= 99)
       - Else (red > green + amber): auto-fix updates green = red - amber (if red > amber) */
    if (draft_red < (draft_green + draft_amber)) {
        int new_red = draft_green + draft_amber;
        if (draft_green >= TUNE_MIN_DURATION &&
            draft_amber >= TUNE_MIN_DURATION &&
            new_red <= TUNE_MAX_DURATION) {
            snprintf(err_reason_buf, sizeof(err_reason_buf),
                     "ERR R%02d!=G%02d+A%02d", draft_red, draft_green, draft_amber);
            tuning_enter_error_wait(1, new_red, draft_green, draft_amber);
        } else {
            snprintf(err_reason_buf, sizeof(err_reason_buf),
                     "ERR G%02d+A%02d>99", draft_green, draft_amber);
            tuning_enter_error_wait(0, red_duration, green_duration, amber_duration);
        }
    } else {
        if (draft_red > draft_amber && draft_red <= TUNE_MAX_DURATION) {
            snprintf(err_reason_buf, sizeof(err_reason_buf),
                     "ERR R%02d!=G%02d+A%02d", draft_red, draft_green, draft_amber);
            tuning_enter_error_wait(1, draft_red, draft_red - draft_amber, draft_amber);
        } else {
            snprintf(err_reason_buf, sizeof(err_reason_buf),
                     "ERR R%02d <= A%02d", draft_red, draft_amber);
            tuning_enter_error_wait(0, red_duration, green_duration, amber_duration);
        }
    }
}

void fsm_tuning_enter_session(void) {
    tuning_reset_drafts_from_active();
    fsm_tuning_init();
}

void fsm_tuning_init(void) {
    tune_substate = TUNE_STATE_EDIT;
    timer_stop(TIMER_ERROR_WAIT);
    timer_stop(TIMER_SAVED_MSG);

    edit_val = *tuning_get_draft_ptr(system_mode);
    led_toggle = 1;
    timer_set_ms(TIMER_TUNING_BLINK, TUNE_BLINK_MS);
    tuning_apply_lights(system_mode, led_toggle);
    tuning_show_current_edit();
}

uint8_t fsm_tuning_is_error_wait(void) {
    return (tune_substate == TUNE_STATE_ERROR_WAIT) ? 1 : 0;
}

void fsm_tuning_run(void) {
    if (tune_substate == TUNE_STATE_ERROR_WAIT) {
        traffic_set_pair(TL_PAIR_ALL_RED);

        /* Consume unrelated button events so they cannot fire later */
        (void)is_button_pressed(1);
        (void)is_button_pressed(2);
        (void)is_button_pressed(3);
        (void)is_button_pressed_hold(0);
        (void)is_button_pressed_hold(1);
        (void)is_button_pressed_hold(2);
        (void)is_button_pressed_hold(3);

        /* Button 1 (index 0) takes priority: return to TUNE RED (Mode 3) keeping all 3 drafts */
        if (is_button_pressed(0) == 1) {
            uart_log("[TUNE] BTN1 -> TUNE RED (kept R=%02d A=%02d G=%02d)\r\n",
                     draft_red, draft_amber, draft_green);
            system_mode = 3;
            fsm_tuning_init();
            LCD_Update_Environment();
            return;
        }

        if (timer_is_expired(TIMER_ERROR_WAIT)) {
            if (err_can_auto_fix) {
                tuning_commit_drafts(pending_red, pending_green, pending_amber);
                uart_log("[TUNE] Auto-fix -> R=%02d G=%02d A=%02d\r\n",
                         red_duration, green_duration, amber_duration);
                system_mode = err_return_mode;
                edit_val = *tuning_get_draft_ptr(system_mode);
                led_toggle = 1;
                timer_set_ms(TIMER_TUNING_BLINK, TUNE_BLINK_MS);
                tuning_apply_lights(system_mode, led_toggle);
                tuning_enter_saved_all();
                LCD_Update_Environment();
            } else {
                /* Impossible case: retain previous valid active config and reset drafts */
                tuning_reset_drafts_from_active();
                uart_log("[TUNE] Impossible -> revert R=%02d G=%02d A=%02d\r\n",
                         red_duration, green_duration, amber_duration);
                system_mode = 3;
                fsm_tuning_init();
                LCD_Update_Environment();
            }
            return;
        }

        uint32_t rem_ms = timer_get_remaining_ms(TIMER_ERROR_WAIT);
        uint8_t rem_sec = (uint8_t)((rem_ms + 999U) / 1000U);
        if (rem_sec < 1) rem_sec = 1;
        if (rem_sec > 10) rem_sec = 10;
        LCD_Show_Tuning_Error(err_reason_buf, rem_sec);
        return;
    }

    /* Blink lights during TUNE_STATE_EDIT and TUNE_STATE_SAVED */
    if (timer_is_expired(TIMER_TUNING_BLINK)) {
        led_toggle = (uint8_t)(1U - led_toggle);
        timer_set_ms(TIMER_TUNING_BLINK, TUNE_BLINK_MS);
        tuning_apply_lights(system_mode, led_toggle);
    }

    uint8_t mode_btn = is_button_pressed(0);
    uint8_t inc = (uint8_t)(is_button_pressed(1) || is_button_pressed_hold(1));
    uint8_t dec = (uint8_t)(is_button_pressed(2) || is_button_pressed_hold(2));
    uint8_t save = is_button_pressed(3);
    (void)is_button_pressed_hold(0);
    (void)is_button_pressed_hold(3); /* Save and Mode are single-press only */

    if (tune_substate == TUNE_STATE_SAVED) {
        if (mode_btn) {
            if (system_mode < 5) {
                system_mode++;
                uart_log("[MODE] -> %s (R=%02d A=%02d G=%02d)\r\n",
                         (system_mode == 4) ? "TUNE YELLOW" : "TUNE GREEN",
                         draft_red, draft_amber, draft_green);
                fsm_tuning_init();
            } else {
                tuning_exit_to_night();
            }
            return;
        }
        if (inc || dec || save) {
            timer_stop(TIMER_SAVED_MSG);
            tune_substate = TUNE_STATE_EDIT;
        } else if (timer_is_expired(TIMER_SAVED_MSG)) {
            tune_substate = TUNE_STATE_EDIT;
            tuning_show_current_edit();
            return;
        } else {
            return;
        }
    }

    /* BTN_1 (Mode): advance WITHOUT saving unconfirmed edit_val */
    if (mode_btn) {
        if (system_mode < 5) {
            system_mode++;
            uart_log("[MODE] -> %s (R=%02d A=%02d G=%02d)\r\n",
                     (system_mode == 4) ? "TUNE YELLOW" : "TUNE GREEN",
                     draft_red, draft_amber, draft_green);
            fsm_tuning_init();
        } else {
            tuning_exit_to_night();
        }
        return;
    }

    /* TUNE_STATE_EDIT */
    if (inc && !dec) {
        if (edit_val < TUNE_MAX_DURATION) {
            edit_val++;
        }
    } else if (dec && !inc) {
        if (edit_val > TUNE_MIN_DURATION) {
            edit_val--;
        }
    }

    /* BTN_4 (Save): ONLY Save confirms edit_val into session draft */
    if (save) {
        if (system_mode == 3) {
            draft_red = edit_val;
            uart_log("[TUNE] Confirm RED=%02d\r\n", draft_red);
            tuning_enter_step_confirmed(3, draft_red);
            return;
        } else if (system_mode == 4) {
            draft_amber = edit_val;
            if (draft_red < draft_amber) {
                snprintf(err_reason_buf, sizeof(err_reason_buf),
                         "ERR R%02d < A%02d", draft_red, draft_amber);
                tuning_enter_error_wait(0, red_duration, green_duration, amber_duration);
                return;
            }
            uart_log("[TUNE] Confirm AMB=%02d\r\n", draft_amber);
            tuning_enter_step_confirmed(4, draft_amber);
            return;
        } else if (system_mode == 5) {
            draft_green = edit_val;
            tuning_validate_final();
            return;
        }
    }

    tuning_show_current_edit();
}
