/*
 * fsm_night.c
 *
 * Mode 6: Night (Blinking Amber) / Warning (All-Red) state machine.
 */

#include "fsm_night.h"
#include "button.h"
#include "lcd.h"
#include "software_timer.h"
#include "traffic_light.h"
#include "uart_log.h"

#define NIGHT_BLINK_MS 500

typedef enum {
    NIGHT_SUB_BLINK_AMBER = 0,
    NIGHT_SUB_WARN_RED = 1
} Night_SubMode;

static Night_SubMode night_submode = NIGHT_SUB_BLINK_AMBER;
static uint8_t blink_on = 1;

void fsm_night_init(void) {
    night_submode = NIGHT_SUB_BLINK_AMBER;
    blink_on = 1;
    timer_set_ms(TIMER_NIGHT_BLINK, NIGHT_BLINK_MS);
    traffic_set_pair(TL_PAIR_ALL_AMBER);
    LCD_Update_Traffic_Info(6, (int)night_submode, 0);
    uart_log("[MODE] -> NIGHT (BLINK AMBER)\r\n");
}

void fsm_night_run(void) {
    /* Consume unused buttons */
    (void)is_button_pressed(2);
    (void)is_button_pressed(3);
    (void)is_button_pressed_hold(1);
    (void)is_button_pressed_hold(2);
    (void)is_button_pressed_hold(3);

    /* BTN_2 toggles between Night Blinking Amber and Warning All-Red */
    if (is_button_pressed(1) == 1) {
        night_submode = (night_submode == NIGHT_SUB_BLINK_AMBER)
                            ? NIGHT_SUB_WARN_RED
                            : NIGHT_SUB_BLINK_AMBER;
        blink_on = 1;
        timer_set_ms(TIMER_NIGHT_BLINK, NIGHT_BLINK_MS);

        if (night_submode == NIGHT_SUB_BLINK_AMBER) {
            traffic_set_pair(TL_PAIR_ALL_AMBER);
            uart_log("[NIGHT] -> BLINK AMBER\r\n");
        } else {
            traffic_set_pair(TL_PAIR_ALL_RED);
            uart_log("[NIGHT] -> WARN ALL-RED\r\n");
        }
        LCD_Update_Traffic_Info(6, (int)night_submode, 0);
        return;
    }

    if (night_submode == NIGHT_SUB_BLINK_AMBER) {
        if (timer_is_expired(TIMER_NIGHT_BLINK)) {
            blink_on = (uint8_t)(1U - blink_on);
            timer_set_ms(TIMER_NIGHT_BLINK, NIGHT_BLINK_MS);
            traffic_set_pair(blink_on ? TL_PAIR_ALL_AMBER : TL_PAIR_OFF);
        }
    } else {
        traffic_set_pair(TL_PAIR_ALL_RED);
    }
}
