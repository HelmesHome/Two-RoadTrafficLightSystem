/*
 * traffic_light.h
 *
 *  Created on: Nov 13, 2026
 *      Author: Admin
 */

#ifndef INC_TRAFFIC_LIGHT_H_
#define INC_TRAFFIC_LIGHT_H_

typedef enum {
    TL_PAIR_OFF = 0,
    TL_PAIR_RED_GREEN = 1,
    TL_PAIR_RED_AMBER = 2,
    TL_PAIR_GREEN_RED = 3,
    TL_PAIR_AMBER_RED = 4,
    TL_PAIR_ALL_RED = 5,
    TL_PAIR_ALL_AMBER = 6,
    TL_PAIR_TUNE_GREEN_ON = 7,
    TL_PAIR_TUNE_GREEN_OFF = 8
} Traffic_Pair_State;

void TL1_Set_Red(void);
void TL1_Set_Green(void);
void TL1_Set_Yellow(void);
void TL1_Set_Off(void);

void TL2_Set_Red(void);
void TL2_Set_Green(void);
void TL2_Set_Yellow(void);
void TL2_Set_Off(void);

void traffic_set_pair(Traffic_Pair_State state);

#endif /* INC_TRAFFIC_LIGHT_H_ */
