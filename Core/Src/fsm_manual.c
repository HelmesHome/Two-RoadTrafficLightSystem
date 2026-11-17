/*
 * fsm_manual.c
 *
 *  Created on: Nov 13, 2026
 *      Author: Admin
 */

#include <button.h>
#include <global.h>
#include <lcd.h>
#include "fsm_manual.h"
#include "traffic_light.h"

typedef enum { MAN_RED_GREEN, MAN_RED_AMBER, MAN_GREEN_RED, MAN_AMBER_RED } Manual_State;
static Manual_State man_state = MAN_RED_GREEN;

void fsm_manual_init(int current_auto_state) {
    if (current_auto_state == 0) {
        man_state = MAN_RED_GREEN;
    } else {
        man_state = (Manual_State)current_auto_state;
    }
}


void fsm_manual_run(void) {
    // Logic đổi state bằng Nút 2 và xuất màu đèn tương ứng
    // ... (Giữ nguyên logic fsm_manual_run như hướng dẫn trước) ...
	if (is_button_pressed(1) == 1) {
		switch (man_state) {
			case MAN_RED_GREEN: man_state = MAN_RED_AMBER; break;
			case MAN_RED_AMBER: man_state = MAN_GREEN_RED; break;
			case MAN_GREEN_RED: man_state = MAN_AMBER_RED; break;
			case MAN_AMBER_RED: man_state = MAN_RED_GREEN; break;
		}
	}

	    // Xuất tín hiệu ra phần cứng
	switch (man_state) {
		case MAN_RED_GREEN: TL1_Set_Red(); TL2_Set_Green(); break;
		case MAN_RED_AMBER: TL1_Set_Red(); TL2_Set_Yellow(); break;
	 	case MAN_GREEN_RED: TL1_Set_Green(); TL2_Set_Red(); break;
	 	case MAN_AMBER_RED: TL1_Set_Yellow(); TL2_Set_Red(); break;
	}
    LCD_Update_Traffic_Info(system_mode, 0, 0);
}
