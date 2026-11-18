/*
 * fsm_tuning.c
 *
 *  Created on: Nov 14, 2026
 *      Author: Admin
 */

#include <button.h>
#include <global.h>
#include <lcd.h>
#include "fsm_tuning.h"
#include "traffic_light.h"

static int tune_duration = 0;
static int blink_timer = 0;
static uint8_t led_toggle = 0;

void fsm_tuning_init(void) {
    if (system_mode == 3) tune_duration = red_duration;
    if (system_mode == 4) tune_duration = amber_duration;
    if (system_mode == 5) tune_duration = green_duration;
}

void fsm_tuning_run(void) {
	blink_timer++;
	    if (blink_timer >= 25) {
	        blink_timer = 0;
	        led_toggle = 1 - led_toggle;

	        if (system_mode == 3) {
	            if (led_toggle) { TL1_Set_Red(); TL2_Set_Red(); } else { TL1_Set_Off(); TL2_Set_Off(); }
	        } else if (system_mode == 4) {
	            if (led_toggle) { TL1_Set_Yellow(); TL2_Set_Yellow(); } else { TL1_Set_Off(); TL2_Set_Off(); }
	        } else if (system_mode == 5) {
	            if (led_toggle) { TL1_Set_Green(); TL2_Set_Green(); } else { TL1_Set_Off(); TL2_Set_Off(); }
	        }
	    }
	    if (is_button_pressed(1) == 1 || is_button_pressed_hold(1) == 1) {
	        tune_duration++;
	        if (tune_duration > 99) tune_duration = 1;
	    }
	    if (is_button_pressed(2) == 1 || is_button_pressed_hold(2) == 1) {
	    	        tune_duration--;
	    	        if (tune_duration < 1) tune_duration = 99;
	    	    }

	    if (is_button_pressed(3) == 1 ||is_button_pressed_hold(3) == 1) {
	    	if (system_mode == 3) {
	    		if (tune_duration > amber_duration && tune_duration <= 99) {
	    			red_duration = tune_duration;
	    			green_duration = red_duration - amber_duration;
	    		} else {
	    			tune_duration = red_duration;
	    		}
	    	} else if (system_mode == 4) {
	    		if (tune_duration > 0 && (tune_duration + green_duration) <= 99) {
	    			amber_duration = tune_duration;
	    			red_duration = green_duration + amber_duration;
	    		} else {
	    			tune_duration = amber_duration;
	    		}
	    	} else if (system_mode == 5) {
	    		if (tune_duration > 0 && (tune_duration + amber_duration) <= 99) {
	    			green_duration = tune_duration;
	    			red_duration = green_duration + amber_duration;
	    		} else {
	    			tune_duration = green_duration;
	    		}
	    	}
	    }
	    if (blink_timer == 0) {
	        LCD_Update_Traffic_Info(system_mode, tune_duration, 0);
	    }
}
