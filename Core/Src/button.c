/*
 * button.c
 *
 *  Created on: Nov 12, 2026
 *      Author: Admin
 */

#include "button.h"
#include "main.h"

#define NO_OF_BUTTONS 4

/* 10 ms period: 100 ticks = 1 s initial hold threshold, 50 ticks = 500 ms repeat */
#define DURATION_FOR_AUTO_INCREASING 100
#define BUTTON_IS_PRESSED GPIO_PIN_RESET
#define BUTTON_IS_RELEASED GPIO_PIN_SET
#define REPEATED 50

static GPIO_PinState buttonBuffer[NO_OF_BUTTONS];
static GPIO_PinState debounceButtonBuffer1[NO_OF_BUTTONS];
static GPIO_PinState debounceButtonBuffer2[NO_OF_BUTTONS];
static GPIO_PinState debounceButtonBuffer3[NO_OF_BUTTONS];

static uint8_t button_flag[NO_OF_BUTTONS];
static uint8_t flagForButtonPress1s[NO_OF_BUTTONS];
static uint16_t counterForButtonPress1s[NO_OF_BUTTONS];

static GPIO_PinState buttonPin(uint8_t index) {
    switch (index) {
        case 0: return HAL_GPIO_ReadPin(BTN_1_GPIO_Port, BTN_1_Pin);
        case 1: return HAL_GPIO_ReadPin(BTN_2_GPIO_Port, BTN_2_Pin);
        case 2: return HAL_GPIO_ReadPin(BTN_3_GPIO_Port, BTN_3_Pin);
        case 3: return HAL_GPIO_ReadPin(BTN_4_GPIO_Port, BTN_4_Pin);
        default: return BUTTON_IS_RELEASED;
    }
}

void button_init(void) {
    for (uint8_t i = 0; i < NO_OF_BUTTONS; i++) {
        buttonBuffer[i] = BUTTON_IS_RELEASED;
        debounceButtonBuffer1[i] = BUTTON_IS_RELEASED;
        debounceButtonBuffer2[i] = BUTTON_IS_RELEASED;
        debounceButtonBuffer3[i] = BUTTON_IS_RELEASED;
        button_flag[i] = 0;
        flagForButtonPress1s[i] = 0;
        counterForButtonPress1s[i] = 0;
    }
}

void button_clear_all(void) {
    for (uint8_t i = 0; i < NO_OF_BUTTONS; i++) {
        button_flag[i] = 0;
        flagForButtonPress1s[i] = 0;
    }
}

void button_reading(void) {
    for (uint8_t i = 0; i < NO_OF_BUTTONS; i++) {
        debounceButtonBuffer3[i] = debounceButtonBuffer2[i];
        debounceButtonBuffer2[i] = debounceButtonBuffer1[i];
        debounceButtonBuffer1[i] = buttonPin(i);

        if ((debounceButtonBuffer1[i] == debounceButtonBuffer2[i]) &&
            (debounceButtonBuffer2[i] == debounceButtonBuffer3[i])) {
            if (buttonBuffer[i] != debounceButtonBuffer1[i]) {
                buttonBuffer[i] = debounceButtonBuffer1[i];
                if (buttonBuffer[i] == BUTTON_IS_PRESSED) {
                    button_flag[i] = 1;
                    counterForButtonPress1s[i] = 0;
                } else {
                    counterForButtonPress1s[i] = 0;
                    flagForButtonPress1s[i] = 0;
                }
            } else if (buttonBuffer[i] == BUTTON_IS_PRESSED) {
                if (counterForButtonPress1s[i] < DURATION_FOR_AUTO_INCREASING) {
                    counterForButtonPress1s[i]++;
                } else {
                    flagForButtonPress1s[i] = 1;
                    counterForButtonPress1s[i] = DURATION_FOR_AUTO_INCREASING - REPEATED;
                }
            }
        }
    }
}

uint8_t is_button_pressed(uint8_t index) {
    if (index >= NO_OF_BUTTONS) {
        return 0;
    }
    if (button_flag[index] == 1) {
        button_flag[index] = 0;
        return 1;
    }
    return 0;
}

uint8_t is_button_pressed_hold(uint8_t index) {
    if (index >= NO_OF_BUTTONS) {
        return 0;
    }
    if (flagForButtonPress1s[index] == 1) {
        flagForButtonPress1s[index] = 0;
        return 1;
    }
    return 0;
}
