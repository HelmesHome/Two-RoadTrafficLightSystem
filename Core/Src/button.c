/*
 * button_reading.c
 *
 *  Created on: Nov 12, 2026
 *      Author: Admin
 */
#include <button.h>
#include"main.h"

#define NO_OF_BUTTONS 4

//timer interrupt duration is 10ms, so to pass 1 second,
//we need to jump to the interrupt service routine 100 time
#define DURATION_FOR_AUTO_INCREASING 100
#define BUTTON_IS_PRESSED GPIO_PIN_RESET
#define BUTTON_IS_RELEASED GPIO_PIN_SET
#define REPEATED 50
//the buffer that the final result is stored after debouncing
static GPIO_PinState buttonBuffer[NO_OF_BUTTONS];

//we define three buffers for debouncing
static GPIO_PinState debounceButtonBuffer1[NO_OF_BUTTONS];
static GPIO_PinState debounceButtonBuffer2[NO_OF_BUTTONS];
static GPIO_PinState debounceButtonBuffer3[NO_OF_BUTTONS];

static uint8_t button_flag[NO_OF_BUTTONS] = {0, 0, 0, 0};

//we define a flag for a button pressed more than 1 second.
static uint8_t flagForButtonPress1s[NO_OF_BUTTONS];

//we define counter for automatically increasing the value
//after the button is pressed more than 1 second.
static uint16_t counterForButtonPress1s[NO_OF_BUTTONS];

GPIO_PinState buttonPin(int index){
	switch (index){
		case 0: return HAL_GPIO_ReadPin(BTN_1_GPIO_Port, BTN_1_Pin);
		case 1: return HAL_GPIO_ReadPin(BTN_2_GPIO_Port, BTN_2_Pin);
		case 2: return HAL_GPIO_ReadPin(BTN_3_GPIO_Port, BTN_3_Pin);
		case 3: return HAL_GPIO_ReadPin(BTN_4_GPIO_Port, BTN_4_Pin);
		default: return BUTTON_IS_RELEASED;
	}
}

void button_reading(void) {
    for(int i = 0; i < NO_OF_BUTTONS; i++) {
    	debounceButtonBuffer3[i] = debounceButtonBuffer2[i];
    	debounceButtonBuffer2[i] = debounceButtonBuffer1[i];
    	debounceButtonBuffer1[i] = buttonPin(i);

        if ((debounceButtonBuffer1[i] == debounceButtonBuffer2[i]) &&
                (debounceButtonBuffer2[i] == debounceButtonBuffer3[i])) {

        	if (buttonBuffer[i] != debounceButtonBuffer1[i]) {
            	buttonBuffer[i] = debounceButtonBuffer1[i];
            	if(buttonBuffer[i] == BUTTON_IS_PRESSED){
            		button_flag[i] = 1;
            		counterForButtonPress1s[i] = 0;
            	}else{
            		counterForButtonPress1s[i] = 0;
            		flagForButtonPress1s[i] = 0;
            	}

            }else if (buttonBuffer[i] == BUTTON_IS_PRESSED) {
                //if a button is pressed, we start counting
                if (counterForButtonPress1s[i] < DURATION_FOR_AUTO_INCREASING) {
                    counterForButtonPress1s[i]++;
                } else {
                    //the flag is turned on when 1 second has passed
                    //since the button is pressed.
                    flagForButtonPress1s[i] = 1;
                    counterForButtonPress1s[i] = DURATION_FOR_AUTO_INCREASING - REPEATED;// Increasing continually;
                }
            }

        }
    }
}

unsigned char is_button_pressed(uint8_t index) {
    if (index >= NO_OF_BUTTONS) return 0xff;
    if(button_flag[index] == 1){
    	button_flag[index] = 0;
    	return 1;
 	}
	return 0;
}

unsigned char is_button_pressed_hold(unsigned char index) {
    if (index >= NO_OF_BUTTONS) return 0xff;
    if(flagForButtonPress1s[index] == 1){
    	flagForButtonPress1s[index] = 0;
    	return 1;
    }
    return 0;
}
