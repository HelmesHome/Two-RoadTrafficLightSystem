/*
 * software_timer.c
 *
 * Monotonic millisecond software timers using HAL_GetTick() so I2C delays
 * do not stretch 1-second or 5-second countdowns.
 */

#include "software_timer.h"

typedef struct {
    uint32_t remaining_ms;
    uint8_t active;
    uint8_t flag;
} SwTimer_t;

static SwTimer_t sw_timers[MAX_SW_TIMERS];
static uint32_t last_tick_ms = 0;

void timer_init(void) {
    last_tick_ms = HAL_GetTick();
    for (uint8_t i = 0; i < MAX_SW_TIMERS; i++) {
        sw_timers[i].remaining_ms = 0;
        sw_timers[i].active = 0;
        sw_timers[i].flag = 0;
    }
}

void timer_set_ms(uint8_t index, uint32_t duration_ms) {
    if (index >= MAX_SW_TIMERS) {
        return;
    }
    if (duration_ms == 0) {
        sw_timers[index].remaining_ms = 0;
        sw_timers[index].active = 0;
        sw_timers[index].flag = 1;
        return;
    }
    sw_timers[index].remaining_ms = duration_ms;
    sw_timers[index].active = 1;
    sw_timers[index].flag = 0;
}

void timer_stop(uint8_t index) {
    if (index >= MAX_SW_TIMERS) {
        return;
    }
    sw_timers[index].remaining_ms = 0;
    sw_timers[index].active = 0;
    sw_timers[index].flag = 0;
}

uint8_t timer_is_expired(uint8_t index) {
    if (index >= MAX_SW_TIMERS) {
        return 0;
    }
    if (sw_timers[index].flag) {
        sw_timers[index].flag = 0;
        return 1;
    }
    return 0;
}

uint32_t timer_get_remaining_ms(uint8_t index) {
    if (index >= MAX_SW_TIMERS) {
        return 0;
    }
    if (!sw_timers[index].active) {
        return 0;
    }
    return sw_timers[index].remaining_ms;
}

void timer_run(void) {
    uint32_t now_ms = HAL_GetTick();
    uint32_t elapsed_ms = now_ms - last_tick_ms;
    last_tick_ms = now_ms;

    if (elapsed_ms == 0) {
        return;
    }

    for (uint8_t i = 0; i < MAX_SW_TIMERS; i++) {
        if (sw_timers[i].active) {
            if (sw_timers[i].remaining_ms <= elapsed_ms) {
                sw_timers[i].remaining_ms = 0;
                sw_timers[i].active = 0;
                sw_timers[i].flag = 1;
            } else {
                sw_timers[i].remaining_ms -= elapsed_ms;
            }
        }
    }
}
