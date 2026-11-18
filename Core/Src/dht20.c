/*
 * dht20.c
 *
 *  Created on: Nov 13, 2026
 *      Author: Admin
 */


#include <lcd.h>
#include "dht20.h"
#include "main.h"
#include "scheduler.h"
#include <stdio.h>

extern I2C_HandleTypeDef hi2c1;

#define DHT20_ADDR (0x38)
DHT20_Data_t DHT20_Data;

// Hàm này được cấu hình để chạy mỗi 2 giây: SCH_Add_Task(Task_DHT20_Trigger, 0, 200);
void Task_DHT20_Trigger(void) {
    uint8_t tx_data[3] = {0xAC, 0x33, 0x00};
    HAL_I2C_Master_Transmit(&hi2c1, (DHT20_ADDR << 1) , tx_data, 3, 100);
    SCH_Add_Task(Task_DHT20_Fetch_And_Print, 8, 0);
}


void Task_DHT20_Fetch_And_Print(void) {
    uint8_t rx_data[7];
    char buffer[16];
    if (HAL_I2C_Master_Receive(&hi2c1, (DHT20_ADDR << 1), rx_data, 7, 100) == HAL_OK) {
        if ((rx_data[0] & 0x80) == 0x80) {
            return;
        }
        uint32_t raw_humidity = rx_data[1];
        raw_humidity = (raw_humidity << 8) | rx_data[2];
        raw_humidity = (raw_humidity << 8) | rx_data[3];
        raw_humidity = raw_humidity >> 4;
        DHT20_Data.Humidity = (float)raw_humidity * 100 / 1048576.0f;

        uint32_t raw_temp = rx_data[3] & 0x0F;
        raw_temp = (raw_temp << 8) | rx_data[4];
        raw_temp = (raw_temp << 8) | rx_data[5];

        DHT20_Data.Temperature = (float)raw_temp * 200 / 1048576.0f - 50.0f;


        lcd_put_cur(1, 0);
        sprintf(buffer, "T:%.1fC H:%.1f%%", DHT20_Data.Temperature, DHT20_Data.Humidity);
        lcd_send_string(buffer);
    }
}
