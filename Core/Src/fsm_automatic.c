/*
 * fsm_automatic.c
 *
 *  Created on: Nov 13, 2026
 *      Author: Admin
 */


#include <global.h>
#include <lcd.h>
#include "fsm_automatic.h"
#include "traffic_light.h"

typedef enum { AUTO_INIT, AUTO_RED_GREEN, AUTO_RED_AMBER, AUTO_GREEN_RED, AUTO_AMBER_RED } Auto_State;
static Auto_State auto_state = AUTO_INIT;
static int count_TL1 = 0, count_TL2 = 0, auto_tick = 0;

int get_auto_state(void) {
    return (int)auto_state;
}

void fsm_automatic_init(void) {
    if (system_mode == 1) auto_state = AUTO_INIT;
}

void fsm_automatic_run(void) {
	auto_tick++;
	if (auto_tick >= 100) {
		auto_tick = 0;
		count_TL1--;
		count_TL2--;
	}
	switch(auto_state){
		case AUTO_INIT:
			count_TL1 = red_duration;
			count_TL2 = green_duration;
			auto_state = AUTO_RED_GREEN;
			break;
		case AUTO_RED_GREEN:
			TL1_Set_Red(); TL2_Set_Green();
			if(count_TL2 <= 0 && auto_tick == 0){
				count_TL2 = amber_duration;
				auto_state = AUTO_RED_AMBER;
			}
			break;
		case AUTO_RED_AMBER:
			TL1_Set_Red();
			TL2_Set_Yellow(); // Tương ứng Amber
			if (count_TL1 <= 0 && auto_tick == 0) {
				count_TL1 = green_duration;
				count_TL2 = red_duration;
				auto_state = AUTO_GREEN_RED;
		 		}
		 	break;

		case AUTO_GREEN_RED:
			TL1_Set_Green();
			TL2_Set_Red();
			if (count_TL1 <= 0 && auto_tick == 0) {
				count_TL1 = amber_duration;
				auto_state = AUTO_AMBER_RED;
		 	}
			break;

		 case AUTO_AMBER_RED:
		   	TL1_Set_Yellow();
		 	TL2_Set_Red();
		 	if (count_TL2 <= 0 && auto_tick == 0) {
		 		count_TL1 = red_duration;
		 		count_TL2 = green_duration;
		 		auto_state = AUTO_RED_GREEN;
		 	}
		 	break;
	}
    LCD_Update_Traffic_Info(system_mode, count_TL1, count_TL2);
}
