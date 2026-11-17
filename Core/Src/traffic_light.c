/*
 * traffic_light.c
 *
 *  Created on: Nov 13, 2026
 *      Author: Admin
 */

#include"traffic_light.h"
#include"main.h"
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

//traffic_set(1,RED)










