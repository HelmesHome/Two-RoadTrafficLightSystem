/*
 * traffic_light.c
 *
 *  Created on: Nov 13, 2026
 *      Author: Admin
 */

#include "traffic_light.h"
#include "main.h"

void TL1_Set_Off(void) {
    HAL_GPIO_WritePin(TL1_A_GPIO_Port, TL1_A_Pin, 0);
    HAL_GPIO_WritePin(TL1_B_GPIO_Port, TL1_B_Pin, 0);
}

void TL1_Set_Yellow(void) {
    HAL_GPIO_WritePin(TL1_A_GPIO_Port, TL1_A_Pin, 0);
    HAL_GPIO_WritePin(TL1_B_GPIO_Port, TL1_B_Pin, 1);
}

void TL1_Set_Green(void) {
    HAL_GPIO_WritePin(TL1_A_GPIO_Port, TL1_A_Pin, 1);
    HAL_GPIO_WritePin(TL1_B_GPIO_Port, TL1_B_Pin, 0);
}

void TL1_Set_Red(void) {
    HAL_GPIO_WritePin(TL1_A_GPIO_Port, TL1_A_Pin, 1);
    HAL_GPIO_WritePin(TL1_B_GPIO_Port, TL1_B_Pin, 1);
}

void TL2_Set_Off(void) {
    HAL_GPIO_WritePin(TL2_A_GPIO_Port, TL2_A_Pin, 0);
    HAL_GPIO_WritePin(TL2_B_GPIO_Port, TL2_B_Pin, 0);
}

void TL2_Set_Yellow(void) {
    HAL_GPIO_WritePin(TL2_A_GPIO_Port, TL2_A_Pin, 0);
    HAL_GPIO_WritePin(TL2_B_GPIO_Port, TL2_B_Pin, 1);
}

void TL2_Set_Green(void) {
    HAL_GPIO_WritePin(TL2_A_GPIO_Port, TL2_A_Pin, 1);
    HAL_GPIO_WritePin(TL2_B_GPIO_Port, TL2_B_Pin, 0);
}

void TL2_Set_Red(void) {
    HAL_GPIO_WritePin(TL2_A_GPIO_Port, TL2_A_Pin, 1);
    HAL_GPIO_WritePin(TL2_B_GPIO_Port, TL2_B_Pin, 1);
}

void traffic_set_pair(Traffic_Pair_State state) {
    switch (state) {
        case TL_PAIR_RED_GREEN:
            TL1_Set_Red();
            TL2_Set_Green();
            break;
        case TL_PAIR_RED_AMBER:
            TL1_Set_Red();
            TL2_Set_Yellow();
            break;
        case TL_PAIR_GREEN_RED:
            /* Set stopping road red before enabling the other road's green */
            TL2_Set_Red();
            TL1_Set_Green();
            break;
        case TL_PAIR_AMBER_RED:
            TL2_Set_Red();
            TL1_Set_Yellow();
            break;
        case TL_PAIR_ALL_RED:
            TL1_Set_Red();
            TL2_Set_Red();
            break;
        case TL_PAIR_ALL_AMBER:
            TL1_Set_Yellow();
            TL2_Set_Yellow();
            break;
        case TL_PAIR_TUNE_GREEN_ON:
            /* Hold road 2 red while road 1 blinks green so both are never green */
            TL2_Set_Red();
            TL1_Set_Green();
            break;
        case TL_PAIR_TUNE_GREEN_OFF:
            TL2_Set_Red();
            TL1_Set_Off();
            break;
        case TL_PAIR_OFF:
        default:
            TL1_Set_Off();
            TL2_Set_Off();
            break;
    }
}
