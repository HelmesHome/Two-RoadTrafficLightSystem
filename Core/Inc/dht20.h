/*
 * dht20.h
 *
 *  Created on: Nov 13, 2026
 *      Author: Admin
 */

#ifndef INC_DHT20_H_
#define INC_DHT20_H_

#include "main.h"

// Cấu trúc lưu trữ dữ liệu cảm biến
typedef struct {
    float Temperature;
    float Humidity;
} DHT20_Data_t;

extern DHT20_Data_t DHT20_Data; // Biến toàn cục chứa kết quả đo

// Khai báo hàm
void Task_DHT20_Trigger(void);
void Task_DHT20_Fetch_And_Print(void);

#endif /* INC_DHT20_H_ */
