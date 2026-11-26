/*
 * dht20.h
 *
 *  Created on: Nov 13, 2026
 *      Author: Admin
 */

#ifndef INC_DHT20_H_
#define INC_DHT20_H_

#include "main.h"

typedef struct {
    float Temperature;
    float Humidity;
    uint8_t valid;
    uint8_t error_count;
} DHT20_Data_t;

extern DHT20_Data_t DHT20_Data;

void DHT20_Init(void);
void Task_DHT20_Trigger(void);
void Task_DHT20_Fetch_And_Print(void);

#endif /* INC_DHT20_H_ */
